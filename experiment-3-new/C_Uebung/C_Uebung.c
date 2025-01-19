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


/*
int main(void) {
  unsigned int frequenzteiler = PCLOCK / (16 * BAUDRATE);
    while (1) {

t}
*/
void initUart(void);

void initUart(void) {
    unsigned long Frequenzteiler = PCLOCK / (16 * BAUDRATE); // Calculate the frequency divider
    U0DLL = Frequenzteiler % 256; // Low-byte (remainder of division by 256)
    U0DLM = Frequenzteiler / 256; // High-byte (integer division by 256)
	  U0LCR = 0x03; /* DLAB-Bit loeschen */
    U0FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */
}

int main(void){
  unsigned int data;
  /* Port-Pins konfigurieren */
    PINSEL0 = PINSEL0 | 0x50000;/* P0.8=TxD, P0.9=RxD UART1 */
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