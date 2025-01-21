/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakult�t fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_�bung.C:                                                      */
/*	  Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
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

#include <LPC21xx.H>		/* LPC21xx Definitionen                 */
#include "C_Uebung.H"

#define			PCLOCK 12.5
#define 		SWITCH_MASK1      0x10000                // Schalter an P0.16 und P0.17
#define 		SWITCH_MASK2      0x20000
#define			SWITCH_MASK3				0x1000000
#define 		INPUT_MASK       0x3C00     		        // Eingabemaske für BCD
static const unsigned int baudrates[] = {
    110,     // 0
    300,     // 1
    600,     // 2
    1200,    // 3
    2400,    // 4
    4800,    // 5
    9600,    // 6
    19200,   // 7
    38400,   // 8
    57600    // 9
};

void initUart(void);

unsigned initBaudrate(void);

void initBCD(void);
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendString(char* str);
void uartSendChar(char data);
unsigned int readSwitchState1(void);
unsigned int readSwitchState2(void);
unsigned int readSwitchState3(void);
void sendMenue(void);
char uartReadChar(void);

void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable) {
    unsigned int uartConfig = 0;
    unsigned int divisor;

    // Konfiguration des UART-Modus
    uartConfig = (8 + paritySelect);  // Parität
    uartConfig = (uartConfig << 1) + parityEnable;  
    uartConfig = (uartConfig << 1) + stopBits;  
    uartConfig = (uartConfig << 2) + dataBits;  

    PINSEL0 = PINSEL0 |0x05;  // P0.8 = TxD1, P0.9 = RxD1 für UART1 aktivieren

    divisor = PCLOCK / (16 * baudRate);  // Baudratenteiler berechnen
    U0LCR = uartConfig;  // DLAB-Bit setzen, 8 Datenbits, 1 Stoppbit
    U0DLL = divisor % 256;  // Niedriges Byte des Divisors
    U0DLM = divisor / 256;   // Hohes Byte des Divisors
    U0LCR = 0x1F;  // DLAB-Bit löschen, 8 Datenbits, 1 Stoppbit
    U0FCR = 0x07;  // FIFO aktivieren und zurücksetzen
}

// Eingabewert von Schaltern einlesen
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;  				        	// Bits 10-13 ausmaskieren
}

unsigned int initBaudrate(void){
		unsigned int index = readInputBCD();
		if(index > 9) index = 9;
		return baudrates[index];
	
}

// Zeichen über UART senden
void uartSendChar(char data) {
    while ((U0LSR & 0x20) == 0);  // Warte, bis das Transmit-Register bereit ist
    U0THR = data;  // Zeichen senden
}

// String über UART senden
void uartSendString(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        uartSendChar(str[i]);  // Zeichenweise senden
        i++;
    }
}

unsigned int readSwitchState1(void){
	return (IOPIN0 & SWITCH_MASK1) >> 16;
}

unsigned int readSwitchState2(void){
	return (IOPIN0 & SWITCH_MASK2) >> 17;
}

unsigned int readSwitchState3(void){
	return (IOPIN1 & SWITCH_MASK3) >> 25;
}
void sendMenue(){
	/* Start-Menue Senden */
	char newline[] = {0x0D, 0x0A};
	uartSendString(newline);
	uartSendString("Stopp-Uhr");
	uartSendString(newline);
	uartSendString("       Start und Anhalten durch Druecken der Interrupt-Taste");
	uartSendString(newline);
	uartSendString("       s,S - Start und Anhalten");
	uartSendString(newline);
	uartSendString("       a,A - Anzeigen des Standes");
	uartSendString(newline);
	uartSendString("       r,R - Ruecksetzen der Stoppuhr");
	uartSendString(newline);
}
char uartReadChar(void) {
    while (!(U0LSR & 0x01));  // Warte auf empfangenes Zeichen
    return U0RBR;  // Zeichen zurückgeben
}

int main(void){
   char choice;
   uartInit(initBaudrate(),3,readSwitchState3(),readSwitchState2(),readSwitchState1());
		

/* Hauptschleife */
while (1) {
   sendMenue();
	 choice = uartReadChar();
		if(choice == 's' || choice == 'S'){
			uartSendString("Timer wurde an aus");
		} else if (choice == 'a' || choice == 'A'){
			uartSendString("Timerstand wurde angezeigt");
		} else if (choice == 'r' || choice == 'R'){
			uartSendString("Timerstand wurde zurück gesetzt");
		} else {
       uartSendString("Falsche Eingabe!");
          
      }
  }
}
