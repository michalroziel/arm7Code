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
    unsigned int divisor;

    // Konfiguration des UART-Modus
    uartConfig = (8 + paritySelect);  // Parität
    uartConfig = (uartConfig << 1) + parityEnable;  
    uartConfig = (uartConfig << 1) + stopBits;  
    uartConfig = (uartConfig << 2) + dataBits;  

    PINSEL0 |= 0x50000;  // P0.8 = TxD1, P0.9 = RxD1 für UART1 aktivieren

    divisor = PCLOCK / (16 * baudRate);  // Baudratenteiler berechnen
    U1LCR = uartConfig;  // DLAB-Bit setzen, 8 Datenbits, 1 Stoppbit
    U1DLL = divisor % 256;  // Niedriges Byte des Divisors
    U1DLM = divisor / 256;   // Hohes Byte des Divisors
    U1LCR = 0x1F;  // DLAB-Bit löschen, 8 Datenbits, 1 Stoppbit
    U1FCR = 0x07;  // FIFO aktivieren und zurücksetzen
}

// Zeichen über UART senden
void uartSendChar(char data) {
    while ((U1LSR & 0x20) == 0);  // Warte, bis das Transmit-Register bereit ist
    U1THR = data;  // Zeichen senden
}

// String über UART senden
void uartSendString(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        uartSendChar(str[i]);  // Zeichenweise senden
        i++;
    }

}

// Zeichen über UART empfangen
char uartReadChar(void) {
    while (!(U1LSR & 0x01));  // Warte auf empfangenes Zeichen
    return U1RBR;  // Zeichen zurückgeben
}

// Hexadezimale Adresse von Benutzer lesen
void uartReadHexInput(char* inputBuffer, unsigned long* address) {
    char receivedChar;
    int i;
    *address = 0;  // Adresse initialisieren



    for (i = 0; i < 8; i++) {
        receivedChar = uartReadChar();  // Zeichen empfangen
        uartSendChar(receivedChar);  // Echo des Zeichens

        if (receivedChar == '\r') {  // Eingabe beenden
            inputBuffer[i] = '\0';
            break;
        }

        inputBuffer[i] = receivedChar;  // Zeichen im Puffer speichern

        // Zeichen in Hex umwandeln
        if (receivedChar >= '0' && receivedChar <= '9') {
            *address = (*address << 4) + (receivedChar - '0');
        } 
        else if (receivedChar >= 'A' && receivedChar <= 'F') {
            *address = (*address << 4) + (receivedChar - 'A' + 10);
        } 
        else if (receivedChar >= 'a' && receivedChar <= 'f') {
            *address = (*address << 4) + (receivedChar - 'a' + 10);
        } 
        else {
            uartSendString("Ungueltiges Zeichen!\r\n");
            i--;  // Wiederhole diese Stelle
        }
    }

		do {

			if ( receivedChar == '\r'){
				break;
			}
			receivedChar = uartReadChar();


			uartSendChar(receivedChar);
			uartSendString(" ist ein ungueltiges Zeichen zum Abschluss der Eingabe - nur CR akzeptiert !");


		}while (receivedChar != '\r');

    inputBuffer[i] = '\0';  // Nullterminierung für String
}

// Speicherinhalt als Hexadezimalwerte ausgeben
void memoryDumpHex(unsigned long address, int length) {
    char* ptr = (char*) address;  // Speicheradresse auf char Zeiger umwandeln,
    int i;                        // um Byteweise zugreifen zu können
    for (i = 0; i < length; i++) {
        unsigned char value = ptr[i];  // Jedes Byte nacheinander auslesen

        // Upper-ByteHälfte und Lower-ByteHälfte extrahieren
        // Wir shiften um 4 und maskieren die restlichen Bits
        char upperHalf = (value >> 4) & 0x0F;
        char lowerhalf = value & 0x0F;

        // numerischen Wert als ASCII anzeigen:
        // '0' hat den Wert 48, 'A' hat den Wert 65.
        // Wenn upperHalf < 10 ist, wird '0' + upperHalf ausgeführt.
        // Bsp :  48 + 5  = 53  (-> 5 in ASCII)
        // Bsp :  65 + (B - 10) (-> 66 = B in ASCII)
        uartSendChar(upperHalf < 10 ? '0' + upperHalf : 'A' + (upperHalf - 10));
        // lowerHalf analog zu upperHalf
        uartSendChar(lowerHalf < 10 ? '0' + lowerHalf : 'A' + (lowerHalf - 10));

        uartSendChar(' ');  // Leerzeichen für visuelles
    }
    // Sende eines Returns und Line-Feed
    uartSendString("\r\n");
}

int main(void) {
    char inputBuffer[9];  // Platz für die eingegebene Adresse (8 Zeichen + Nullterminierung)
    unsigned long address;
	
    uartInit(BAUDRATE, 3, 1, 1, 1);  // UART initialisieren
	
	  uartSendString("0123456789");
	  uartSendString("\r\n");
		
    while (1) {
        uartReadHexInput(inputBuffer, &address);
        uartSendString("\r\n");

        uartSendString(inputBuffer);  // Original-Eingabe anzeigen
			  uartSendString(": ");

        memoryDumpHex(address, 16);  // Speicherinhalt anzeigen

        uartSendString("\r\n\n");
    }
}
