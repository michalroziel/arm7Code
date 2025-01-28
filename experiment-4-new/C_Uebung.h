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
#define SEGMENT 0x1FC0000
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

 unsigned int sek = 0;        // Sekunden-Zähler
 
unsigned long bcd[10]={0x0FC0000, 0x180000, 0x16C0000, 0x13C0000, 0x1980000,
 0x1B40000, 0x1F40000, 0x1C0000, 0x1FC0000, 0x1BC0000}; 
static const unsigned int baudrates[] = {110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600};
// Funktionsprototypen
unsigned int readInputBCD(void);
unsigned int readSwitchState1(void);
unsigned int readSwitchState2(void);
unsigned int readSwitchState3(void);
unsigned int initBaudrate(void);
void initTimer(void);
void initExIn(void);
void myEXTINT(void) __irq;
void T0isr(void) __irq;


void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable);
void uartSendChar(char data);
void uartSendString(char* str);
char uartReadChar(void);
#endif /* C_UEBUNG_H */
