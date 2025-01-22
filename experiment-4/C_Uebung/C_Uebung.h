/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Uebung.h: Header-Datei für die Implementierungsdatei          */
/********************************************************************/

#ifndef C_UEBUNG_H
#define C_UEBUNG_H

#include <LPC21xx.H> // LPC21xx Definitionen

/* Konstanten */
/*** Globale Konstanten und Variablen ***/
#define PCLOCK          12.5
#define SWITCH_MASK1    0x10000  // Schalter an P0.16
#define SWITCH_MASK2    0x20000  // Schalter an P0.17
#define SWITCH_MASK3    0x1000000 // Schalter an P1.25
#define INPUT_MASK      0x3C00   // Eingabemaske für BCD
static const unsigned int baudrates[] = {110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600};

 unsigned int sek = 0;        // Sekunden-Zähler
 unsigned int tasterzustand = 0;

/*** Funktionsprototypen ***/
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendChar(char data);
void uartSendString(char* str);
char uartReadChar(void);

unsigned int readInputBCD(void);
unsigned int readSwitchState1(void);
unsigned int readSwitchState2(void);
unsigned int readSwitchState3(void);
unsigned int initBaudrate(void);

void initTimer(void);
void initExIn(void);
void myEXTINT(void) __irq;
void T0isr(void) __irq;

void sendMenu(void);
void sendTime(int time);
void sendInt(int value);
#endif /* C_UEBUNG_H */
