                AREA    Daten, DATA, READWRITE   ; Define a writable data area
Datenanfang
STR_1           EQU Datenanfang + 0x100
Stack_Anfang    EQU Datenanfang + 0x300
Top_Stack       EQU Stack_Anfang + 0x100
STR_2           EQU Top_Stack + 0x100

;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher                         *
;********************************************************************
                AREA    Programm, CODE, READONLY
                ARM
Reset_Handler   MSR     CPSR_c, #0x10           ; User Mode aktivieren

;********************************************************************
;* Hauptprogramm                                                   *
;********************************************************************
                LDR     SP, =Top_Stack          ; Adresse des Stacks laden
                LDR     R0, =String_1           ; Adresse von String_1 laden
                LDR     R9, =0x0000FFFF         ; Maske für AND-Operation
                BL      atouI                   ; String zu Integer konvertieren
                AND     R0, R0, R9              ; Maske anwenden
                BL      berechnung              ; Berechnung durchführen
                AND     R0, R0, R9              ; Ergebnis erneut maskieren
                LDR     R1, =STR_2              ; Zieladresse für String setzen
                BL      uitoa                   ; Zahl in String umwandeln

endlos          B       endlos                  ; Endlosschleife

;********************************************************************
;* Unterprogramme                                                  *
;********************************************************************

; String zu Integer konvertieren
atouI
                STMFD   SP!, {R1-R4, R14}       ; Register sichern
                MOV     R2, #10                 ; Basis (10) setzen
                MOV     R3, #0                  ; Ergebnis initialisieren

schleife_atouI
                LDRB    R1, [R0], #1            ; Nächstes Zeichen laden, R0 inkrementieren
                MOV     R4, R3                  ; Zwischenergebnis speichern
                CMP     R1, #0x00               ; Ende des Strings?
                SUBNE   R1, R1, #0x30           ; Zeichen in Zahl umwandeln
                MLANE   R3, R4, R2, R1          ; Ergebnis * 10 + neue Zahl
                BNE     schleife_atouI          ; Wiederholen, falls nicht Ende

                MOV     R0, R4                  ; Ergebnis in R0
                LDMFD   SP!, {R1-R4, R14}       ; Register wiederherstellen
                BX      LR                      ; Rücksprung

; Berechnung durchführen
berechnung
                CMP     R0, #0                  ; Prüfen, ob Zahl negativ ist
                RSBMI   R0, R0, #0              ; Falls negativ, Betrag berechnen
                MOV     R1, R0                  ; Kopiere R0 nach R1
                MOV     R2, R1, LSL #1          ; R2 = 2 * R0

                LDR     R1, =0xCCCCCCCD         ; Magic-Number für Division durch 5
                UMULL   R3, R4, R2, R1          ; Unsigned-Multiply: R2 * 0xCCCCCCCD
                MOV     R2, R4, LSR #2          ; Division durch 5 (High-Result >> 2)

                MUL     R0, R2, R2              ; Quadrieren des Ergebnisses
                BX      LR                      ; Rücksprung

; Integer zu String konvertieren
uitoa
                STMFD   SP!, {R2-R7, LR}        ; Register sichern
                MOV     R2, #10                 ; Basis (10) setzen
                LDR     R3, =0x1999A            ; Magic-Number für Division durch 10
                MOV     R4, #0                  ; Zähler initialisieren

schleife
                CMP     R0, #0                  ; Prüfen, ob Zahl 0 ist
                UMULLNE R6, R5, R0, R3          ; Division durch 10
                MOVNE   R5, R6, LSR #16         ; Quotient berechnen
                MULNE   R6, R5, R2              ; Quotient * 10
                SUBNE   R6, R0, R6              ; Rest berechnen
                ADDNE   R6, #0x30               ; In ASCII-Zeichen umwandeln
                STMFDNE SP!, {R6}               ; Zeichen auf Stack speichern
                MOVNE   R0, R5                  ; Nächste Zahl bearbeiten
                ADDNE   R4, R4, #1              ; Zähler erhöhen
                BNE     schleife                ; Wiederholen, falls Zahl nicht 0

revstr
                CMP     R4, #0
                LDMFDNE SP!, {R6}               ; Zeichen vom Stack holen
                STRBNE  R6, [R1], #1            ; Zeichen speichern, Zeiger inkrementieren
                SUBNE   R4, R4, #1              ; Zähler verringern
                BNE     revstr                  ; Wiederholen, bis alle Zeichen verarbeitet

                MOV     R3, #0x00               ; Null-Terminator
                STRB    R3, [R1]                ; Null-Terminator setzen
                LDMFD   SP!, {R2-R7, LR}        ; Register wiederherstellen
                BX      LR                      ; Rücksprung

;********************************************************************
; Konstanten im CODE-Bereich
;********************************************************************
String_1        DCB     "123", 0x00

;********************************************************************
; Ende der Programm-Quelle
;********************************************************************
                SPACE   4
                END
