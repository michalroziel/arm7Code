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
STR_2			EQU Top_Stack 


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
						LDR	   		R0,=STR_1			; Wert laden
						BL	atouI
						BL berechnung
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
					 MUL R0, R0, R0			; changed Return R8 to R0
					 BX LR
				
uitoa
					STMFD SP!,{R2-R7,R14} ;Registerbelegung retten
				    MOV r2,#10 ; für mal 10
				    LDR r3,=0xCCCCCCCD ;Magic number
					MOV r4,#0 ; zähler
schleife_uitoa
			           CMP		R0,#0				;Ist R4 == 0?
				   MOVNE		R5,R0				;Lade Wert von R0 nach R5
                  	           UMULLNE   r6, r0, r5, r3   ; // Multiply number (r0) by magic number, store high result in r1
				   LSRNE     r0, r0, #19       ;// Shift right to get quotient in r1
				   MULNE     r6, r0, r2       ;// Multiply quotient by 10
				   SUBNE     r5, r5, r6        ;// Subtract (Quotient * 10) from original number to get remainder

				   ADDNE	R5,#0x30			; Nein: Zahl in R6 um #0x30 Hochaddieren um die Hex-Zahl zur Ausgabe zu erhalten
				   STMFDNE	SP!,{R5}
				   ADDNE	R4,R4,#1			; Nein: Erhoehe Zeichenzaehler R7 um 1	   
				   BNE		schleife_uitoa				; Nein: Ruecksprung
revstr
				  MOV R3,#0x00
				  CMP	R4,#0
				  LDMFDNE SP!,{R5}
				  STRBNE R5,[R1],#1
				  SUBNE	R4, R4, #1
				  BNE	revstr
				  STRB R3,[R1]
				  
			          LDMFD SP!, {R2-R7, R14}	;Lade urspruengliche Registerbelegung
			          BX LR


;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
String_1		DCB		"100",0x00
;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				END
