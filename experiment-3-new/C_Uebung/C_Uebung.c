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

    PINSEL0 |= UART_PINSEL_CONFIG;  // P0.8 = TxD1, P0.9 = RxD1 für UART1 aktivieren

    Frequenzteiler = PCLOCK / (16 * baudRate);  // Baudratenteiler berechnen
                                                // DLAB : einstellung BAUD
    U1LCR = DLAB_BIT | uartConfig;  						// DLAB-Bit setzen, 8 Datenbits, 1 Stoppbit
    U1DLL = Frequenzteiler % 256;  							// Niedriges Byte des FrequenzTeilers
    U1DLM = Frequenzteiler / 256;   						// Hohes Byte des FrequenzTeilers
    U1LCR = uartConfig;													// DLAB-Bit löschen
    U1FCR = UART_FIFO_ENABLE;  									// FIFO aktivieren und zurücksetzen
}

// Zeichen über UART senden
void uartSendChar(char data) {
    while ((U1LSR & UART_READY_BIT) == 0);  		// Warte, bis das Transmit-Register bereit ist
    U1THR = data;  															// char send, sobald beschrieben -> senden
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
    while (!(U1LSR & UART_RX_READY));  					// Warte auf empfangenes Zeichen
    return U1RBR;  															// Zeichen zurückgeben
}																								// DR Bit - Bit 0 in LSR

// Hex Zeichen in numerischen Wert umwandeln
unsigned int hexCharToValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';													// '0' = 48 , '9' = 57
    } else if (c >= 'A' && c <= 'F') {					// 'A' = 65 , 'F' = 70
        return c - 'A' + 10;										// 'c' = 97 , 'f' = 102
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return INVALID_HEX_VALUE;  									// Ungültiges Zeichen
}

// Hexadezimale Adresse von Benutzer lesen
void uartReadHexInput(char* inputBuffer, unsigned long* address) {
    char receivedChar;
	  unsigned int hexValue;
    int i=0;																		// Anzahl der empf. zeichen
    *address = 0;  															// Adresse init -> um aufzubauen

    while (i < HEX_DIGIT_LIMIT) {
        receivedChar = uartReadChar();  				// Zeichen empfangen
        uartSendChar(receivedChar);  						// Echo des Zeichens -> bestätigung

        if (receivedChar == '\r') { 					  // Eingabe beenden
            break;
        }
																								// Umwandlung des Zeichens in Hex-Wert
        hexValue = hexCharToValue(receivedChar);
        if (hexValue != INVALID_HEX_VALUE) {
																								// ADDR UPDATE & Zeichen in Puffer schreiben
						*address = (*address << 4) + hexValue;
						inputBuffer[i++] = receivedChar;
						} else {
					   uartSendString(" Ungültiges Zeichen! Bitte nur Hex-Zeichen verwenden (0-9, a-f, A-F)." CR_LF);
				    }
    }
				inputBuffer[i] = '\0';  								// Nullterminierung für String
																								// Zusätzliche Zeichen nach Eingabe verwerfen
		while (receivedChar != '\r') {
        receivedChar = uartReadChar();
        uartSendChar(receivedChar);
        if (receivedChar != '\r') {
            uartSendString(" Ungültiges Zeichen nach der Eingabe! Bitte mit Enter abschließen." CR_LF);
        }
    }

}

// Speicherinhalt als Hexadezimalwerte ausgeben - Jedes Byte als Zwei Zeichen ausgeben
void memoryDumpHex(unsigned long address, unsigned int length) {
    char* ptr = (char*) address;  							// Speicheradresse auf char Zeiger umwandeln,
    int i;                       							  // um Byteweise zugreifen zu können
    for (i = 0; i < length; i++) {
        unsigned char value = ptr[i];           // aktuelles Byte auslesen

				// Upper-Byte Hälfte und Lower-ByteHälfte extrahieren
        char upperHalf = (value >> 4) & 0x0F    // um 4 shiften und mask. zum extrahieren
        char lowerHalf = value & 0x0F;					// Bit-maske

        uartSendChar(upperHalf < 10 ? '0' + upperHalf : 'A' + (upperHalf - 10));
																								// lowerHalf analog zu upperHalf
        uartSendChar(lowerHalf < 10 ? '0' + lowerHalf : 'A' + (lowerHalf - 10));

        uartSendChar(' ');  										// Leerzeichen für visuelles
    }
																								// Sende eines Returns und Line-Feed
    uartSendString(CR_LF);
}

int main(void) {
    char inputBuffer[HEX_DIGIT_LIMIT + 1];  // Platz für die eingegebene Adresse (8 Zeichen + Nullterminierung)
    unsigned long address;
	
    uartInit(BAUDRATE, 3, 1, 1, 1);  // UART initialisieren
	
	  uartSendString("0123456789");
	  uartSendString(CR_LF);
		
    while (1) {
        uartReadHexInput(inputBuffer, &address);
        uartSendString(CR_LF);

        uartSendString(inputBuffer);  // Original-Eingabe anzeigen
			  uartSendString(": ");

        memoryDumpHex(address, 16);  // Speicherinhalt anzeigen
        uartSendString(CR_LF"\n");
    }
}
