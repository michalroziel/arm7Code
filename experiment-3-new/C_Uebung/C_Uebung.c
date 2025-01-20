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

#define	PCLOCK 12.5
#define	BAUDRATE	4800



void initUart(unsigned int BaudRate, unsigned int DatenBits,unsigned int StoppBits,unsigned int ParitätAuswahl,unsigned int ParitätAktivierung);

void sendchar (unsigned char daten);

void initUart(unsigned int BaudRate, unsigned int DatenBits,unsigned int StoppBits,unsigned int ParitätAuswahl,unsigned int ParitätAktivierung) {
	  unsigned int komparameter = ((((((0b1000 +ParitätAuswahl)<<1)+ParitätAktivierung)<<1)+StoppBits)<<2)+DatenBits;
	  PINSEL0 = PINSEL0 |0x05; /* 		UART0 Initialisierung 		*/
    unsigned int Frequenzteiler = PCLOCK / (16 * BaudRate); // Calculate the frequency divider
		U0LCR = komparameter;
    U0DLL = Frequenzteiler % 256; // Low-byte (remainder of division by 256)
    U0DLM = Frequenzteiler / 256; // High-byte (integer division by 256)
	  U0LCR = 0x03; /* DLAB-Bit loeschen */
    U0FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */
}

void sendchar (unsigned char daten){
	while ((U0LSR & 0x20) == 0);
	if ( (U0LSR & 0x20) != 0 ) {
	   		U0THR = daten;
	   }
}

int main(void){
  unsigned int data;
  /* Port-Pins konfigurieren */
   
		initUart();

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
