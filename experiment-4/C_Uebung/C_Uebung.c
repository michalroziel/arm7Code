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
#define 		SWITCH_MASK12      0x30000                // Schalter an P0.16 und P0.17
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



// Eingabewert von Schaltern einlesen
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;  				        	// Bits 10-13 ausmaskieren
}

unsigned int initBaudrate(void){
		unsigned int index = readInputBCD();
		if(index > 9) index = 9;
		return baudrates[index];
	
}

// Schalter-Status einlesen
unsigned int readSwitchState(void) {
    return (IOPIN0 & SWITCH_MASK12) >> 16;  // Bits 16-17 ausmaskieren
}

void initUart(void) {
    unsigned long Frequenzteiler = PCLOCK / (16 * initBaudrate()); // Calculate the frequency divider
    U0DLL = Frequenzteiler % 256; // Low-byte (remainder of division by 256)
    U0DLM = Frequenzteiler / 256; // High-byte (integer division by 256)
	  U0LCR = 0x03; /* DLAB-Bit loeschen */
    U0FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */
}

int main(void){
  unsigned int data;
  /* Port-Pins konfigurieren */
    PINSEL0 = PINSEL0 |0x05; /* 		UART0 Initialisierung 		*/
		initUart();
	  initBaudrate();

/* Hauptschleife */
while (1) {
  /* test auf Datenempfang */
    if ((U0LSR & 0x01) != 0) {
      data = U0RBR; /* empfangene Daten auslesen */
    /* versenden der Daten */
      if ((U0LSR & 0x20) != 0) { /* Senderegister frei? */
      U0THR = data; /* eintragen in sende-FIFO */
      }
    }
  }
}
