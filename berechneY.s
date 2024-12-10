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
				
				BL 	Berechnung
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************

Berechnung


	; DIE ZAHL ITS IN R0 
; Assume the dividend is in R0
; The result will be in R6
; Perform division using shifts and subtraction


						LDR R1, =0xCCCCCCCD
						UMULL R2, R3, R0, R1
						MOV  R0, R3, LSR #2

						
						LSL R0, R0, #1 
						
					
						; DA GILT : X < 2°16 : Wir können MUL benutzen 
	
						MUL R8, R0, R0
						
		
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
