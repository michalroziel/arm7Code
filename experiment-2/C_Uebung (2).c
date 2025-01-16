/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Uebung.C:                                                      */
/*    Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *                                          */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *      Valentin Straßer    5014379         */
/*                       *      Michal Roziel       5012845         */
/*                       *                                          */
/********************************************************************/
/*  Abgabedatum:         *      Donnerstag 16.01.2025               */
/*                       *                                          */
/********************************************************************/
#include <LPC21xx.H>

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

// LED-Ausgänge initialisieren
void initLED(void) {
    IODIR1 = LED_MASK;                      				// P1.16-P1.23 als Ausgang definieren
    IOCLR1 = LED_MASK;                      			  // Alle LEDs initial ausschalten
}

// BCD-Anzeige initialisieren
void initBCD(void) {
    IODIR0 = SEGMENT_MASK;                				  // P0.18-P0.24 als Ausgang definieren
    IOCLR0 = SEGMENT_MASK;                  				// BCD-Anzeige initial löschen
}

// BCD-Anzeige mit Wert aktualisieren
void updateBCD(unsigned int value) {
    if (value > BCD_MAX) value = BCD_MAX;   				// Wert begrenzen
    
    IOCLR0 = SEGMENT_MASK;                  				// Alte Anzeige löschen
    IOSET0 = BCD_CODES[value];             				  // Neuen Wert setzen
}

// LED-Muster aktualisieren
void updateLEDs(unsigned int pattern) {
    IOCLR1 = LED_MASK;      												// Alle LEDs ausschalten
    IOSET1 = (pattern << 16); 										  // Neues Muster auf P1.16-P1.23 setzen
}

// Eingabewert von Schaltern einlesen
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;  				        	// Bits 10-13 ausmaskieren
}

// Schalter-Status einlesen
unsigned int readSwitchState(void) {
    return (IOPIN0 & SWITCH_MASK) >> 16;  // Bits 16-17 ausmaskieren
}

// Timer-Interrupt-Service-Routine
void T0isr(void) __irq {										  		 // -- -> Compiler übernimmt save & load von Reg.
	unsigned int switchState = readSwitchState();
	// Volatile Variablen für ISR-Kommunikation
	static unsigned int g_ledPattern = 0;             // Aktuelles LED-Muster
																									 // irq weil wir keinen Fast Interrupt Request senden
																									 // LED-Muster bei aktivem Schalter aktualisieren
    if (switchState & 0x2) {										 // Schalter aktiv ?  -> LEDs werden aktualisiert
         g_ledPattern = (g_ledPattern << 1) + 1;   // Neues LED-Muster : verschieben des Bitmusters 
																									 // Addition 1 : nächste LED einschalten	 	
																								   
        if (g_ledPattern > LED_MAX) {							 // Bei Überschreitung zurücksetzen
            g_ledPattern = 0;											 // Wenn g_ledPattern das Muster überschreitet :
        }																					 // auf 0 setzen, um neu zu starten
        
        updateLEDs(g_ledPattern);									 // Übetragen des Musters auf ARM-MP 
				
    } else {																			 // Schalter nicht aktiv ? -> LEDs ausschalten
        updateLEDs(0);													   // Alle LEDs ausschalten
    }                                            
    
																									 // Interrupt-Flag des Timer0 zurücksetzen
    T0IR = 0x01;																	 // sonst : Timer läuft weiter
    VICVectAddr = 0x00;														 // Wir informieren VIC : Bearbeitung fertig		
}

// Timer initialisieren
void initTimer(void) {
// Timer-Konfiguration
    T0PR = PERI_TAKT-1;    													 // Prescaler für 1 kHz ( 1 MS pro Tick ) 
																									 // Timer zählt alle 12500 Takte einmal
    T0MR0 = 500;                                   // Match-Wert für 0.5 Sekunden ausgelöst
	
																									 // Match Control Register :
																									 // Bit 0 : Aktiviert Interrupt für M0
																									 // Bit 1 : Setzt Timer Zähler nach Match zurück
    T0MCR = 0x03;                                  // Interrupt und Reset bei Match
    T0TCR = 0x01;                                  // Timer starten

 // Interrupt-Konfiguration
    VICVectAddr4 = (unsigned long)T0isr;           // Speichern Adresse für ISR Kanal 0, 
																									 // T0isr als ISR für Timer 0 registriert
    VICVectCntl4 = 0x24;                           // Kanal 4 aktivieren - Bit 5
																									 // Quelle festlegen -> 4 für Timer 0
    VICIntEnable = 0x10;                           // Timer 0 Interrupt aktivieren
																									 // Jedes Bit : entsprechender Kanal
}

int main(void) {
    unsigned int bcdInput = 0;
	  unsigned int switchState = readSwitchState();
    
// Hardware initialisieren
    initBCD();
    initLED();
    initTimer();

    while (1) {
// Schalter-Status aktualisieren
				switchState = readSwitchState();	
// BCD-Anzeige aktualisieren
        if (switchState & 0x1) {
            bcdInput = readInputBCD();
            updateBCD(bcdInput);
        } else {
            IOCLR0 = SEGMENT_MASK; 			        	 // BCD-Anzeige löschen
        }
    }
}
