#ifndef __C_UEBUNG
#define __C_UEBUNG

// Masken und Konstanten
#define 		SEGMENT_MASK     0x1FC0000 	        		// 7-Segment-Anzeige an P0.18-P0.23
#define 		LED_MASK         0xFF0000   	        	// LEDs an P1.16-P1.23
#define 		SWITCH_MASK      0x30000                // Schalter an P0.16 und P0.17
#define 		INPUT_MASK       0x3C00     		        // Eingabemaske für BCD
#define 		BCD_MAX          9                      // Maximalwert der BCD-Anzeige
#define 		LED_MAX          0xFF     	            // Maximales LED-Muster
#define			PERI_TAKT				 12500									// Takt der Peripherie Komponenten 12500

// BCD-Codes für die Zahlen 0-9 auif dem 7 Segement Display
static const unsigned long BCD_CODES[10] = {
    0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
    0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000
};


	
// Funktionsprototypen
void initLED(void);        							   				  // LED-Initialisierung
void initBCD(void);                                 // BCD-Anzeige-Initialisierung
void updateBCD(unsigned int value); 			          // BCD-Anzeige aktualisieren
void updateLEDs(unsigned int pattern);              // LEDs aktualisieren
unsigned int readInputBCD(void);                    // Eingabe einlesen
unsigned int readSwitchState(void);                         // Schalter-Status einlesen
void initTimer(void);                      				  // Timer initialisieren
void T0isr(void) __irq;                    				  // Timer-Interrupt-Service-Routine

