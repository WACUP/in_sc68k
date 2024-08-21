;;; SoundFX replay routine
;;; adapted for sc68 by Gerald Schnabel <gschnabel@gmx.de>
;;;
;;; Clean-up by Ben G.

	opt	o+,p+

SFX_DELAY:	set	0x38e5		; Base delay ~122 BPM

	;; SFX header
	rsreset
	;; ------------------------------------.
hd_magic:	rs.l	1		; "SONG"
hd_delay:	rs.w	1		; Delay (default=0x38e5)
hd_reserved:	rs.w	7		;
hd_size:	rs.b	0		; -> 20 bytes
	;; ------------------------------------'

	;; SFX instrument
	rsreset
	;; ------------------------------------.
si_name:	rs.b	22		; Name
si_len:	rs.w	1		; Length in words
si_tune:	rs.b	1		; Fine tune
si_volume:	rs.b	1		; Volume 0-63
si_lpoff:	rs.w	1		; Loop offset (in bytes)
si_lplen:	rs.w	1		; Loop len (in words)
si_size:	rs.b	0		; -> 30 bytes
	;; ------------------------------------'

	;; SFX sequence header
	rsreset
	;; ------------------------------------.
sh_len:	rs.b	1		; Song length
sh_restart:	rs.b	1		; Restart position
sh_seqdata:	rs.b	128		; Pattern sequence
sh_size:	rs.b	0		; -> 130 bytes
	;; ------------------------------------'


	bra.w	fx_init
	bra.w	fx_stop
	bra.w	fx_play

fx_init:
	movem.l	d0-a6,-(a7)

	;; Check mod version:
	lea	Base(pc),a6
	basereg	Base,a6
	clr.l	va_modPtr(a6)

	;; Looking for 'SONG' magic to detect 15 or 31 instruments
	move.l	#"SONG",d0
	lea	15*4(a0),a1
	cmp.l	(a1),d0
	bne.s	not15
	;; Mod v1.3 (15 instruments)
	moveq	#15,d1
	lea	hd_size(a1),a2
	lea	15*si_size(a2),a3
	bra.s	storePtrs
not15:
	lea	31*4(a0),a1
	cmp.l	(a1),d0
	bne	fx_error
	;; Mod v2.0 (31 instruments)
	moveq	#31,d1
	lea	hd_size(a1),a2
	lea	31*si_size(a2),a3
storePtrs:
	move.l	a0,va_modPtr(a6)
	move.l	a1,va_hdrPtr(a6)
	move.w	d1,va_nbInst(a6)
	move.l	a2,va_insPtr(a6)
	move.l	a3,va_seqPtr(a6)


	moveq	#31,d1
	lea	31*4(a0),a1
	cmp.l	(a1),d0
	bne	fx_error
.found:
	move.w	d1,va_nbi(a6)
	move.l	a0,va_modPtr(a6)
	move.l	a1,a0	  ; a0: header

	lea	60(a0),a0	  ;Laengentabelle ueberspringen

	hd_delay

	move.b	470(a0),AnzPat+1	;Laenge des Sounds
	movem.l	d1-d7/a0-a6,-(a7)

	;;
	move.l	va_modPtr,a0
	lea	532(a0),a0
	move	AnzPat(pc),d2	;wieviel Positions
	subq	#1,d2		;für dbf
	moveq	#0,d1
	moveq	#0,d0
SongLenLoop:
	move.b	(a0)+,d0	          ;Patternnummer holen
	cmp.b	d0,d1	          ;ist es die höchste ?
	bhi.s	LenHigher	          ;nein!
	move.b	d0,d1	          ;ja
LenHigher:
	dbf	d2,SongLenLoop
	move.l	d1,d0	  ;Hoechste BlockNummer nach d0
	addq	#1,d0	  ;plus 1
	mulu	#1024,d0	  ;Laenge eines Block
	movem.l	(a7)+,d1-d7/a0-a6
	add.l	d0,a0			;Zur Adresse der Songstr.
	add.w	#600,a0			;Laenge der SongStr.
	move.l	va_modPtr(pc),a2
	lea	Instruments(pc),a1	;Tabelle auf Samples
	moveq	#14,d7			;15 Instrumente
CalcIns:
	move.l	a0,(A1)+		;Startadresse des Instr.
	add.l	(a2)+,a0		;berechnen un speichern
	dbf	d7,CalcIns
	lea	Instruments(pc),a0	;Zeiger auf instr.Tabelle
	moveq	#14,d7			;15 Instrumente
	lea	$dff000,a0		;AMIGA
	move.w	#-1,PlayLock		;player zulassen
	clr	$a8(A0)			;Alle Voloumenregs. auf 0
	clr	$b8(A0)
	clr	$c8(a0)
	clr	$d8(a0)
	clr.w	Timer			;zahler auf 0
	clr.l	TrackPos		;zeiger auf pos
	clr.l	PosCounter		;zeiger innehalb des pattern

fx_error
	movem.l	(a7),d0-a6


	rts			;

fx_kill:	;; ------------------------------------.
	pea	(a6)		;
	lea	Base(pc),a6		;
	clr.l	va_modPtr(a6)	;
	clr.w	$dff0a8		; Mute voices
	clr.w	$dff0b8		;
	clr.w	$dff0c8		;
	clr.w	$dff0d8		;
	move.w	#$f,$dff096		; Disable Audio DMA
	move.l	(a7)+,a6		;
	;; ------------------------------------'
	rts

fx_play:	;; ------------------------------------.
	pea	(a6)		;
	lea	Base(pc),a6		;
	tst.l	va_modPtr(a6)	;

	move.w	va_timerCnt(a6),d0
	add.w	va_timerAdd(a6),d0
	move.w	d0,va_timerCnt(a6)
	bcc.s	CheckEffects
	bsr	PlaySound
NoPlay:
	rts

CheckEffects:
	moveq	#3,d7		;4 voices
	lea	StepControl0,a4
	lea	ChannelData0(pc),a6	;zeiger auf daten für 0
	lea	$dff0a0,a5		; Voice A
EffLoop:
	movem.l	d7/a5,-(a7)
	bsr.s	MakeEffekts		; Effekt spielen
	movem.l	(a7)+,d7/a5
NoEff:
	addq	#8,a4
	add	#$10,a5	            ; nächster Kanal
	add	#22,a6	            ; Nächste KanalDaten
	dbf	d7,EffLoop
	movem.l	(a7)+,d0-d7/a0-a6
	rts

MakeEffekts:
	move	(a4),d0
	beq.s	NoStep
	bmi.s	StepItUp
	add	d0,2(a4)
	move	2(a4),d0
	move	4(a4),d1
	cmp	d0,d1
	bhi.s	StepOk
	move	d1,d0
StepOk:
	move	d0,6(a5)
	move	d0,2(a4)
	rts

StepItUp:
	add	d0,2(a4)
	move	2(a4),d0
	move	4(a4),d1
	cmp	d0,d1
	blt.s	StepOk
	move	d1,d0
	bra.s	StepOk

NoStep:
	move.b	2(a6),d0
	and.b	#$0f,d0
	cmp.b	#1,d0
	beq	appreggiato
	cmp.b	#2,d0
	beq	pitchbend
	cmp.b	#3,d0
	beq	LedOn
	cmp.b	#4,d0
	beq	LedOff
	cmp.b	#7,d0
	beq.s	SetStepUp
	cmp.b	#8,d0
	beq.s	SetStepDown
	rts

LedOn:
	bset	#1,$bfe001
	rts
LedOff:
	bclr	#1,$bfe001
	rts

SetStepUp:
	moveq	#0,d4
StepFinder:
	clr	(a4)
	move	(A6),2(a4)
	moveq	#0,d2
	move.b	3(a6),d2
	and	#$0f,d2
	tst	d4
	beq.s	NoNegIt
	neg	d2
NoNegIt:
	move	d2,(a4)
	moveq	#0,d2
	move.b	3(a6),d2
	lsr	#4,d2
	move	(a6),d0
	lea	NoteTable,a0

StepUpFindLoop:
	move	(A0),d1
	cmp	#-1,d1
	beq.s	EndStepUpFind
	cmp	d1,d0
	beq.s	StepUpFound
	addq	#2,a0
	bra.s	StepUpFindLoop
StepUpFound:
	lsl	#1,d2
	tst	d4
	bne.s	NoNegStep
	neg	d2
NoNegStep:
	move	(a0,d2.w),d0
	move	d0,4(A4)
	rts

EndStepUpFind:
	move	d0,4(A4)
	rts

SetStepDown:
	st	d4
	bra.s	StepFinder

StepControl0:
	dc.l	0,0
StepControl1:
	dc.l	0,0
StepControl2:
	dc.l	0,0
StepControl3:
	dc.l	0,0

appreggiato:
	lea	ArpeTable(pc),a0
	moveq	#0,d0
	move	Timer,d0
	subq	#1,d0
	lsl	#2,d0
	move.l	(A0,d0.l),a0
	jmp	(A0)

Arpe4:	lsl.l	#1,d0
	clr.l	d1
	move.w	16(a6),d1
	lea.l	NoteTable(pc),a0
Arpe5:	move.w	(a0,d0.l),d2
	cmp.w	(a0),d1
	beq.s	Arpe6
	addq.l	#2,a0
	bra.s	Arpe5

Arpe1:	clr.l	d0
	move.b	3(a6),d0
	lsr.b	#4,d0
	bra.s	Arpe4

Arpe2:	clr.l	d0
	move.b	3(a6),d0
	and.b	#$0f,d0
	bra.s	Arpe4

Arpe3:	move.w	16(a6),d2

Arpe6:	move.w	d2,6(a5)
	rts

pitchbend:
	clr.l	d0
	move.b	3(a6),d0
	lsr.b	#4,d0
	tst.b	d0
	beq.s	pitch2
	add.w	d0,(a6)
	move.w	(a6),6(a5)
	rts
pitch2:	clr.l	d0
	move.b	3(a6),d0
	and.b	#$0f,d0
	tst.b	d0
	beq.s	pitch3
	sub.w	d0,(a6)
	move.w	(a6),6(a5)
pitch3:	rts

PlaySound:
	move.l	va_modPtr(pc),a0	;Zeiger auf SongFile
	add	#60,a0			;Laengentabelle ueberspringen
	move.l	a0,a3
	move.l	a0,a2
	lea	600(A0),a0		;Zeiger auf BlockDaten
	add	#472,a2			;zeiger auf Patterntab.
	add	#12,a3			;zeiger auf Instr.Daten
	move.l	TrackPos(pc),d0		;Postionzeiger
	clr.l	d1
	move.b	(a2,d0.l),d1		;dazugehörige PatternNr. holen
	moveq	#10,d7
	lsl.l	d7,d1			;*1024 / länge eines Pattern
	add.l	PosCounter,d1		;Offset ins Pattern
	clr.w	DmaCon
	lea	StepControl0(pc),a4
	lea	$dff0a0,a5		;Zeiger auf Kanal0
	lea	ChannelData0(pc),a6	;Daten für Kanal0
	moveq	#3,d7			;4 Kanäle
SoundHandleLoop:
	bsr	PlayNote		;aktuelle Note spielen
	add	#8,a4
	add.l	#$10,a5			;nächster Kanal
	add.l	#22,a6			;nächste Daten
	dbf	d7,SoundHandleLoop	;4*

	move	DmaCon(pc),d0		;DmaBits
	bset	#15,d0			;Clear or Set Bit setzen
	move.w	d0,$dff096		;DMA ein!

	move	#300,d0			;Verzögern (genug für MC68030)
Delay2:
	dbf	d0,Delay2

	lea	ChannelData3(pc),a6
	lea	$dff0d0,a5
	moveq	#3,d7
SetRegsLoop:
	move.l	10(a6),(a5)		;Adresse
	move	14(a6),4(a5)		;Länge
NoSetRegs:
	sub	#22,a6			;nächste Daten
	sub	#$10,a5			;nächster Kanal
	dbf	d7,SetRegsLoop
	tst	PlayLock
	beq.s	NoEndPattern
	add.l	#16,PosCounter		;PatternPos erhöhen
	cmp.l	#1024,PosCounter	;schon Ende ?
	blt.s	NoEndPattern

	clr.l	PosCounter		;PatternPos löschen
	addq.l	#1,TrackPos		;Position erhöhen
NoAddPos:
	move.w	AnzPat(pc),d0		;AnzahlPosition
	move.l	TrackPos(pc),d1		;Aktuelle Pos
	cmp.w	d0,d1			;Ende?
	bne.s	NoEndPattern		;nein!
	clr.l	TrackPos		;ja/ Sound von vorne
NoEndPattern:
	rts

PlayNote:
	clr.l	(A6)
	tst	PlayLock		;Player zugellassen ?
	beq.s	NoGetNote		;
	move.l	(a0,d1.l),(a6)		;Aktuelle Note holen
NoGetNote:
	addq.l	#4,d1			;PattenOffset + 4
	moveq	#0,d2
	cmp	#-3,(A6)		;Ist Note = 'PIC' ?
	beq	NoInstr2		;wenn ja -> ignorieren
	move.b	2(a6),d2		;Instr Nummer holen
	and.b	#$f0,d2			;ausmaskieren
	lsr.b	#4,d2			;ins untere Nibble
	tst.b	d2			;kein Intrument ?
	beq.w	NoInstr2		;wenn ja -> überspringen

	moveq	#0,d3
	lea.l	Instruments(pc),a1	;Instr. Tabelle
	move.l	d2,d4			;Instrument Nummer
	subq	#1,d2
	lsl	#2,d2			;Offset auf akt. Instr.
	mulu	#30,d4			;Offset Auf Instr.Daten
	move.l	(a1,d2.w),4(a6)		;Zeiger auf akt. Instr.
	move.w	(a3,d4.l),8(a6)		;Instr.Länge
	move.w	2(a3,d4.l),18(a6)	;Volume
	move.w	4(a3,d4.l),d3		;Repeat
	tst	d3			;kein Repeat?
	beq.s	NoRepeat		;Nein!
					;Doch!

	move.l	4(a6),d2		;akt. Instr.
	add.l	d3,d2			;Repeat dazu
	move.l	d2,10(a6)		;Repeat Instr.
	move.w	6(a3,d4),14(a6)		;rep laenge
	move.w	18(a6),d3		;Volume in HardReg.
	bra.s	NoInstr

NoRepeat:
	move.l	4(a6),d2		;Instrument
	add.l	d3,d2			;rep Offset
	move.l	d2,10(a6)		;in Rep. Pos.
	move.w	6(a3,d4.l),14(a6)	;rep Laenge
	move.w	18(a6),d3		;Volume in Hardware

CheckPic:
NoInstr:
	move.b	2(a6),d2
	and	#$0f,d2
	cmp.b	#5,d2
	beq.s	ChangeUpVolume
	cmp.b	#6,d2
	bne.W	SetVolume2
	moveq	#0,d2
	move.b	3(a6),d2
	sub	d2,d3
	tst	d3
	bpl	SetVolume2
	clr	d3
	bra.W	SetVolume2
ChangeUpVolume:
	moveq	#0,d2
	move.b	3(A6),d2
	add	d2,d3
	tst	d3
	cmp	#64,d3
	ble.W	SetVolume2
	move	#64,d3
SetVolume2:
	move	d3,8(a5)

NoInstr2:
	cmp	#-3,(a6)		;Ist Note = 'PIC' ?
	bne.s	NoPic
	clr	2(a6)			;wenn ja -> Note auf 0 setzen
	bra.s	NoNote
NoPic:
	tst	(a6)			;Note ?
	beq.s	NoNote			;wenn 0 -> nicht spielen

	clr	(a4)
	move.w	(a6),16(a6)		;eintragen
	move.w	20(a6),$dff096		;dma abschalten


	move.l	d7,-(a7)
	move	#300,d7			;genug für MC68030
Delay1:
	dbf	d7,Delay1		;delay
	move.l	(a7)+,d7

	cmp	#-2,(a6)		;Ist es 'STP'
	bne.s	NoStop			;Nein!
	clr	8(a5)
	bra	Super
NoStop:
	move.l	4(a6),0(a5)		;Intrument Adr.
	move.w	8(a6),4(a5)		;Länge
	move.w	0(a6),6(a5)		;Period
Super:
	move.w	20(a6),d0		;DMA Bit
	or.w	d0,DmaCon		;einodern
NoNote:
	rts

ArpeTable:
	dc.l	Arpe1
	dc.l	Arpe2
	dc.l	Arpe3
	dc.l	Arpe2
	dc.l	Arpe1

ChannelData0:
	ds.l	5,0		;Daten für Note
	dc.w	1		;DMA - Bit
ChannelData1:
	ds.l	5,0		;u.s.w
	dc.w	2
ChannelData2:
	ds.l	5,0		;etc.
	dc.w	4
ChannelData3:
	ds.l	5,0		;a.s.o
	dc.w	8
Instruments:
	ds.l	15,0	 ;Zeiger auf die 15 Instrumente
PosCounter:
	dc.l	0	            ;Offset ins Pattern
TrackPos:
	dc.l	0		;Position Counter
Timer:
	dc.w	0		;Zähler 0-5
DmaCon:
	dc.w	0	   ;Zwischenspeicher für DmaCon
AnzPat:
	dc.w	1		;Anzahl Positions
PlayLock:
	dc.w	0	     ;Flag fuer 'Sound erlaubt'
va_modPtr:
	dc.l	0

Reserve:
	dc.w	856,856,856,856,856,856,856,856,856,856,856,856
NoteTable:
	dc.w	856,808,762,720,678,640,604,570,538,508,480,453 ;1.Okt
	dc.w	428,404,381,360,339,320,302,285,269,254,240,226 ;2.Okt
	dc.w	214,202,190,180,170,160,151,143,135,127,120,113 ;3.Okt
	dc.w	113,113,113,113,113,113,113,113,113,113,113,113 ;Reserve
	dc.w	-1
