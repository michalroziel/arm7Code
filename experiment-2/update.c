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

#include <LPC21xx.H>        /* LPC21xx Definitionen */

// Definitionen
#define SEGMENT_MASK 0x1FC0000 // BCD-7-Segment-Anzeige
#define LED_MASK     0xFF0000  // LEDs an P0.16-P0.23
#define SWITCH_MASK  0x30000   // Schalter an P0.16 und P0.17
#define BCD_MAX      9         // Maximalwert der BCD-Anzeige
#define TIMER_PERIOD 500       // Timer-Periode in Millisekunden
#define PRESCALER    12500     // Prescaler für 1 kHz Timer

// BCD-Codes für die Zahlen 0-9
const unsigned long BCD_CODES[10] = {
    0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
    0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000
};

// Globale Variablen (nur für ISR-Kommunikation)
volatile unsigned int ledPattern = 0;      // Aktuelles LED-Muster
volatile unsigned int switchState = 0;

// Prototypen
void delay(void);
void initLED(void);
void initBCD(void);
void initTimer(void);
void setBCD(unsigned int value);
void setLEDs(unsigned int pattern);
void T0isr(void) __irq;

// Funktionen
void delay(void) {
    T0TCR = 0x02;            // Timer zurücksetzen
    T0PR = PRESCALER - 1;    // Prescaler für 1 kHz
    T0MR0 = TIMER_PERIOD;    // Match-Wert
    T0MCR = 0x03;            // Interrupt und Reset bei Match
    T0TCR = 0x01;            // Timer starten
}

void initLED(void) {
    IODIR1 = LED_MASK;       // LEDs als Ausgang
    IOCLR1 = LED_MASK;       // Alle LEDs ausschalten
}

void initBCD(void) {
    IODIR0 = SEGMENT_MASK;   // BCD-Anzeige als Ausgang
    IOCLR0 = SEGMENT_MASK;   // BCD-Anzeige löschen
}

void setBCD(unsigned int value) {
    if (value > BCD_MAX) value = BCD_MAX; // Eingabe begrenzen
    IOCLR0 = SEGMENT_MASK;                // Anzeige löschen
    IOSET0 = BCD_CODES[value];            // Wert setzen
}

void setLEDs(unsigned int pattern) {
    IOCLR1 = LED_MASK;                   // Alle LEDs ausschalten
    IOSET1 = (pattern << 16);            // Muster setzen
}

void T0isr(void) __irq {

		 // Überprüfung von Schalter 0x2
        if (switchState & 0x2) {
					
					ledPattern = (ledPattern << 1) | 1; // Neues Bit hinzufügen
    if (ledPattern > 0xFF) {
        ledPattern = 0; // Alle LEDs ausschalten und zurücksetzen
    }
					
					
            setLEDs(ledPattern);
        } else {
            // Schalter aus: LEDs ausschalten und Muster speichern
            setLEDs(0); // Alle LEDs ausschalten
        }
    // Timer-Interrupt-Flag zurücksetzen
    T0IR = 0x01;
    VICVectAddr = 0x00; // Interrupt beendet
}

void initTimer(void) {
    T0PR = PRESCALER - 1;        // Prescaler für 1 kHz
    T0MR0 = TIMER_PERIOD;        // Match-Wert
    T0MCR = 0x03;                // Interrupt und Reset bei Match
    T0TCR = 0x01;                // Timer starten

    VICVectAddr0 = (unsigned long)T0isr; // ISR-Adresse
    VICVectCntl0 = 0x24;                 // Kanal 4, aktivieren
    VICIntEnable = 0x10;                 // Timer 0 Interrupt aktivieren
}

int main(void) {
    unsigned int bcdInput = 0;

    // Initialisierung
    initBCD();
    initLED();
    initTimer();

    while (1) {
        // Schalterzustand einlesen
        switchState = (IOPIN0 & SWITCH_MASK) >> 16;

        // BCD-Anzeige aktualisieren
        if (switchState & 0x1) {
            bcdInput = (IOPIN0 >> 10) & 0xF; // Eingabe lesen
            setBCD(bcdInput);
        } else {
					  IOCLR0 = SEGMENT_MASK;
				}

       
    }
}
