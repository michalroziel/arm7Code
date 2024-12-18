;********************************************************************
;* htw saar - Fakultaet fuer Ingenieurwissenschaften				*
;* Labor fuer Eingebettete Systeme									*
;* Mikroprozessortechnik											*
;********************************************************************
;* Assembler_Startup.S: 											*
;* Programmrumpf fuer Assembler-Programme mit dem Keil				*
;* Entwicklungsprogramm uVision fuer ARM-Mikrocontroller			*
;********************************************************************
;* Aufgabe-Nr.:         	*	          	1     						*
;*              			*						    			*
;********************************************************************
;* Gruppen-Nr.: 			*				5						*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	*				Valentin Straßer	5014379						*
;*											Michal Roziel		5012845				*
;*									*										*
;********************************************************************
;* Abgabedatum:         	*         		 19.12.2024    							*
;*									*										*
;********************************************************************
;********************************************************************
;* Daten-Bereich bzw. Daten-Speicher				            	*
;********************************************************************
			AREA   				Daten, DATA, READWRITE  			
Datenanfang
STR_1 			  				EQU 				Datenanfang +0x100
Stack_Anfang					EQU 				Datenanfang + 0x300
Top_Stack 						EQU 				Stack_Anfang + 0x400
STR_2			    			EQU 				Top_Stack
;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
			AREA				Programm, CODE, READONLY
			ARM
Reset_Handler		      		MSR					CPSR_c, #0x10    ; User Mode aktivieren
;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************
	LDR					SP,=Top_Stack 		 ; Adresse des Werts laden
	LDR	   				R0,=String_1		 ; Wert laden
	LDR					R9,=0x0000FFFF       ; Zur Sicherheit : Begrenzung auf 16 Bits 
	BL					atouI
	AND					R0,R0,R9
	BL 					berechnung
	AND					R0,R0,R9			 ; Zur Sicherheit : Begrenzung auf 16 Bits 
	LDR					R1,=STR_2
	BL					uitoa
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos					   		 B					endlos
;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************
atouI
			STMFD       		SP!, {R1-R4, R14}    ; Speichere Register R1-R4 und Rücksprungadresse auf dem Stack
			MOV         		R2, #10              ; R2 = 10 (Basis für Dezimalberechnung)
			MOV         		R3, #0               ; R3 = 0 (Startwert für die Berechnung)

schleife_atouI
			LDRB        		R1, [R0], #1         ; Lade aktuelles Zeichen aus der Speicheradresse R0, R0++
			MOV         		R4, R3               ; Speichere bisherigen Wert von R3 in R4
			CMP         		R1, #0x00            ; Prüfe, ob Ende der Zeichenkette erreicht (Null-Terminator)
			SUBNE      		    R1, R1, #0x30        ; Falls nicht, konvertiere ASCII-Zeichen in Dezimalwert
			MLANE      		    R3, R4, R2, R1       ; Multipliziere bisherigen Wert mit 10 und addiere neuen Wert
			BNE         		schleife_atouI       ; Wiederhole, falls noch Zeichen übrig

			MOV         		R0, R4               ; Speichere Endergebnis in R0
			LDMFD       		SP!, {R1-R4, R14}    ; Wiederherstellen der ursprünglichen Registerwerte
			BX          		LR                   ; Rückkehr zur aufrufenden Funktion

berechnung
			MOV         		R0, R0, LSL #16      ; Verschiebe die unteren 16 Bits nach oben
			ASR         		R0, R0, #16          ; Verschiebe arithmetisch nach rechts (übernimmt Vorzeichen)
			CMP        		    R0, #0               ; Prüfe, ob die Zahl negativ ist
			RSBMI       		R0, R0, #0           ; Falls negativ, mache sie positiv

			MOV         		R1, R0               ; R1 = Eingabewert (X)
			MOV         		R2, R1, LSL #1       ; R2 = 2 * X (Linksschieben um 1 Bit)

			LDR        			R1, =0xCCCCCCCD      ; Lade Magic Number für Division durch 10 -> Skript
			UMULL       		R3, R4, R2, R1       ; R2 * Magic Number -> Ergebnis in R3:R4
			MOV         		R2, R4, LSR #2       ; Teile Ergebnis (R4) durch 4 (Rechtsverschiebung um 2 Bits)

			MUL         		R0, R2, R2           ; Quadriere das Ergebnis (R0 = R2 * R2)
			BX          		LR                   ; Rückkehr zur aufrufenden Funktion

uitoa
			STMFD       		SP!, {R2-R7, LR}     ; Speichere Register R2-R7 und Rücksprungadresse
			MOV         		R2, #10              ; Basis (10) für Division
			LDR         		R3, =0xCCCD          ; Magic Number für Division durch 10
			MOV              	R4, #0               ; Zähler für die Anzahl der Ziffern

schleife
			CMP        			R0, #0               ; Prüfe, ob der Wert 0 ist
			MULNE    			R6, R0, R3           ; Multipliziere mit der Magic Number
			MOVNE       		R5, R6, LSR #19      ; Quotient ausrechnen (Rechtsverschiebung um 19 Bit)
			MULNE       		R6, R5, R2           ; R6 = R5 * 10
			SUBNE       		R6, R0, R6           ; Rest berechnen (R6 = R0 - (R5 * 10))
			ADDNE     	   		R6, #0x30            ; Umwandeln in ASCII ('0'-'9')
			STMFDNE     		SP!, {R6}            ; Speichere das Zeichen auf dem Stack
			MOVNE       		R0, R5               ; Aktualisiere R0 für nächste Iteration
			ADDNE       		R4, R4, #1           ; Zähler erhöhen
			BNE         		schleife             ; Wiederhole, falls noch Ziffern übrig
revstr
			CMP       		    R4, #0               ; Prüfe, ob noch Zeichen vorhanden sind
			LDMFDNE             SP!, {R6}            ; Hole Zeichen vom Stack
			STRBNE              R6, [R1], #1         ; Schreibe Zeichen in Speicher, R1++
			SUBNE               R4, R4, #1           ; Zähler verringern
			BNE        		    revstr               ; Wiederhole, bis alle Zeichen verarbeitet sind

			MOV       			R3, #0x00            ; Null-Terminator
			STRB        	    R3, [R1]             ; Schreibe Null-Terminator ans Ende

			LDMFD               SP!, {R2-R7, LR}     ; Wiederherstellen der ursprünglichen Registerwerte
			BX                  LR                   ; Rückkehr zur aufrufenden Funktion

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
String_1				       DCB				   "256",0x00
;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
		   ALIGN
		   END
