#include <LPC21xx.H>

#define PCLOCK 12500000 // 12.6 MHz
#define BAUDRATE 19200  

void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitaetAuswahl, unsigned int ParitaetAktivierung);
void sendchar(unsigned char daten);
void sendchars(char* daten);
void readHexInput(char* buffer);
void sendHexDump(unsigned int address);
void intToStr(int value, char* str);
int strLength(const char* str);
unsigned long hexToDecimal(const char* hex);

unsigned long hexToDecimal(const char* hex) {
    unsigned long result = 0;
    char c;
    while ((c = *hex++) != '\0') {
        result <<= 4; // Multiplikation mit 16 (Shift um 4 Bits nach links)
        if (c >= '0' && c <= '9') {
            result += c - '0';
        } else if (c >= 'A' && c <= 'F') {
            result += c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            result += c - 'a' + 10;
        } else {
            // Ungültiges Zeichen ignorieren oder Fehlerbehandlung
            break;
        }
    }
    return result;
}


void initUart(unsigned int BaudRate, unsigned int DatenBits, unsigned int StoppBits, unsigned int ParitaetAuswahl, unsigned int ParitaetAktivierung) {
    unsigned int komparameter;
    unsigned int Frequenzteiler;

    komparameter = (8 + ParitaetAuswahl);
    komparameter = (komparameter << 1) + ParitaetAktivierung;
    komparameter = (komparameter << 1) + StoppBits;
    komparameter = (komparameter << 2) + DatenBits;

    PINSEL0 |= 0x50000; // P0.8=TxD, P0.9=RxD UART1
    Frequenzteiler = PCLOCK / (16 * BaudRate);
    U1LCR = komparameter;
    U1DLL = Frequenzteiler % 256;
    U1DLM = Frequenzteiler / 256;
    U1LCR = 0x03;
    U1FCR = 0x07;
}



void sendchar(unsigned char daten) {
    while ((U1LSR & 0x20) == 0);
    U1THR = daten;
}

void sendHexFromMemory(unsigned long address, int length) {
    sendchars("Hex-Dump:\r\n");

    // Zeiger auf die Adresse casten
    unsigned char* ptr = (unsigned char*)address;

    for (int i = 0; i < length; i++) {
        unsigned char value = ptr[i];  // Lese ein Byte aus dem Speicher

        // Oberes und unteres Nibble des Byte-Wertes in Hexadezimal umwandeln
        char highNibble = (value >> 4) & 0x0F;
        char lowNibble = value & 0x0F;

        // Nibble in Hex-Zeichen umwandeln und senden
        sendchar(highNibble < 10 ? '0' + highNibble : 'A' + (highNibble - 10));
        sendchar(lowNibble < 10 ? '0' + lowNibble : 'A' + (lowNibble - 10));

        sendchars(" ");  // Leerzeichen für bessere Lesbarkeit
    }

    sendchars("\r\n");  // Neue Zeile am Ende
}




void sendlong(unsigned long number) {
    char buffer[20];  // Platz für maximal 20 Ziffern (ein long kann bis zu 19 Stellen haben)
    int index = 0;

    // Extrahiere Ziffern von hinten nach vorne
    do {
        buffer[index++] = (number % 10) + '0';  // Letzte Ziffer extrahieren und als ASCII-Zeichen speichern
        number /= 10;  // Entferne die letzte Ziffer
    } while (number > 0);

    // Ziffern in der richtigen Reihenfolge senden
    while (index > 0) {
        sendchar(buffer[--index]);  // Puffer rückwärts durchlaufen und Zeichen senden
    }
}

void sendchars(char* daten) {
    int j = 0;
    while (daten[j] != '\0') {
        sendchar(daten[j]);
        j++;
    }
}

unsigned long  readHexInput(void) {
    int index = 0;
    char receivedChar;

    unsigned long address = 0 ;

    sendchars("readHexInput wurde aufgerufen");

while (index < 8) {
        receivedChar = readChar();  // Verwende readChar() zum Empfang eines Zeichens

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
        } 
        else {
            sendchars("Ungültiges Zeichen!\r\n");
            continue;  // Ungültige Zeichen überspringen
        }

        index++;
        sendchar(receivedChar);  // Echo des eingegebenen Zeichens
    }

      return address;
}


char readChar(void) {
    while (!(U1LSR & 0x01));  // Wait until data is available in the receiver buffer
    return U1RBR;             // Read and return the received character
}



void sendHexDump(unsigned int address) {
    char buffer[16];
    char* ptr = (char*) address;

    int i;
    for (i = 0; i < 16; i++) {
        buffer[i] = ptr[i];
    }

    sendchars(buffer);
    sendchars("\r\n\r\n");
}

int main(void) {
    char inputBuffer[9];
    unsigned long address;

    initUart(BAUDRATE, 3, 1, 1, 1);

    while (1) {
        readHexInput(inputBuffer);
        address = hexToDecimal(inputBuffer);
        sendHexDump(address);
    }
}
