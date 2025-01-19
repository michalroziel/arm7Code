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

#define	PCLOCK 12500
#define	BAUDRATE	4800

/*
int main(void) {
  unsigned int frequenzteiler = PCLOCK / (16 * BAUDRATE);
    while (1) {

    }
}
*/
int main(void){
  unsigned int data;
  /* Port-Pins konfigurieren */
    PINSEL0 = PINSEL0 | 0x50000;/* P0.8=TxD, P0.9=RxD UART1 */
  /* serielle Schnittstelle konfigurieren */
    U1LCR = 0x83; /* 8-Bit pro Datenbyte, 1 Stop-Bit,
  keine Parität, DLAB-Bit setzen */

  /* Divisor-Berechnung für 9600 Baud bei p-clock = 30Mhz
  30*10^6 / (9600*16) = 195 */
    U1DLL = 195; /* Baudrate setzen */
    U1DLM = 0;
    U1LCR = 0x03; /* DLAB-Bit loeschen */
    U1FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */

/* Hauptschleife */
while (1) {
  /* test auf Datenempfang */
    if ((U1LSR & 0x01) != 0) {
      data = U1RBR; /* empfangene Daten auslesen */
    /* versenden der Daten */
      if ((U1LSR & 0x20) != 0) { /* Senderegister frei? */
      U1THR = data; /* eintragen in sende-FIFO */
      }
    }
  }
}