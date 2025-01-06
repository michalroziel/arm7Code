/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Übung.C:                                                      */
/*    Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *                                          */
/*                       *                                          */
/********************************************************************/
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *      Valentin Straßer                    */
/*                       *      Michal Roziel                       */
/*                       *                                          */
/********************************************************************/
/*  Abgabedatum:         *      16.01.2025                          */
/*                       *                                          */
/********************************************************************/

#include <LPC21xx.H>        /* LPC21xx Definitionen                 */

#define SEGMENT 0x1FC0000
#define LED 0xF0000

unsigned long bcd[10]={0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
 0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000};





int main(void){
									
	IODIR0=SEGMENT;  		       	// Ausgang von P0.18-P.0.24 (BCD Anzeige) - SETZT 
	IOCLR0=SEGMENT;	       	        	 // Setzt Low-Spannungspegel an P0.18-P0.24 
	IOSET0=bcd[0];				    // Initialisiert den BCD auf 0 	

	IODIR1=LED;				        // Ausgang von P0.16-P0.19 (LEDs) - SETZT
	IOCLR1=LED;				        // Setzt Low-Spannungspegel an P0.16-P0.19
	


	 		
     while(1){

	int eingabe=(IOPIN0>>10)&0xf;		// Nach rechts schieben um 10 Bit 

	if (eingabe>9) eingabe=9;		    // Wenn die Eingabe größer als 9 ist, dann setze die Eingabe auf 9
						                // (da nur 0-9 möglich sind nach Aufgabenstellung)			
	IOCLR0=SEGMENT;				        // Setzt Low-Spannungspegel an P0.18-P0.24
	IOSET0=bcd[eingabe];			    // Setzt Highspannungspegel am BCD 

	IOSET1=(eigabe<<16);			    // Setzt Highspannungspegel an P0.16-P0.1



     }




}



