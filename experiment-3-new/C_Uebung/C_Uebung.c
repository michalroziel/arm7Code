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
	  PINSEL0 = PINSEL0 | 0x50000;/* P0.8=TxD, P0.9=RxD UART1 */
    unsigned int Frequenzteiler = PCLOCK / (16 * BaudRate); // Calculate the frequency divider
		U1LCR = komparameter;
    U1DLL = Frequenzteiler % 256; // Low-byte (remainder of division by 256)
    U1DLM = Frequenzteiler / 256; // High-byte (integer division by 256)
	  U1LCR = 0x03; /* DLAB-Bit loeschen */
    U1FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */
}

void sendchar (unsigned char daten){
	while ((U1LSR & 0x20) == 0);
	if ( (U1LSR & 0x20) != 0 ) {
	   		U1THR = daten;
	   }
}

void sendchars(char* daten) {
    int j = 0;

    while (daten[j] != '\0') {  // Läuft durch den String, bis das Nullterminierungszeichen erreicht wird
        while ((U1LSR & 0x20) == 0);  // Wartet, bis das U0THR bereit ist
        if ((U1LSR & 0x20) != 0) {
            U1THR = daten[j];  // Überträgt das aktuelle Zeichen
        }
        j++;  // Gehe zum nächsten Zeichen
    }
}


int main(void){
  unsigned int data;
  /* Port-Pins konfigurieren */
   
		initUart();
	  sendchars();
/* Hauptschleife */
while (1) {
  /* test auf Datenempfang */
    if ((U1LSR & 0x01) != 0) {
      data = U0RBR; /* empfangene Daten auslesen */
    /* versenden der Daten */
      if ((U1LSR & 0x20) != 0) { /* Senderegister frei? */
      U1THR = data; /* eintragen in sende-FIFO */
      }
    }
  }
}
