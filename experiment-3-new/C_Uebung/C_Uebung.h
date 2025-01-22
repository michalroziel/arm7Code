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
// UART-Konstanten
#define PCLOCK 12500000   // Peripherie-Clock in Hz
#define BAUDRATE 19200    // Baudrate der seriellen Schnittstelle

// Funktionsprototypen
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendChar(char data);
void uartSendString(char* str);
char uartReadChar(void);
void uartReadHexInput(char* inputBuffer, unsigned long* address);
void memoryDumpHex(unsigned long address, int length);
#endif /* C_UEBUNG_H */
