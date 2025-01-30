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
/*  Name / Matrikel-Nr.: *      Valentin Straßer    5014379         */
/*                       *      Michal Roziel       5012845         */
/********************************************************************/
/*  Abgabedatum:         *      30.01.2025                          */
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
    U0DLL = Frequenzteiler % 256;           // Niedriges Byte des Baudratenteilers
    U0DLM = Frequenzteiler / 256;           // Hohes Byte des Baudratenteilers
    U0LCR = uartConfig;                     // DLAB-Bit löschen
    U0FCR = UART_FIFO_ENABLE;               // FIFO aktivieren
}

// Schalterzustand von P0.16 -> S1 lesen
unsigned int readSwitchState1(void) {
    return (IOPIN0 >> 16) & 1;
}

// Schalterzustand von P0.17 -> S2 lesen
unsigned int readSwitchState2(void) {
    return (IOPIN0 >> 17) & 1;
}

// Schalterzustand von P1.25 -> S3 lesen
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
		// Wenn Index größer als 9 ist, baudrates[9] verw.
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
    return U0RBR;               // Empfangenes Zeichen zurückgeben
}

// Timer initialisieren
void initTimer(void) {
    T0PR = 12500;               // Prescaler für den Timer - 12.5 MHz
    T0TCR = 0x02;               // Timer zurücksetzen mit Bit 1
    T0MCR = 0x03;               // Interrupt und Reset bei Übereinstimmung
    T0MR0 = 1000;               // 1000 ms 1 Interrupt 
    T0TCR = 0x00; 						  // Timer anhalten mit Timer Control Reg.

    // Interrupt-Konfiguration
    VICVectAddr4 = (unsigned long)T0isr; // addr. von isr -> VIC
		VICVectCntl4 = 0x24;				// VICVecCntl Bit 5 & 4 : Kanal4
    VICIntEnable |= 0x10;
}

// Externen Interrupt initialisieren
void initExIn(void) {
    PINSEL0  |= 0x80000000;      // EINT2 aktivieren -> Func. 01
    EXTMODE  |= 0x04;       		 // Flankengesteuerter INT. Bit 2
	  EXTPOLAR |= 0x04;            // EINT2 -> rising edge
	  EXTINT 	 |= 0x04;            // Interrupt Flag 
	  VICVectCntl0 = 0x30;    		 // Kanal 16 : Bit5 :VekKanal, 4: Int.ID
    VICVectAddr0 = (unsigned long)myEXTINT; // save addr. der isr
    VICIntEnable = 0x10000; 		 // EINT2 aktivieren
}

// Integer über UART senden
void sendInt(int value) {
    char buffer[5]; 						// Bufffer um Int. int richtig. Reih. zu senden
    int i = 0;									// Laufindex 

																// Sonderfall: Wert ist 0, da sonst endlos 
    if (value == 0) {
        buffer[i++] = '0';      // manuelles zeichen
    }

																// Zahlen in umgekehrter Reihenfolge 
																//in den Buffer schreiben
    while (value > 0) {					// mod extrahiert letzte ziffer
																// '0' -> ASCII Zeichen
        buffer[i++] = (value % 10) + '0';
        value /= 10;						// Ganzzahlige Integer Division entf. letzt. Z.				
    }

																// Zahl korrekt herum ausgeben
    while (i--) {								// Rückwärts durch Buffer zählen 
        uartSendChar(buffer[i]);
    }
}

// Externer Interrupt-Handler
void myEXTINT(void) __irq {
	
   	T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;  
																// Timer starten oder anhalten
																// Falls Timer Läuft Stoppen, sonst starten
																// bei jedem EINT2 wechselt der Timer 
																
																// Message ob Timer läuft oder nicht 
																					
	  uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");

    EXTINT = 0x04;  						 // Interrupt-Flag löschen -> Setzt EINT2 zurück 
																 // Sonst : EINT2 kann nicht erneut ausgeführt werden
    VICVectAddr = 0x00;          // Signalisiert VIC, dass isr fertig 
}

// Timer-Interrupt-Handler
void T0isr(void) __irq {
    sek++;											 // vergangene Zeit speichern 
    IOCLR0 = SEGMENT;            // Anzeige zurücksetzen
	
    IOSET0 = bcd[sek % 10];      // EinerDarstellung der aktuellen Sek.
	
    T0IR |= 0x10;                // Interrupt-Flag löschen
    VICVectAddr = 0x00;          // VIC signalisieren, dass isr fertig 
}

// Zeit über UART senden
void sendTime(int time) {       
    int h = time / 3600;         // Stunden berechnen 
    int m = (time % 3600) / 60;  // Minuten berechnen  
    int s = time % 60;           // Sekunden berechnen 

    uartSendString("Zeit: ");
    if (h < 10) sendInt(0);      // falls h< 10, führende Null 
    sendInt(h);                  // h senden
    uartSendChar(':');
    if (m < 10) sendInt(0);      // falls m < 10, führende Null 
    sendInt(m);
    uartSendChar(':');
    if (s < 10) sendInt(0);      // falls s < 10, führende Null
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
    sek = 215990;      // Initiale Zeit setzen

    // Initialisierungen 
    uartInit(initBaudrate(), 8, readSwitchState3(), readSwitchState2(), readSwitchState1());
											 // UART : baudrate, datenbits, stoppbits, parity 
    initSeg();
    initExIn();
    initTimer();

    // Hauptschleife
    while (1) {
        sendMenu();    // menu senden
        choice = uartReadChar();

        switch (choice) {
            case 's': case 'S':
												// timer start-stop
                T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;
                uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");
                break;
            case 'a': case 'A':
												// Zeit anzeigen lassen
                sendTime(sek);
                break;
												// Timer-Reset
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
