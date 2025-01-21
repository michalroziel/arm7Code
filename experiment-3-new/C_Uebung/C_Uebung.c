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

#include <LPC21xx.H>        /* LPC21xx Definitionen                 */

#define PCLOCK 12500000
#define BAUDRATE 19200

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitaetAuswahl, unsigned int ParitaetAktivierung);
void sendchar(char daten);
void sendchars(char* daten);
unsigned long readHexInput(void);
char readChar(void);
void sendHexFromMemory(unsigned long address, int length) ;



void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitaetAuswahl, unsigned int ParitaetAktivierung) {
    unsigned int komparameter;
    unsigned int Frequenzteiler;

    komparameter = (8 + ParitaetAuswahl);
    komparameter = (komparameter << 1) + ParitaetAktivierung;
    komparameter = (komparameter << 1) + StoppBits;
    komparameter = (komparameter << 2) + DatenBits;

    PINSEL0 |= 0x50000; // P0.8=TxD, P0.9=RxD UART1
    Frequenzteiler = PCLOCK / (16 * BaudRate);
    U1LCR = 0x9F;
    U1DLL = Frequenzteiler % 256;
    U1DLM = Frequenzteiler / 256;
    U1LCR = 0x1F;
    U1FCR = 0x07;
}

void sendchar(char daten) {
    while ((U1LSR & 0x20) == 0);
    U1THR = daten;
}

void sendchars(char* daten) {
    int j = 0;
    while (daten[j] != '\0') {
        sendchar(daten[j]);
        j++;
    }
	}
char readChar(void){
	while (!(U1LSR & 0x01));
        return U1RBR;
}

void sendHexFromMemory(unsigned long address, int length) {
    // Zeiger auf die Adresse casten
    char* ptr = (char*) address;
		int i;
    for (i = 0; i < length; i++) {
        unsigned char value = ptr[i];  // Lese ein Byte aus dem Speicher

        // Oberes und unteres Nibble des Byte-Wertes in Hexadezimal umwandeln
        char highNibble = (value >> 4) & 0x0F;
        char lowNibble = value & 0x0F;

        // Nibble in Hex-Zeichen umwandeln und senden
        sendchar(highNibble < 10 ? '0' + highNibble : 'A' + (highNibble - 10));
        sendchar(lowNibble < 10 ? '0' + lowNibble : 'A' + (lowNibble - 10));

        sendchars(" ");  // Leerzeichen für bessere Lesbarkeit
    }
}

unsigned long readHexInput(void) {
    char receivedChar;
		unsigned long address = 0;
		int i;
	  for (i = 0; i < 8; i++) {
        receivedChar = readChar();
			  sendchar(receivedChar);
				if (receivedChar == '\r') {
            break;
        }
				if (receivedChar >= '0' && receivedChar <= '9') {
            address = (address << 4) + (receivedChar - '0');
        } 
        else if (receivedChar >= 'A' && receivedChar <= 'F') {
            address = (address << 4) + (receivedChar - 'A' + 10);
        } 
        else if (receivedChar >= 'a' && receivedChar <= 'f') {
            address = (address << 4) + (receivedChar - 'a' + 10);
        } else {
            sendchars("Ungueltiges Zeichen!\r\n");
            i--;
        }
    }
		return address;
}

int main(void) {
    unsigned long address = 0;
    initUart(BAUDRATE, 3, 1, 1, 1);
    while (1) {
			address = readHexInput();
			sendchars("\r\n");
			sendchars("Addresse: ");
			sendHexFromMemory(address,16);
			sendchars("\r\n\n");  // Neue Zeile am Ende
    }	
}
