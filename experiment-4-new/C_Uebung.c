/********************************************************************/
/* Hochschule fuer Technik und Wirtschaft                          */
/* Fakultät fuer Ingenieurwissenschaften                           */
/* Labor fuer Eingebettete Systeme                                 */
/* Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Übung.C:                                                      */
/*    Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Name / Matrikel-Nr.: *      Valentin Straßer                    */
/*                       *      Michal Roziel                       */
/********************************************************************/
/*  Abgabedatum:         *      16.01.2025                          */
/********************************************************************/

#include <LPC21xx.H>  // LPC21xx Mikrocontroller Definitionen
#include "C_Uebung.H"

// UART initialisieren
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable) {
    unsigned int uartConfig = 0;
    unsigned int Frequenzteiler;

    // UART-Konfiguration erstellen
    uartConfig = (paritySelect << 1) + parityEnable;
    uartConfig = (uartConfig << 1) + stopBits;
    uartConfig = (uartConfig << 2) + (dataBits-5);

    // UART0 an P0.0 (TxD0) und P0.1 (RxD0) aktivieren
    PINSEL0 |= 0x05;

    // Baudratenteiler berechnen
    Frequenzteiler = PCLOCK / (16 * baudRate);

    // UART-Register konfigurieren
    U0LCR = DLAB_BIT | uartConfig;          // DLAB-Bit setzen, UART-Konfiguration
    U0DLL = Frequenzteiler % 256;          // Niedriges Byte des Baudratenteilers
    U0DLM = Frequenzteiler / 256;          // Hohes Byte des Baudratenteilers
    U0LCR = uartConfig;                    // DLAB-Bit löschen
    U0FCR = UART_FIFO_ENABLE;              // FIFO aktivieren
}

// Schalterzustand von P0.16 lesen
unsigned int readSwitchState1(void) {
    return (IOPIN0 >> 16) & 1;
}

// Schalterzustand von P0.17 lesen
unsigned int readSwitchState2(void) {
    return (IOPIN0 >> 17) & 1;
}

// Schalterzustand von P1.25 lesen
unsigned int readSwitchState3(void) {
    return (IOPIN1 >> 25) & 1;
}

// BCD-Eingang von P0.10-P0.13 lesen
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;
}

// Menü über UART senden
void sendMenu(void) {
    uartSendString("\r\nStopp-Uhr\r\n");
    uartSendString("\tStart und Anhalten durch Druecken der Interrupt-Taste\r\n");
    uartSendString("\ts,S - Start/Stop\r\n");
    uartSendString("\ta,A - Anzeigen\r\n");
    uartSendString("\tr,R - Reset\r\n");
}

// Baudrate initialisieren basierend auf BCD-Eingang
unsigned int initBaudrate(void) {
    unsigned int index = readInputBCD();
    return (index > 9) ? baudrates[9] : baudrates[index];
}

// Einzelnes Zeichen über UART senden
void uartSendChar(char data) {
    while ((U0LSR & UART_READY_BIT) == 0);  // Warten, bis UART bereit ist
    U0THR = data;  // Zeichen senden
}

// String über UART senden
void uartSendString(char* str) {
    int i = 0;
    while (str[i] != '\0') {  // Bis zur Nullterminierung
        uartSendChar(str[i]);  // Zeichenweise senden
        i++;
    }
}

// Zeichen über UART empfangen
char uartReadChar(void) {
    while (!(U0LSR & UART_RX_READY));  // Warten, bis ein Zeichen empfangen wurde
    return U0RBR;  // Empfangenes Zeichen zurückgeben
}

// Timer initialisieren
void initTimer(void) {
    T0PR = 12500;  // Prescaler für den Timer
    T0TCR = 0x02;  // Timer zurücksetzen
    T0MCR = 0x03;  // Interrupt und Reset bei Übereinstimmung
    T0MR0 = 1000;  // 1 Sekunde
    T0TCR = 0x00;  // Timer anhalten

    // Interrupt-Konfiguration
    VICVectAddr4 = (unsigned long)T0isr;
    VICVectCntl4 = 0x24;
    VICIntEnable |= 0x10;
}

// Externen Interrupt initialisieren
void initExIn(void) {
    PINSEL0 |= 0x80000000;  // EINT2 aktivieren
    EXTMODE |= 0x04;        // Flankengesteuerter Interrupt
	  EXTPOLAR |= 0x04;
	  EXTINT 	|= 0x04;
    VICVectCntl0 = 0x30;    // Kanal 16 aktivieren
    VICVectAddr0 = (unsigned long)myEXTINT;
    VICIntEnable = 0x10000; // EINT2 aktivieren
}

// Integer über UART senden
void sendInt(int value) {
    char buffer[5];
    int i = 0;

    // Sonderfall: Wert ist 0
    if (value == 0) {
        buffer[i++] = '0';
    }

    // Zahl in umgekehrter Reihenfolge in den Buffer schreiben
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    // Zahl korrekt herum ausgeben
    while (i--) {
        uartSendChar(buffer[i]);
    }
}

// Externer Interrupt-Handler
void myEXTINT(void) __irq {
	
   	T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;  // Timer starten oder anhalten
	  uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");

    EXTINT = 0x04;  // Interrupt-Flag löschen
    VICVectAddr = 0x00;
}

// Timer-Interrupt-Handler
void T0isr(void) __irq {
    sek++;
    IOCLR0 = SEGMENT;            // Anzeige zurücksetzen
    IOSET0 = bcd[sek % 10];      // Anzeige aktualisieren
    T0IR |= 0x10;                // Interrupt-Flag löschen
    VICVectAddr = 0x00;
}

// Zeit über UART senden
void sendTime(int time) {
    int h = time / 3600;
    int m = (time % 3600) / 60;
    int s = time % 60;

    uartSendString("Zeit: ");
    if (h < 10) sendInt(0);
    sendInt(h);
    uartSendChar(':');
    if (m < 10) sendInt(0);
    sendInt(m);
    uartSendChar(':');
    if (s < 10) sendInt(0);
    sendInt(s);
    uartSendString("\r\n");
}

// Siebensegmentanzeige initialisieren
void initSeg(void) {
    IODIR0 = SEGMENT;  // P0.18-P0.24 als Ausgang setzen
    IOCLR0 = SEGMENT;  // Ausgang zurücksetzen
    IOSET0 = bcd[0];   // Anzeige auf 0 setzen
}

// Hauptprogramm
int main(void) {
    char choice;
    sek = 215990;  // Initiale Zeit setzen

    // Initialisierungen
    uartInit(initBaudrate(), 8, readSwitchState3(), readSwitchState2(), readSwitchState1());
    initSeg();
    initExIn();
    initTimer();

    // Hauptschleife
    while (1) {
        sendMenu();
        choice = uartReadChar();

        switch (choice) {
            case 's': case 'S':
                T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;
                uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");
                break;
            case 'a': case 'A':
                sendTime(sek);
                break;
            case 'r': case 'R':
                if (T0TCR == 0x01) {
                    uartSendString("Sie muessen die Stoppuhr erst anhalten, erst dann duerfen Sie sie zuruecksetzen\r\n");
                } else {
                    T0TCR = 0x02;  // Timer zurücksetzen
                    sek = 0;
                    uartSendString("Timer wurde erfolgreich zurueckgesetzt!\r\n");
                }
                break;
            default:
                uartSendString("Ungültige Eingabe!\r\n");
                break;
        }

        // Zeitgrenze prüfen und zurücksetzen
        if (sek >= 215999) {
            T0TCR = 0x00;  // Timer anhalten
            T0TCR = 0x02;  // Timer zurücksetzen
            sek = 0;
            uartSendString("aktuelle Timer-Zeit: 59:59:59. Stoppuhr wurde angehalten und zurueckgesetzt\r\n");
        }
    }
}
