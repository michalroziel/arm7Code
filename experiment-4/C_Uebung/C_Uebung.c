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

void myEXTINT(void) __irq;
void T0isr(void) __irq;
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

void sendint(int daten) {
    char data[5];  // Ein Integer hat maximal 5 Ziffern (bei einer Zahl im Bereich 0-9999).
    int i = 0;
    int j;
    // Fall 1: daten ist 0
    if (daten == 0) {
        data[i++] = '0';  // Wenn die Zahl 0 ist, setzen wir das erste Zeichen als '0'
    }
    // Fall 2: daten ist eine andere Zahl
    else {
        while (daten > 0) {
            data[i++] = (daten % 10) + '0';  // Nimm die letzte Ziffer und speichere sie als Zeichen
            daten /= 10;  // Entferne die letzte Ziffer
        }
    }
    // Umkehren der Ziffern (die Zahl ist nun rückwärts gespeichert)
    for (j = i - 1; j >= 0; j--) {
        // Warten, bis der UART bereit ist, Daten zu senden
        while ((U0LSR & 0x20) == 0);  // Warteschleife bis der UART bereit ist
        // Sende das Zeichen
        U0THR = data[j];
    }
}

void sendTime(int time) { // zur unwandlung für später
    int tempIA[3] = {0, 0, 0};

    if (time < 60) {  // < 60 Sekunden
        tempIA[2] = time;  // Sekunden
    } else if (time < 3600) {  // >= 60 Sekunden und < 3600 Sekunden (1 Stunde)
        tempIA[1] = time / 60;  // Minuten
        tempIA[2] = time % 60;  // Sekunden
    } else {  // >= 3600 Sekunden (1 Stunde oder mehr)
        tempIA[0] = time / 3600;  // Stunden
        tempIA[1] = (time % 3600) / 60;  // Minuten
        tempIA[2] = time % 60;  // Sekunden
    }
	uartSendString("Aktueller Stand: ");
	if (tempIA[0] < 10) uartSendChar('0');
	sendint(tempIA[0]);
	uartSendChar(':');
	if (tempIA[1] < 10) uartSendChar('0');
	sendint(tempIA[1]);
	uartSendChar(':');
	if (tempIA[2] < 10) uartSendChar('0');
	sendint(tempIA[2]);
	uartSendString("\r\n");
}

void myEXTINT(void) __irq{
	int i;
	static unsigned int tasterzustand = 0;
	if(tasterzustand > 0)
	{
	    if (T0TCR==0x01){									// Wenn Timer läuft
			T0TCR=0x00;										// Timer anhalten
			uartSendString ("Timer angehalten!");
			uartSendString("\r\n");
		} else {											// sonst
			T0TCR=0x01;										// Timer starten
			uartSendString ("Timer gestartet!");
			uartSendString("\r\n");
		}
		tasterzustand = 0;
	
		for(i=0; i<200000;i++);
		EXTINT=0x04;										// EINT2 flankenabhängig
	}
	else
	{
		tasterzustand += 1;
	}
    VICVectAddr=0x0;									// Interrupt beendet
}

void T0isr(void) __irq{
	static unsigned int sek = 0;
	++sek;
	T0EMR=T0EMR|0x02;									// Pin-Match auf 1 setzen
	T0IR|=0x10;											// Ruecksetzen des Interrupt Flags
	VICVectAddr=0x00;									// IR Ende
}
void initExIn(void){
	PINSEL0=PINSEL0|0x80000000;
	EXTMODE=EXTMODE|0x04;
	VICVectCntl0=0x30;  								// Kanal 16 aktivieren (S. 67)
  VICVectAddr0=(unsigned long) myEXTINT;
  VICIntEnable=0x10000;
}

void initTimer(void){
	T0PR=12500; 										// Prescaler: 12.5 Mhz = 1/1000 * 12.5 Mhz = 12500 Herz
	T0TCR=0x02;			   								// Timer rücksetzen
	T0MCR=0x03;											// IR und Reset
	T0MR0=1000;											// 1 Sekunden
	T0TCR=0x00;											// Timer anhalten
 	VICVectAddr4=(unsigned long) T0isr;					// isr Adresse
	VICVectCntl4=0x24;									// Aktiv für Kanal 4 (Timer 0)
	VICIntEnable=VICIntEnable |0x10;
}

int main(void){
   char choice;
   uartInit(initBaudrate(),3,readSwitchState3(),readSwitchState2(),readSwitchState1());
	 initExIn();
	 initTimer();
	 T0TCR=0x01;
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
