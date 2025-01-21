#include <LPC21xx.H>

#define PCLOCK 12500000
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

void sendchars(char* daten) {
    int j = 0;
    while (daten[j] != '\0') {
        sendchar(daten[j]);
        j++;
    }
}

void readHexInput(char* buffer) {
    int index = 0;
    char receivedChar;

    while (index < 8) {
        while ((U1LSR & 0x01) == 0);
        receivedChar = U1RBR;

        if (receivedChar == '\r') {
            break;
        }

        if ((receivedChar >= '0' && receivedChar <= '9') ||
            (receivedChar >= 'A' && receivedChar <= 'F') ||
            (receivedChar >= 'a' && receivedChar <= 'f')) {
            buffer[index++] = receivedChar;
        }
    }

    buffer[index] = '\0';
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
