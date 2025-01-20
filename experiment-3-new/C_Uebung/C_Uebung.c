#include <LPC21xx.H>		/* LPC21xx Definitionen                 */
#include "C_Uebung.H"

#define	PCLOCK 12.5
#define	BAUDRATE	19200

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitätAuswahl, unsigned int ParitätAktivierung);
void sendchar(unsigned char daten);
void sendchars(char* daten);
void readHexInput(char* buffer, int maxLength);
void sendHexDump(unsigned int address);

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitätAuswahl, unsigned int ParitätAktivierung) {
  //  unsigned int komparameter = ((((((0b1000 + ParitätAuswahl) << 1) + ParitätAktivierung) << 1) + StoppBits) << 2) + DatenBits;

    unsigned int komparameter = (0b1000 + ParitätAuswahl);
      komparameter = (komparameter << 1) + ParitätAktivierung;
      komparameter = (komparameter << 1) + StoppBits;
      komparameter = (komparameter << 2) + DatenBits;



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
void readHexInput(char* buffer) {
    int index = 0;
    char receivedChar;

    while (index < 9 ) {                            // 9 da : 8 bytes und abschluss zeichen
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
            
        }
    }

    buffer[index] = '\0'; // Nullterminierung hinzufügen
    
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
  sendchars("\r\n");
  char buffer[16];


    char *ptr = (char *) address;  // Zahl in eine Speicheradresse umwandeln

    for (int i = 0; i < 16; i++) {
        buffer[i] = ptr[i];  // Lese die Daten ab der Adresse

      if ( i == 15){

        buffer[i+1] = '\n';

      }


    }



    sendchars(buffer);
    // Zeilenumbruch hinzufügen
    sendchars("\r\n\r\n");
}

void sendAsciiNumbers(void) {
    for (char num = '0'; num <= '9'; num++) {
        sendchar(num);  // Sende die ASCII-Zahl über UART
    }
    //sendchars("\r\n");  // Neue Zeile zur Trennung
}

int main(void) {
    char inputBuffer[9]; // Buffer für die Eingabe (maximal 8 Hex-Zeichen + Nullterminierung)
    unsigned long address;

    // UART initialisieren
    initUart(BAUDRATE, 3, 1, 1, 1);

    sendAsciiNumbers();

    while (1) {



        // Hexadezimalzahl von der seriellen Schnittstelle lesen
        readHexInput(inputBuffer, sizeof(inputBuffer));

        // Eingabe in eine Ganzzahl umwandeln
        address = strtoul(inputBuffer, NULL, 16);

        // Hex-Dump ausgeben
        sendHexDump(address);
    }
}
