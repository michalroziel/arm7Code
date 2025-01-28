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
/*  Name / Matrikel-Nr.: *      Valentin Straßer                    */
/*                       *      Michal Roziel                       */
/********************************************************************/
/*  Abgabedatum:         *      16.01.2025                          */
/********************************************************************/

#include <LPC21xx.H>  // LPC21xx Mikrocontroller Definitionen
#include "C_Uebung.H"

void uartInit(unsigned int baudRate, unsigned int dataBits, unsigned int stopBits, unsigned int paritySelect, unsigned int parityEnable) {
    unsigned int uartConfig = 0;
    unsigned int Frequenzteiler;
    uartConfig = (paritySelect << 1) + parityEnable;  
    uartConfig = (uartConfig << 1) + stopBits;  
    uartConfig = (uartConfig << 2) + dataBits;  

    PINSEL0 |= 0x05;  // P0.8 = TxD1, P0.9 = RxD1 für UART1 aktivieren

    Frequenzteiler = PCLOCK / (16 * baudRate);  // Baudratenteiler berechnen
                                                // DLAB : einstellung BAUD
    U0LCR = DLAB_BIT | uartConfig;  						// DLAB-Bit setzen, 8 Datenbits, 1 Stoppbit
    U0DLL = Frequenzteiler % 256;  							// Niedriges Byte des FrequenzTeilers
    U0DLM = Frequenzteiler / 256;   						// Hohes Byte des FrequenzTeilers
    U0LCR = uartConfig;													// DLAB-Bit löschen
    U0FCR = UART_FIFO_ENABLE;  									// FIFO aktivieren und zurücksetzen
}

unsigned int readSwitchState1(void){
	return (IOPIN0>> 16) & 1;
}

unsigned int readSwitchState2(void){
	return (IOPIN0>> 17) & 1;
}

unsigned int readSwitchState3(void){
	return (IOPIN1 >> 25) & 1;
}

/* BCD Eingang lesen */
unsigned int readInputBCD(void) {       
    return (IOPIN0 >> 10) & 0xF;
}

/* Menüs senden */
void sendMenu(void) {
    uartSendString("\r\nStopp-Uhr\r\n");
	  uartSendString("	Start und Anhalten durch druecken der Interrupt-Taste\r\n");
    uartSendString("	s,S - Start/Stop\r\n");
    uartSendString("	a,A - Anzeigen\r\n");
    uartSendString("	r,R - Reset\r\n");
}

/* Baudrate initialisieren */
unsigned int initBaudrate(void) {
    unsigned int index = readInputBCD();
    return (index > 9) ? baudrates[9] : baudrates[index];
}

// Zeichen über UART senden
void uartSendChar(char data) {
    while ((U0LSR & UART_READY_BIT) == 0);  		// Warte, bis das Transmit-Register bereit ist
    U0THR = data;  															// char send, sobald beschrieben -> senden
}

// String über UART senden
void uartSendString(char* str) {
    int i = 0;
    while (str[i] != '\0') {										// Null Terminierung
        uartSendChar(str[i]); 								  // Zeichenweise senden
        i++;
    }
	}

// Zeichen über UART empfangen
char uartReadChar(void) {
    while (!(U0LSR & UART_RX_READY));  					// Warte auf empfangenes Zeichen
    return U0RBR;  															// Zeichen zurückgeben
}																								// DR Bit - Bit 0 in LSR


/* Timer Initialisierung */
void initTimer(void) {
    T0PR = 12500;  // Prescaler
    T0TCR = 0x02;  // Timer zurücksetzen
    T0MCR = 0x03;  // Interrupt und Reset
    T0MR0 = 1000;  // 1 Sekunde
    T0TCR = 0x00;  // Timer anhalten

    VICVectAddr4 = (unsigned long)T0isr;
    VICVectCntl4 = 0x24;
    VICIntEnable |= 0x10;
}

/* Interrupt Initialisierung */
void initExIn(void) {
    PINSEL0 |= 0x80000000;  // EINT2 aktivieren
    EXTMODE |= 0x04;  // Flankengetriggerter Interrupt
    VICVectCntl0 = 0x30;  // Kanal 16 aktivieren
    VICVectAddr0 = (unsigned long)myEXTINT;
    VICIntEnable = 0x10000;  // EINT2 aktivieren
}

/* Integer senden */
void sendInt(int value) {
    char buffer[5];
    int i = 0;
    if (value == 0) buffer[i++] = '0';
    while (value > 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    while (i--) uartSendChar(buffer[i]);
}

/* Externer Interrupt Handler */
void myEXTINT(void) __irq {
	  static unsigned int tasterzustand = 0;
    if (tasterzustand > 0) {
        T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;
        uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");
        tasterzustand = 0;
    } else {
        tasterzustand++;
    }
    EXTINT = 0x04;
    VICVectAddr = 0x00;
}

/* Timer Interrupt Handler */
void T0isr(void) __irq {
    sek++;
	  IOCLR0=SEGMENT;	       	        	// Setzt Low-Spannungspegel an P0.18-P0.24 
		IOSET0=bcd[sek%10];										// Initialisiert den BCD auf 0
    T0IR |= 0x10;
    VICVectAddr = 0x00;
}

/* Zeit senden */
void sendTime(int time) {
    int h = time / 3600;
    int m = (time % 3600) / 60;
    int s = time % 60;
    uartSendString("Zeit: ");
		if(h<10) sendInt(0);
    sendInt(h); 
	  uartSendChar(':');
	  if(m<10) sendInt(0);
    sendInt(m);
	  uartSendChar(':');
	  if(s<10) sendInt(0);
    sendInt(s); 
		uartSendString("\r\n");
}
void initSeg(void){
	  IODIR0=SEGMENT;  		       				// Ausgang von P0.18-P.0.24 (BCD Anzeige) - SETZT 
		IOCLR0=SEGMENT;	       	        	// Setzt Low-Spannungspegel an P0.18-P0.24 
		IOSET0=bcd[0];										// Initialisiert den BCD auf 0
}

int main(void) {
	  
		char choice;
	  sek = 215990;
    uartInit(initBaudrate(), 3, readSwitchState3(), readSwitchState2(), readSwitchState1());  // UART initialisieren
	  initSeg();
	  initExIn();
    initTimer();
		
    while (1) {
			    sendMenu();
          choice = uartReadChar();
        switch (choice) {
            case 's': case 'S':
                T0TCR = (T0TCR == 0x01) ? 0x00 : 0x01;
                uartSendString((T0TCR == 0x01) ? "Timer gestartet!\r\n" : "Timer angehalten!\r\n");
                break;
            case 'a': case 'A':
                sendTime(sek);
                break;
            case 'r': case 'R':
                if (T0TCR==0x01){						// Wenn Timer läuft
													uartSendString ("Sie muessen die Stoppuhr erst anhalten, erst dann duerfen Sie sie zuruecksetzen");
													uartSendString("\r\n");
									} else {						// sonst
													T0TCR=0x02;			   				// Timer rücksetzen
													sek = 0;	
													uartSendString("Timer wurde erfolgreich zurückgesetzt!");
									}
                break;
            default:
                uartSendString("Ungültige Eingabe!\r\n");
                break;
        }
				if (sek >= 215999){
						T0TCR=0x00;									// Timer angehalten
						T0TCR=0x02;			   						// Timer rücksetzen
						sek = 0;								// Sekunden zurücksetzen
						uartSendString ("aktuelle Timer-Zeit: 59:59:59. Stoppuhr wurde angehalten und zurueckgesetzt");
						uartSendString("\r\n");
		}
    }
}
