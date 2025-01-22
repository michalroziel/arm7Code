/********************************************************************/
/*  Hochschule für Technik und Wirtschaft                          */
/*  Fakultät für Ingenieurwissenschaften                           */
/*  Labor für Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                          */
/********************************************************************/
/*                                                                  */
/*  C_Übung.C:                                                      */
/*	  Programmstruktur für C-Programme mit dem Keil                */
/*    Entwicklungsprogramm uVision für ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *          Versuch 3                       */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	     *                                          */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *        Valentin Straßer    5014379       */
/*                       *        Michal Roziel       5012845       */
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *        23.01.2025                        */
/*                       *                                          */
/********************************************************************/

#include <LPC21xx.H>  /* LPC21xx Definitionen */
#include "C_Uebung.H"

/*** Globale Konstanten und Variablen ***/
#define PCLOCK          12.5
#define SWITCH_MASK1    0x10000  // Schalter an P0.16
#define SWITCH_MASK2    0x20000  // Schalter an P0.17
#define SWITCH_MASK3    0x1000000 // Schalter an P1.25
#define INPUT_MASK      0x3C00   // Eingabemaske für BCD
static const unsigned int baudrates[] = {110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600};

static unsigned int sek = 0;        // Sekunden-Zähler
static unsigned int tasterzustand = 0;

/*** Funktionsprototypen ***/
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendChar(char data);
void uartSendString(char* str);
char uartReadChar(void);

unsigned int readInputBCD(void);
unsigned int readSwitchState1(void);
unsigned int readSwitchState2(void);
unsigned int readSwitchState3(void);
unsigned int initBaudrate(void);

void initTimer(void);
void initExIn(void);
void myEXTINT(void) __irq;
void T0isr(void) __irq;

void sendMenu(void);
void sendTime(int time);
void sendInt(int value);

/*** Implementierungen ***/

/* UART Initialisierung */
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable) {
    unsigned int uartConfig = (paritySelect << 1) | parityEnable;
	  unsigned int divisor = PCLOCK / (16 * baudRate);
    uartConfig = (uartConfig << 1) | stopBits;
    uartConfig = (uartConfig << 2) | dataBits;

    PINSEL0 |= 0x05;  // P0.8 = TxD1, P0.9 = RxD1 aktivieren
    U0LCR = uartConfig | 0x80;  // DLAB setzen
    U0DLL = divisor % 256;
    U0DLM = divisor / 256;
    U0LCR &= ~0x80;  // DLAB zurücksetzen
    U0FCR = 0x07;  // FIFO aktivieren
}

/* UART Zeichen senden */
void uartSendChar(char data) {
    while (!(U0LSR & 0x20));  // Warte, bis das Transmit-Register bereit ist
    U0THR = data;
}

/* UART String senden */
void uartSendString(char* str) {
    while (*str) {
        uartSendChar(*str++);
    }
}

/* UART Zeichen empfangen */
char uartReadChar(void) {
    while (!(U0LSR & 0x01));  // Warte, bis ein Zeichen empfangen wurde
    return U0RBR;
}

/* Schalterzustand lesen */
unsigned int readSwitchState1(void){
	return (IOPIN0 & SWITCH_MASK1) >> 16;
}

unsigned int readSwitchState2(void){
	return (IOPIN0 & SWITCH_MASK2) >> 17;
}

unsigned int readSwitchState3(void){
	return (IOPIN1 & SWITCH_MASK3) >> 25;
}

/* BCD Eingang lesen */
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;
}

/* Baudrate initialisieren */
unsigned int initBaudrate(void) {
    unsigned int index = readInputBCD();
    return (index > 9) ? baudrates[9] : baudrates[index];
}

/* Timer Initialisierung */
void initTimer(void) {
    T0PR = 12500;  // Prescaler
    T0TCR = 0x02;  // Timer zurücksetzen
    T0MCR = 0x03;  // Interrupt und Reset
    T0MR0 = 1000;  // 1 Sekunde
    T0TCR = 0x00;  // Timer anhalten

    VICVectAddr4 = (unsigned long)T0isr;
    VICVectCntl4 = 0x24;
    VICIntEnable |= 0x10;
}

/* Interrupt Initialisierung */
void initExIn(void) {
    PINSEL0 |= 0x80000000;  // EINT2 aktivieren
    EXTMODE |= 0x04;  // Flankengetriggerter Interrupt
    VICVectCntl0 = 0x30;  // Kanal 16 aktivieren
    VICVectAddr0 = (unsigned long)myEXTINT;
    VICIntEnable = 0x10000;  // EINT2 aktivieren
}

/* Menüs senden */
void sendMenu(void) {
    uartSendString("\r\nStopp-Uhr\r\n");
    uartSendString("s/S - Start/Stop\r\n");
    uartSendString("a/A - Anzeigen\r\n");
    uartSendString("r/R - Reset\r\n");
}

/* Zeit senden */
void sendTime(int time) {
    int h = time / 3600;
    int m = (time % 3600) / 60;
    int s = time % 60;
    uartSendString("Zeit: ");
    sendInt(h); uartSendChar(':');
    sendInt(m); uartSendChar(':');
    sendInt(s); uartSendString("\r\n");
}

/* Integer senden */
void sendInt(int value) {
    char buffer[5];
    int i = 0;
    if (value == 0) buffer[i++] = '0';
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    while (i--) uartSendChar(buffer[i]);
}

/* Externer Interrupt Handler */
void myEXTINT(void) __irq {
    if (tasterzustand > 0) {
        T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;
        uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");
        tasterzustand = 0;
    } else {
        tasterzustand++;
    }
    EXTINT = 0x04;
    VICVectAddr = 0x00;
}

/* Timer Interrupt Handler */
void T0isr(void) __irq {
    sek++;
    T0IR |= 0x10;
    VICVectAddr = 0x00;
}

/* Hauptprogramm */
int main(void) {
    char choice;
    uartInit(initBaudrate(),3,readSwitchState3(),readSwitchState2(),readSwitchState1());
    initExIn();
    initTimer();

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
                if (T0TCR==0x01){						// Wenn Timer läuft
													uartSendString ("Sie muessen die Stoppuhr erst anhalten, erst dann duerfen Sie sie zuruecksetzen");
													uartSendString("\r\n");
									} else {						// sonst
													T0TCR=0x02;			   				// Timer rücksetzen
													sek = 0;	
													uartSendString("Timer wurde erfolgreich zurückgesetzt!");
									}
                break;
            default:
                uartSendString("Ungültige Eingabe!\r\n");
                break;
        }
				if (sek == 215999){
						T0TCR=0x00;									// Timer angehalten
						T0TCR=0x02;			   						// Timer rücksetzen
						sek = 0;								// Sekunden zurücksetzen
						uartSendString ("aktuelle Timer-Zeit: 59:59:59. Stoppuhr wurde angehalten und zurueckgesetzt");
						uartSendString("\r\n");
		}
    }
}
