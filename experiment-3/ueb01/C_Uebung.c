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
#define	PCLOCK 12500
#define	BAUDRATE	4800


int main(void) {
  unsigned int frequenzteiler = PCLOCK / (16 * BAUDRATE);
    while (1) {
			
    }
}