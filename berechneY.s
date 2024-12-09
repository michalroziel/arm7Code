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


	MOV R5, R0          ; Copy the original number (R0) to R5
    MOV R6, #51         ; Load the constant (scaled factor for 1/5)
    MUL R4, R5, R6      ; Multiply R0 by the scaled factor (R4 = R0 * 51)
    ADD R4, R4, #128    ; Add bias for rounding (128 = 2^7)
    ASR R4, R4, #8      ; Arithmetic shift right by 8 (divide by 256)

					
						


						
						; X/5 WURDE BERECHNET 
						; JETZT : MIT 2 MULT 
						
						LSL R4, R4, #1 
						
					
						; DA GILT : X < 2°16 : Wir können MUL benutzen 
	
						MUL R8, R4, R4 
						
		
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
