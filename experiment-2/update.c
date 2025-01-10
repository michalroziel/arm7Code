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
/*                       *     				 Michal Roziel                       */
/*                       *                                          */
/********************************************************************/
/*  Abgabedatum:         *      16.01.2025                          */
/*                       *                                          */
/********************************************************************/

#include <LPC21xx.H>

// Masken und Konstanten
#define 		SEGMENT_MASK     0x1FC0000 	 // 7-Segment-Anzeige an P0.18-P0.23
#define 		LED_MASK         			0xFF0000   	 // LEDs an P1.16-P1.23
#define 		SWITCH_MASK      	0x30000         // Schalter an P0.16 und P0.17
#define 		INPUT_MASK       		0x3C00     		 // Eingabemaske für BCD
#define 		BCD_MAX          			9                   // Maximalwert der BCD-Anzeige
#define 		LED_MAX          				0xFF     			   // Maximales LED-Muster

// BCD-Codes für die Zahlen 0-9
static const unsigned long BCD_CODES[10] = {
    0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
    0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000
};

// Volatile Variablen für ISR-Kommunikation
volatile unsigned int g_ledPattern = 0;      // Aktuelles LED-Muster
volatile unsigned int g_switchState = 0;     // Aktueller Schalter-Status

// Funktionsprototypen
void initLED(void);           // LED-Initialisierung
void initBCD(void);           // BCD-Anzeige-Initialisierung
void updateBCD(unsigned int value);  // BCD-Anzeige aktualisieren
void updateLEDs(unsigned int pattern);  // LEDs aktualisieren
unsigned int readInputBCD(void); // Eingabe einlesen
void readSwitchState(void);   // Schalter-Status einlesen
void initTimer(void);         // Timer initialisieren
void T0isr(void) __irq;       // Timer-Interrupt-Service-Routine

// LED-Ausgänge initialisieren
void initLED(void) {
    IODIR1 = LED_MASK;       // P1.16-P1.23 als Ausgang definieren
    IOCLR1 = LED_MASK;       // Alle LEDs initial ausschalten
}

// BCD-Anzeige initialisieren
void initBCD(void) {
    IODIR0 = SEGMENT_MASK;   // P0.18-P0.24 als Ausgang definieren
    IOCLR0 = SEGMENT_MASK;   // BCD-Anzeige initial löschen
}

// BCD-Anzeige mit Wert aktualisieren
void updateBCD(unsigned int value) {
    if (value > BCD_MAX) value = BCD_MAX;  // Wert begrenzen
    
    IOCLR0 = SEGMENT_MASK;   // Alte Anzeige löschen
    IOSET0 = BCD_CODES[value];  // Neuen Wert setzen
}

// LED-Muster aktualisieren
void updateLEDs(unsigned int pattern) {
    IOCLR1 = LED_MASK;       // Alle LEDs ausschalten
    IOSET1 = (pattern << 16);  // Neues Muster auf P1.16-P1.23 setzen
}

// Eingabewert von Schaltern einlesen
unsigned int readInputBCD(void) {
    return (IOPIN0 >> 10) & 0xF;  // Bits 10-13 ausmaskieren
}

// Schalter-Status einlesen
void readSwitchState(void) {
    g_switchState = (IOPIN0 & SWITCH_MASK) >> 16;  // Bits 16-17 ausmaskieren
}

// Timer-Interrupt-Service-Routine
void T0isr(void) __irq {
    // LED-Muster bei aktivem Schalter aktualisieren
    if (g_switchState & 0x2) {
        g_ledPattern = (g_ledPattern << 1) | 1;  // Neues Bit hinzufügen
        
        // Bei Überlauf zurücksetzen
        if (g_ledPattern > LED_MAX) {
            g_ledPattern = 0;
        }
        
        updateLEDs(g_ledPattern);
    } else {
        updateLEDs(0);  // Alle LEDs ausschalten
    }
    
    // Interrupt-Flag zurücksetzen
    T0IR = 0x01;
    VICVectAddr = 0x00;
}

// Timer initialisieren
void initTimer(void) {
    // Timer-Konfiguration
    T0PR = 12500 - 1;    // Prescaler für 1 kHz
    T0MR0 = 500;         // Match-Wert für 0.5 Sekunden
    T0MCR = 0x03;        // Interrupt und Reset bei Match
    T0TCR = 0x01;        // Timer starten

    // Interrupt-Konfiguration
    VICVectAddr0 = (unsigned long)T0isr;  // ISR-Adresse setzen
    VICVectCntl0 = 0x24;                  // Kanal 4 aktivieren
    VICIntEnable = 0x10;                  // Timer 0 Interrupt aktivieren
}

int main(void) {
    unsigned int bcdInput = 0;
    
    // Hardware initialisieren
    initBCD();
    initLED();
    initTimer();

    while (1) {
        // Schalter-Status aktualisieren
        readSwitchState();

        // BCD-Anzeige aktualisieren
        if (g_switchState & 0x1) {
            bcdInput = readInputBCD();
            updateBCD(bcdInput);
        } else {
            IOCLR0 = SEGMENT_MASK;  // BCD-Anzeige löschen
        }
    }
}
