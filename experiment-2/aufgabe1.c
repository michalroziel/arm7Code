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
#define LED 		0xFF0000
#define S				0x30000


unsigned long bcd[10]={0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
 0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000};

 unsigned long schalter ;
 
 unsigned int ledLaufIndex; 
 unsigned int eingabe;  // Deklaration der Variable nach oben verschoben
 
 
 
static unsigned int periode= 1000;


    void delay(void){

        	T0PR=12500; 				// Prescaler: 12.5 Mhz = 1/1000 * 12.5 Mhz = 12500 Herz

	        T0TCR=0x02;			   		// Timer rücksetzen
	        T0MCR=0x03;					// IR und Reset
	        T0MR0=periode;				// 1 Sekunde

	        T0TCR=0x01;	
    }
		
		
		
		void T0isr(void) __irq{
			
			unsigned long time;
			time = (IOPIN0 & 0x400000);
		
			if (time > 0){
		
				IOCLR0 = 0x400000;
		} 
		
		else {
		IOSET0 = 0x400000;
	}
		T0EMR=T0EMR |0x02;		// Pin-Match auf 1 setzen
		T0IR |= 0x10;			// Ruecksetzen des Interrupt Flags
		VICVectAddr=0x00;		// IR Ende
	}


 

int main(void){
	
										
		IODIR0=SEGMENT;  		       				// Ausgang von P0.18-P.0.24 (BCD Anzeige) - SETZT 
		IOCLR0=SEGMENT;	       	        	// Setzt Low-Spannungspegel an P0.18-P0.24 
		IOSET0=bcd[0];										// Initialisiert den BCD auf 0 	

		IODIR1=LED;												// Ausgang von P0.16-P0.19 (LEDs) - SETZT
		IOCLR1=LED;												// Setzt Low-Spannungspegel an P0.16-P0.19
		
		IODIR0 = S;


	 		
while(1) {
    IOCLR0 = SEGMENT;  // Setzt Low-Spannungspegel an P0.18-P0.24
    
    schalter = (IOPIN0 & 0x10000) >> 16;
    
 
    eingabe = (IOPIN0 >> 10) & 0xf;  // Zuweisung
    
    if (eingabe > 9) eingabe = 9;  // Eingabe begrenzen
    
    if (schalter) {
        IOSET0 = bcd[eingabe];  // Setzt High-Spannungspegel am BCD
    }

		
		
		
		schalter = (IOPIN0  & 0x20000)>>16;
		
			if ( schalter ){
			
					ledLaufIndex  = (ledLaufIndex<< 1) + 1  ;				
					
			} 
			
			else {
			
			ledLaufIndex = 0;
				
			}
		
			

			IOSET1=(ledLaufIndex<<16);								// Setzt Highspannungspegel an P0.16-P0.19
			 
			 

     }
}



