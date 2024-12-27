;********************************************************************
;* htw saar - Fakultaet fuer Ingenieurwissenschaften				*
;* Labor fuer Eingebettete Systeme									*
;* Mikroprozessortechnik											*
;********************************************************************
;* Assembler_Startup.S: 											*
;* Programmrumpf fuer Assembler-Programme mit dem Keil				*
;* Entwicklungsprogramm uVision fuer ARM-Mikrocontroller			*
;********************************************************************
;* Aufgabe-Nr.:         	*	          1     						*
;*              			*						    			*
;********************************************************************
;* Gruppen-Nr.: 			*				5						*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	*		Valentin Straßer	5014379						*
;*							*				Michal Roziel		5012845				*
;*							*										*
;********************************************************************
;* Abgabedatum:         	*          19.12.2024    							*
;*							*										*
;********************************************************************
;********************************************************************
;* Daten-Bereich bzw. Daten-Speicher				            	*
;********************************************************************
						AREA    Daten, DATA, READWRITE  ; Define a writable data area
Datenanfang
STR_1 			  	EQU Datenanfang +0x100
	
Stack_Anfang	EQU Datenanfang + 0x300
Top_Stack 		EQU Stack_Anfang + 0x100
STR_2			EQU Top_Stack +100


;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
						AREA		Programm, CODE, READONLY
						ARM
Reset_Handler	MSR			CPSR_c, #0x10	; User Mode aktivieren

;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************

						LDR			SP,=Top_Stack 		; Adresse des Werts laden
						LDR	   		R0,=String_1			; Wert laden
						LDR			R9,=0x0000FFFF
						BL	atouI
						AND	R0,R0,R9
						BL berechnung
						AND	R0,R0,R9
						LDR			R1,=STR_2
						BL	uitoa

				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos				B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************
atouI
					  STMFD SP!, {R1-R4, R14}		;bisherige Register retten
					  MOV		R2,#10			;R2 sei 10; die 10 wird benötigt um Platz zu schaffen
					  MOV		R3,#0			;R3 sei am Startpunkt 0
			  
schleife_atouI
					  LDRB	R1,[R0],#1		;Lade xtes Zeichen von String in R1, inkrementiere R0 um 
					  MOV		R4,R3
					  CMP		R1,#0x00		;Ust R1 == 0?
					  SUBNE	R1,R1,#0x30		; Nein: dekremetiere R1 um 0x30 um die tatsächliche Hex-Zahl zu erhalten 
					  MLANE	R3,R4,R2,R1		; Nein: Multipliziere das alte Erg mit 10, Addiere die neue Hex-Zahl hinzu, Speichere in R3
					  BNE schleife_atouI				; Nein: wiederhole Schleife
					 
					  MOV		R0,R4			;Schreibe Endergebnis nach R
					  LDMFD SP!, {R1-R4, R14}	;Lade urspruengliche Registerbelegung
					  BX LR
			  
berechnung
					 LDR R1, =0xCCCCCCCD
					 UMULL R2, R3, R0, R1
					 MOV  R0, R3, LSR #2

					 LSL R0, R0, #1 
					 MUL R8, R0, R0
					 MOV R0,R8
					 BX LR
				
uitoa
					STMFD SP!, {R2-R7, LR}   ; Save registers
					MOV   R2, #10            ; Base (10) for division
					LDR   R3, =0x1999A       ; Magic number for division by 10 (16-bit optimization)
					MOV   R4, #0             ; Digit count

schleife
					CMP   R0, #0             ; Check if the number is zero
					UMULLNE R6, R5, R0, R3     ; Multiply with the magic number
					MOVNE   R5, R6, LSR #20    ; Adjust quotient (shift right by 20 for 16-bit precision)
					MULNE   R6, R5, R2         ; R6 = R5 * 10
					SUBNE   R6, R0, R6         ; R6 = R0 - (R5 * 10), gives remainder
					ADDNE   R6, #0x30          ; Convert remainder to ASCII ('0'-'9')
					STMFDNE SP!, {R6}          ; Push character to stack
					MOVNE   R0, R5             ; Update R0 for next iteration
					ADDNE   R4, R4, #1         ; Increment digit counter
					BNE     schleife

revstr
					CMP   R4, #0
					LDMFDNE SP!, {R6}        ; Pop characters from stack
					STRBNE R6, [R1], #1      ; Write characters to memory, increment pointer
					SUBNE R4, R4, #1         ; Decrement counter
					BNE   revstr

					MOV   R3, #0x00          ; Null terminator
					STRB  R3, [R1]           ; Write null terminator

					LDMFD SP!, {R2-R7, LR}   ; Restore registers
					BX    LR                 ; Return

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
String_1		DCB		"123",0x00
;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				END
