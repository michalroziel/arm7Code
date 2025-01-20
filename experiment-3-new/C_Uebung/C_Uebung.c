#include <LPC21xx.H>		/* LPC21xx Definitionen                 */
#include "C_Uebung.H"

#define	PCLOCK 12.5
#define	BAUDRATE	4800

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitätAuswahl, unsigned int ParitätAktivierung);
void sendchar(unsigned char daten);
void sendchars(char* daten);
void readHexInput(char* buffer, int maxLength);
void sendHexDump(unsigned int address);

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitätAuswahl, unsigned int ParitätAktivierung) {
    unsigned int komparameter = ((((((0b1000 + ParitätAuswahl) << 1) + ParitätAktivierung) << 1) + StoppBits) << 2) + DatenBits;
    PINSEL0 = PINSEL0 | 0x50000; /* P0.8=TxD, P0.9=RxD UART1 */
    unsigned int Frequenzteiler = PCLOCK / (16 * BaudRate); // Calculate the frequency divider
    U1LCR = komparameter;
    U1DLL = Frequenzteiler % 256; // Low-byte (remainder of division by 256)
    U1DLM = Frequenzteiler / 256; // High-byte (integer division by 256)
    U1LCR = 0x03; /* DLAB-Bit loeschen */
    U1FCR = 0x07; /* FIFO's aktivieren und ruecksetzen */
}

void sendchar(unsigned char daten) {
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

// Liest eine Hexadezimalzahl von der seriellen Schnittstelle
void readHexInput(char* buffer, int maxLength) {
    int index = 0;
    char receivedChar;

    while (index < maxLength - 1) {
        // Warte auf ein empfangenes Zeichen
        while ((U1LSR & 0x01) == 0);
        receivedChar = U1RBR;

        // Wenn CR (0x0D) empfangen wurde, Eingabe beenden
        if (receivedChar == '\r') {
            break;
        }

        // Nur gültige Hex-Zeichen akzeptieren (0-9, A-F, a-f)
        if ((receivedChar >= '0' && receivedChar <= '9') ||
            (receivedChar >= 'A' && receivedChar <= 'F') ||
            (receivedChar >= 'a' && receivedChar <= 'f')) {
            buffer[index++] = receivedChar;
            sendchar(receivedChar); // Eingegebenes Zeichen zurücksenden (Echo)
        }
    }

    buffer[index] = '\0'; // Nullterminierung hinzufügen
    sendchars("\r\n");    // CR + LF nach Eingabe
}


void sendint(int daten){
	int laenge;
	int j;
	char data[5];
	sprintf(data, "%i", daten); 
	laenge = (int)strlen(data);
	for (j = 0; j < laenge; j++){
		while ((U0LSR & 0x20) == 0);
	   	if ( (U0LSR & 0x20) != 0 ) {
				U0THR = data[j];
	   	}
	}
}
// Sendet einen Hex-Dump von 16 Bytes ab der angegebenen Adresse
void sendHexDump(unsigned int address) {
  sendchars("\r\n0x");
  char buffer[16];


    char *ptr = (char *) address;  // Zahl in eine Speicheradresse umwandeln

    for (int i = 0; i < 16; i++) {
        buffer[i] = ptr[i];  // Lese die Daten ab der Adresse
    }

    sendchars(buffer);
    // Zeilenumbruch hinzufügen
    sendchars("\r\n\r\n");
}

int main(void) {
    char inputBuffer[9]; // Buffer für die Eingabe (maximal 8 Hex-Zeichen + Nullterminierung)
    unsigned int address;

    // UART initialisieren
    initUart(BAUDRATE, 8, 1, 0, 0);

    sendchars("Hex-Dump Tool bereit. Geben Sie eine 8-stellige Hex-Zahl ein (Abschluss mit CR):\r\n");

    while (1) {
        // Hexadezimalzahl von der seriellen Schnittstelle lesen
        readHexInput(inputBuffer, sizeof(inputBuffer));

        // Eingabe in eine Ganzzahl umwandeln
        address = strtoul(inputBuffer, NULL, 16);

        // Hex-Dump ausgeben
        sendHexDump(address);
    }
}
