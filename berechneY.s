;********************************************************************
;* htw saar - Fakultaet fuer Ingenieurwissenschaften				*
;* Labor fuer Eingebettete Systeme									*
;* Mikroprozessortechnik											*
;********************************************************************
;* Assembler_Startup.S: 											*
;* Programmrumpf fuer Assembler-Programme mit dem Keil				*
;* Entwicklungsprogramm uVision fuer ARM-Mikrocontroller			*
;********************************************************************
;* Aufgabe-Nr.:         	*	               						*
;*              			*						    			*
;********************************************************************
;* Gruppen-Nr.: 			*										*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	*										*
;*							*										*
;*							*										*
;********************************************************************
;* Abgabedatum:         	*              							*
;*							*										*
;********************************************************************

;********************************************************************
;* Daten-Bereich bzw. Daten-Speicher				            	*
;********************************************************************
				AREA		Daten, DATA, READWRITE
Datenanfang
	
;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
				AREA		Programm, CODE, READONLY
				ARM
Reset_Handler	MSR			CPSR_c, #0x10	; User Mode aktivieren

;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************
	
				LDR R0, =Zahl   ; Lade die Adresse der Zahl
				LDR	R0, [R0]	; Lade die Zahl aus der Adresse 
				
				MOV R1, #5
				MOV R3, #0
				
				CMP R1, #0x00
				BEQ endlos
				
				BL 	Berechnung
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************

Berechnung


JUMP                
                        ;*** Jetzt durch 5 teilen -> Divident in RO
						
						CMP R0, R1	; Vergleiche Divident und Divisor ! 
						BMI FERTIG	; Wenn N-Flag gesetzt ist, springe zum Label fertig 
						SUB R0, R0, R1 ; Subtrahiere den Divisor von dem Divident 
						ADD R2, R2, #1 ; Inkrementiere den Quotienten um 1 
						B JUMP			; Bedingungs-unabhängiger Jump

FERTIG
                        MOV R7, R0      ; Rest der Übrig bleibt
                        MOV R6, R2      ; Quotient
						
						
						; X/5 WURDE BERECHNET 
						; JETZT : MIT 2 MULT 
						
						LSL R6, R6, #1 
						
						; 2/5 * X WURDE BERECHNET 
						; JETZT alles hoch 2 
						; DA GILT : X < 2°16 : Wir können MUL benutzen 
	
						MUL R8, R6, R6 
						
		
						BX LR
		

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************

Zahl 		DCW	0x00000064


;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				SPACE 2
				END
