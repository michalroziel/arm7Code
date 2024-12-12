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
;* Gruppen-Nr.: 	Valentin Straßer	5014379						*
;*			Michal Roziel		5012845			*										*
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
	
		LDR			SP,=Stack_Ende 		; Adresse des Werts laden
		LDR	   		R0,=String_1			; Wert laden
		BL	atouI
		BL berechnung
		;BL	uitoa
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

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
		 BX LR
	
uitoa
		 STMFD SP!,{R1-R7,R14} ;Registerbelegung retten
		 LDR r1,=Ergebnis
		 MOV r2,#10
		 LDR r3,=0xCCCCCCCD ;Magic number

   
                 LDMFD SP!, {R1-R7, R14}	;Lade urspruengliche Registerbelegung
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
