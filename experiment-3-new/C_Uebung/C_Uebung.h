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
// UART-Konstanten
#define PCLOCK 12500000    // Peripherie-Clock in Hz
#define BAUDRATE 19200    // Baudrate der seriellen Schnittstelle
#define DLAB_BIT 	128 			 // 7 Bit -> 1
#define UART_PINSEL_CONFIG 0x50000  // P0.8 = TxD1, P0.9 = RxD1
#define UART_READY_BIT 0x20         // Ready-Bit im UART
#define UART_FIFO_ENABLE 0x07       // FIFO aktivieren und zurücksetzen
#define UART_RX_READY 0x01          // RX-Ready-Bit
#define HEX_DIGIT_LIMIT 8           // Maximale Länge der Hex-Eingabe
#define CR_LF "\r\n"               // Carriage Return + Line Feed
#define INVALID_HEX_VALUE 0xFFFFFFFF

// Funktionsprototypen
void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendChar(char data);
void uartSendString(char* str);
char uartReadChar(void);
unsigned int hexCharToValue(char c);
void uartReadHexInput(char* inputBuffer, unsigned long* address);
void memoryDumpHex(unsigned long address, unsigned int length);
#endif /* C_UEBUNG_H */
