;;; ****************************************
;;; *** JamCrackerPro V1.0a play-routine ***
;;; ***   Originally coded by M. Gemmel  ***
;;; ***           Code optimised         ***
;;; ***         by Xag of Betrayal       ***
;;; ***    See docs for important info   ***
;;; ****************************************
;;; PIC modified version for sc68 by Ben G.

	opt	o+,w+,a+,p+

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Relative offset definitions ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	RsReset			; Instrument info structure
it_name:		rs.b	31
it_flags:		rs.b	1
it_size:		rs.l	1
it_address:	rs.l	1
it_sizeof:	rs.w	0

	RsReset			; Pattern info structure
pt_size:		rs.w	1
pt_address:	rs.l	1
pt_sizeof:	rs.w	0

	RsReset			; Note info structure
nt_period:	rs.b	1
nt_instr:		rs.b	1
nt_speed:		rs.b	1
nt_arpeggio:	rs.b	1
nt_vibrato:	rs.b	1
nt_phase:		rs.b	1
nt_volume:	rs.b	1
nt_porta:		rs.b	1
nt_sizeof:	rs.w	0

	RsReset			; Voice info structure
pv_waveoffset:	rs.w	1
pv_dmacon:	rs.w	1
pv_custbase:	rs.l	1
pv_inslen:	rs.w	1
pv_insaddress:	rs.l	1
pv_peraddress:	rs.l	1
pv_pers:		rs.w	3
pv_por:		rs.w	1
pv_deltapor:	rs.w	1
pv_porlevel	rs.w	1
pv_vib:		rs.w	1
pv_deltavib:	rs.w	1
pv_vol:		rs.w	1
pv_deltavol:	rs.w	1
pv_vollevel:	rs.w	1
pv_phase:		rs.w	1
pv_deltaphase:	rs.w	1
pv_vibcnt:	rs.b	1
pv_vibmax:	rs.b	1
pv_flags:		rs.b	1
pv_sizeof:	rs.w	0

	section	.text

;;;;;;;;;;;;;;;;;;;;
;;; Test routine ;;;
;;;;;;;;;;;;;;;;;;;;

	IfD TEST
	opt	p-
Start:	;; ---------------------------.
	move.l	4.w,a6		; Amiga DOS stuff
	jsr	-132(a6)		;
	lea	music(pc),a0	; Music player init
	jsr	pp_init		;
sync:	cmp.b	#$50,$DFF006	; Sync at raster line #$50
	bne.s	sync		;
	move.w	#$0F00,$DFF180	; Some red
	jsr	pp_play		; Play music
	move.w	#$0000,$DFF180	; Back to black
	btst	#6,$BFE001	; Mouse button ?
	bne.s	.loop		; Nope -> loop
	jsr	pp_end		; Kill sound
	move.l	4.w,a6		; More Amiga DOS stuff
	jsr	-138(a6)		;
	moveq	#0,d0		; No error code (probably)
	rts			;
music:	incbin	"some.jam"	; Incbin JCP music module
	even			;
	;; ---------------------------'
	opt	p+
	EndC

;;;;;;;;;;;;;;;;;
;;; sc68 stub ;;;
;;;;;;;;;;;;;;;;;

	bra.w	pp_init		; a0: music
	bra.w	pp_kill
	bra.w	pp_play


;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Initialise routine ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;

	basereg	base,a5
pp_init:	;; ---------------------------.
	movem.l	d0-a6,-(a7)

	lea	base(pc),a5	; The basereg
	move.l	a0,_song(a5)

	addq.w	#4,a0
	move.w	(a0)+,d0
	move.w	d0,d1
	move.l	a0,instable(a5)
	mulu	#it_sizeof,d0
	add.w	d0,a0

	move.w	(a0)+,d0
	move.w	d0,d2
	move.l	a0,patttable(a5)
	mulu	#pt_sizeof,d0
	add.w	d0,a0

	move.w	(a0)+,d0
	move.w	d0,songlen(a5)
	move.l	a0,songtable(a5)
	add.w	d0,d0
	add.w	d0,a0

	move.l	patttable(a5),a1
	move.w	d2,d0
	subq.w	#1,d0
.l0:
	move.l	a0,pt_address(a1)
	move.w	(a1),d3		;pt_size
	mulu	#nt_sizeof*4,d3
	add.w	d3,a0
	addq.w	#pt_sizeof,a1
	dbra	d0,.l0

	move.l	instable(a5),a1
	move.w	d1,d0
	subq.w	#1,d0
.l1:
	move.l	a0,it_address(a1)
	move.l	it_size(a1),d2
	add.l	d2,a0
	lea	it_sizeof(a1),a1
	dbra	d0,.l1

	move.l	songtable(a5),pp_songptr(a5)
	move.w	songlen(a5),pp_songcnt(a5)
	move.l	pp_songptr(a5),a0
	move.w	(a0),d0
	mulu	#pt_sizeof,d0
	add.l	patttable(a5),d0
	move.l	d0,a0
	move.l	a0,pp_pattentry(a5)
	move.b	pt_size+1(a0),pp_notecnt(a5)
	move.l	pt_address(a0),pp_address(a5)
	move.b	#6,pp_wait(a5)
	move.b	#1,pp_waitcnt(a5)
	clr.w	pp_nullwave(a5)
	move.w	#$000F,$DFF096 ; disable all audio DMAs

	lea	pp_variables(a5),a0
	lea	$DFF0A0,a1
	moveq	#1,d1
	move.w	#$80,d2
	moveq	#4-1,d0
.l2:
	move.w	#0,8(a1)		; volume 0
	move.w	d2,(a0)		; pv_waveoffset
	move.w	d1,pv_dmacon(a0)
	move.l	a1,pv_custbase(a0)

	pea	pp_periods(a5)
	move.l	(a7)+,pv_peraddress(a0)

	move.w	#1019,pv_pers(a0)
	clr.w	pv_pers+2(a0)
	clr.w	pv_pers+4(a0)
	clr.l	pv_por(a0)
	clr.w	pv_porlevel(a0)
	clr.l	pv_vib(a0)
	clr.l	pv_vol(a0)
	move.w	#$40,pv_vollevel(a0)
	clr.l	pv_phase(a0)
	clr.w	pv_vibcnt(a0)
	clr.b	pv_flags(a0)
	lea	pv_sizeof(a0),a0
	lea	$10(a1),a1
	add.w	d1,d1
	add.w	#$40,d2
	dbra	d0,.l2

	bset	#1,$BFE001         ; Filter
	;; --------------------------'
	movem.l	(a7)+,d0-a6
	rts

;;;;;;;;;;;;;;;;;;;;;;;;
;;; Clean-up routine ;;;
;;;;;;;;;;;;;;;;;;;;;;;;

pp_kill:	;; ---------------------------.
	clr.w	$DFF0A8		; Volume to zero
	clr.w	$DFF0B8		;
	clr.w	$DFF0C8		;
	clr.w	$DFF0D8		;
	move.w	#$F,$DFF096	; Disable audio DMAs
	bclr	#1,$BFE001	; Filter Off
	rts			;
	;; ---------------------------'

;;;;;;;;;;;;;;;;;;;;
;;; Play routine ;;;
;;;;;;;;;;;;;;;;;;;;

pp_play:	;; ---------------------------.
	movem.l	d0-a6,-(a7)	;
	bsr.s	pp_real		;
	movem.l	(a7)+,d0-a6	;
	rts			;
	;; ---------------------------'

dma_wait:	bsr	dma_w8
dma_w8:	move.b	$DFF006,d0
ras_w8:	cmp.b	$DFF006,d0
	beq.s	ras_w8
	rts

pp_real:
	lea	base(pc),a5	; The basereg
	lea	$DFF000,a6
	subq.b	#1,pp_waitcnt(a5)
	bne.s	.l0
	bsr	pp_nwnt
	move.b	pp_wait(a5),pp_waitcnt(a5)

.l0:
	lea	pp_variables(a5),a1
	bsr.s	pp_uvs
	lea	pp_variables+pv_sizeof(a5),a1
	bsr.s	pp_uvs
	lea	pp_variables+2*pv_sizeof(a5),a1
	bsr.s	pp_uvs
	lea	pp_variables+3*pv_sizeof(a5),a1

pp_uvs:
	move.l	pv_custbase(a1),a0

.l0:
	move.w	pv_pers(a1),d0
	bne.s	.l1
	bsr	pp_rot
	bra.s	.l0
.l1:
	add.w	pv_por(a1),d0
	tst.w	pv_por(a1)
	beq.s	.l1c
	bpl.s	.l1a
	cmp.w	pv_porlevel(a1),d0
	bge.s	.l1c
	bra.s	.l1b
.l1a:
	cmp.w	pv_porlevel(a1),d0
	ble.s	.l1c
.l1b:
	move.w	pv_porlevel(a1),d0

.l1c:
	add.w	pv_vib(a1),d0
	cmp.w	#135,d0
	bge.s	.l1d
	move.w	#135,d0
	bra.s	.l1e
.l1d:
	cmp.w	#1019,d0
	ble.s	.l1e
	move.w	#1019,d0
.l1e:
	move.w	d0,6(a0)
	bsr	pp_rot

	move.w	pv_deltapor(a1),d0
	add.w	d0,pv_por(a1)
	cmp.w	#-1019,pv_por(a1)
	bge.s	.l3
	move.w	#-1019,pv_por(a1)
	bra.s	.l5
.l3:
	cmp.w	#1019,pv_por(a1)
	ble.s	.l5
	move.w	#1019,pv_por(a1)

.l5:
	tst.b	pv_vibcnt(a1)
	beq.s	.l7
	move.w	pv_deltavib(a1),d0
	add.w	d0,pv_vib(a1)
	subq.b	#1,pv_vibcnt(a1)
	bne.s	.l7
	neg.w	pv_deltavib(a1)
	move.b	pv_vibmax(a1),pv_vibcnt(a1)

.l7:
	move.w	pv_dmacon(a1),d0
	move.w	pv_vol(a1),8(a0)
	move.w	pv_deltavol(a1),d0
	add.w	d0,pv_vol(a1)
	tst.w	pv_vol(a1)
	bpl.s	.l8
	clr.w	pv_vol(a1)
	bra.s	.la
.l8:
	cmp.w	#$40,pv_vol(a1)
	ble.s	.la
	move.w	#$40,pv_vol(a1)

.la:
	btst	#1,pv_flags(a1)
	beq.s	.l10
	tst.w	pv_deltaphase(a1)
	beq.s	.l10
	bpl.s	.sk
	clr.w	pv_deltaphase(a1)
.sk:
	move.l	pv_insaddress(a1),a0
	move.w	(a1),d0		;pv_waveoffset
	neg.w	d0
	lea	(a0,d0.w),a2
	move.l	a2,a3
	move.w	pv_phase(a1),d0
	lsr.w	#2,d0
	add.w	d0,a3

	moveq	#$40-1,d0
.lb:
	move.b	(a2)+,d1
	ext.w	d1
	move.b	(a3)+,d2
	ext.w	d2
	add.w	d1,d2
	asr.w	#1,d2
	move.b	d2,(a0)+
	dbra	d0,.lb

	move.w	pv_deltaphase(a1),d0
	add.w	d0,pv_phase(a1)
	cmp.w	#$100,pv_phase(a1)
	blt.s	.l10
	sub.w	#$100,pv_phase(a1)

.l10:
	rts

pp_rot:
	move.w	pv_pers(a1),d0
	move.w	pv_pers+2(a1),pv_pers(a1)
	move.w	pv_pers+4(a1),pv_pers+2(a1)
	move.w	d0,pv_pers+4(a1)
	rts

pp_nwnt:
	move.l	pp_address(a5),a0
	add.l	#4*nt_sizeof,pp_address(a5)
	subq.b	#1,pp_notecnt(a5)
	bne.s	.l5

.l0:
	addq.l	#2,pp_songptr(a5)
	subq.w	#1,pp_songcnt(a5)
	bne.s	.l1
	move.l	songtable(a5),pp_songptr(a5)
	move.w	songlen(a5),pp_songcnt(a5)
.l1:
	move.l	pp_songptr(a5),a1
	move.w	(a1),d0
	mulu	#pt_sizeof,d0
	add.l	patttable(a5),d0
	move.l	d0,a1
	move.b	pt_size+1(a1),pp_notecnt(a5)
	move.l	pt_address(a1),pp_address(a5)

.l5:
	clr.w	pp_tmpdmacon(a5)
	lea	pp_variables(a5),a1
	bsr	pp_nnt
	addq.w	#nt_sizeof,a0
	lea	pp_variables+pv_sizeof(a5),a1
	bsr	pp_nnt
	addq.w	#nt_sizeof,a0
	lea	pp_variables+2*pv_sizeof(a5),a1
	bsr	pp_nnt
	addq.w	#nt_sizeof,a0
	lea	pp_variables+3*pv_sizeof(a5),a1
	bsr	pp_nnt

	move.w	pp_tmpdmacon(a5),$96(a6)

	bsr	dma_wait

	lea	pp_variables(a5),a1
	bsr.s	pp_scr
	lea	pp_variables+pv_sizeof(a5),a1
	bsr.s	pp_scr
	lea	pp_variables+2*pv_sizeof(a5),a1
	bsr.s	pp_scr
	lea	pp_variables+3*pv_sizeof(a5),a1
	bsr.s	pp_scr

	bset	#7,pp_tmpdmacon(a5)
	move.w	pp_tmpdmacon(a5),$96(a6)

	bsr	dma_wait

	move.l	pp_variables+pv_insaddress(a5),$a0(a6)
	move.w	pp_variables+pv_inslen(a5),$a4(a6)
	move.l	pp_variables+pv_sizeof+pv_insaddress(a5),$b0(a6)
	move.w	pp_variables+pv_sizeof+pv_inslen(a5),$b4(a6)
	move.l	pp_variables+2*pv_sizeof+pv_insaddress(a5),$c0(a6)
	move.w	pp_variables+2*pv_sizeof+pv_inslen(a5),$c4(a6)
	move.l	pp_variables+3*pv_sizeof+pv_insaddress(a5),$d0(a6)
	move.w	pp_variables+3*pv_sizeof+pv_inslen(a5),$d4(a6)

	rts

pp_scr:
	move.w	pp_tmpdmacon(a5),d0
	and.w	pv_dmacon(a1),d0
	beq.s	.l5

	move.l	pv_custbase(a1),a0
	move.l	pv_insaddress(a1),(a0)
	move.w	pv_inslen(a1),4(a0)
	move.w	pv_pers(a1),6(a0)
	btst	#0,pv_flags(a1)
	bne.s	.l5
	pea	pp_nullwave(a5)
	move.l	(a7)+,pv_insaddress(a1)
	move.w	#1,pv_inslen(a1)

.l5:
	rts

pp_nnt:
	clr.w	d1
	move.b	(a0),d1		; nt_period
	beq	.l5

	add.w	d1,d1
	lea	pp_periods-2(a5,d1.w),a2

	btst	#6,nt_speed(a0)
	beq.s	.l2
	move.w	(a2),pv_porlevel(a1)
	bra.s	.l5

.l2:
	move.w	pv_dmacon(a1),d0
	or.w	d0,pp_tmpdmacon(a5)

	move.l	a2,pv_peraddress(a1)
	move.w	(a2),pv_pers(a1)
	move.w	(a2),pv_pers+2(a1)
	move.w	(a2),pv_pers+4(a1)

	clr.w	pv_por(a1)

	move.b	nt_instr(a0),d0
	ext.w	d0
	mulu	#it_sizeof,d0
	add.l	instable(a5),d0
	move.l	d0,a2
	tst.l	it_address(a2)
	bne.s	.l1
	pea	pp_nullwave(a5)
	move.l	(a7)+,pv_insaddress(a1)
	move.w	#1,pv_inslen(a1)
	clr.b	pv_flags(a1)
	bra.s	.l5

.l1:
	move.l	it_address(a2),a3
	btst	#1,it_flags(a2)
	bne.s	.l0a
	move.l	it_size(a2),d0
	lsr.l	#1,d0
	move.w	d0,pv_inslen(a1)
	bra.s	.l0
.l0a:
	move.w	(a1),d0		;pv_waveoffset
	add.w	d0,a3
	move.w	#$20,pv_inslen(a1)
.l0:
	move.l	a3,pv_insaddress(a1)
	move.b	it_flags(a2),pv_flags(a1)
	move.w	pv_vollevel(a1),pv_vol(a1)

.l5:
	move.b	nt_speed(a0),d0
	and.b	#$0F,d0
	beq.s	.l6
	move.b	d0,pp_wait(a5)

.l6:
	move.l	pv_peraddress(a1),a2
	move.b	nt_arpeggio(a0),d0
	beq.s	.l9
	cmp.b	#$FF,d0
	bne.s	.l7
	move.w	(a2),pv_pers(a1)
	move.w	(a2),pv_pers+2(a1)
	move.w	(a2),pv_pers+4(a1)
	bra.s	.l9

.l7:
	and.b	#$0F,d0
	add.b	d0,d0
	ext.w	d0
	move.w	(a2,d0.w),pv_pers+4(a1)
	move.b	nt_arpeggio(a0),d0
	lsr.b	#4,d0
	add.b	d0,d0
	ext.w	d0
	move.w	(a2,d0.w),pv_pers+2(a1)
	move.w	(a2),pv_pers(a1)

.l9:
	move.b	nt_vibrato(a0),d0
	beq.s	.ld
	cmp.b	#$FF,d0
	bne.s	.la
	clr.l	pv_vib(a1)
	clr.b	pv_vibcnt(a1)
	bra.s	.ld
.la:
	clr.w	pv_vib(a1)
	and.b	#$0F,d0
	ext.w	d0
	move.w	d0,pv_deltavib(a1)
	move.b	nt_vibrato(a0),d0
	lsr.b	#4,d0
	move.b	d0,pv_vibmax(a1)
	lsr.b	#1,d0
	move.b	d0,pv_vibcnt(a1)

.ld:
	move.b	nt_phase(a0),d0
	beq.s	.l10
	cmp.b	#$FF,d0
	bne.s	.le
	clr.w	pv_phase(a1)
	move.w	#$FFFF,pv_deltaphase(a1)
	bra.s	.l10
.le:
	and.b	#$0F,d0
	ext.w	d0
	move.w	d0,pv_deltaphase(a1)
	clr.w	pv_phase(a1)

.l10:
	move.b	nt_volume(a0),d0
	bne.s	.l10a
	btst	#7,nt_speed(a0)
	beq.s	.l16
	bra.s	.l11a
.l10a:
	cmp.b	#$FF,d0
	bne.s	.l11
	clr.w	pv_deltavol(a1)
	bra.s	.l16
.l11:
	btst	#7,nt_speed(a0)
	beq.s	.l12
.l11a:
	move.b	d0,pv_vol+1(a1)
	move.b	d0,pv_vollevel+1(a1)
	clr.w	pv_deltavol(a1)
	bra.s	.l16
.l12:
	bclr	#7,d0
	beq.s	.l13
	neg.b	d0
.l13:
	ext.w	d0
	move.w	d0,pv_deltavol(a1)

.l16:
	move.b	nt_porta(a0),d0
	beq.s	.l1a
	cmp.b	#$FF,d0
	bne.s	.l17
	clr.l	pv_por(a1)
	bra.s	.l1a
.l17:
	clr.w	pv_por(a1)
	btst	#6,nt_speed(a0)
	beq.s	.l17a
	move.w	pv_porlevel(a1),d1
	cmp.w	pv_pers(a1),d1
	bgt.s	.l17c
	neg.b	d0
	bra.s	.l17c

.l17a:
	bclr	#7,d0
	bne.s	.l18
	neg.b	d0
	move.w	#135,pv_porlevel(a1)
	bra.s	.l17c
.l18:
	move.w	#1019,pv_porlevel(a1)
.l17c:
	ext.w	d0
.l18a:
	move.w	d0,pv_deltapor(a1)
.l1a:
base:	rts

;;;;;;;;;;;;;;;;;;;;
;;; Data section ;;;
;;;;;;;;;;;;;;;;;;;;


pp_periods:
	dc.w	1019,962,908,857,809,763,720,680,642,606,572,540
	dc.w	509,481,454,428,404,381,360,340,321,303,286,270
	dc.w	254,240,227,214,202,190,180,170,160,151,143,135
	dc.w	135,135,135,135,135,135,135,135,135
	dc.w	135,135,135,135,135,135

songlen:		ds.w	1
songtable:	ds.l	1
instable:		ds.l	1
patttable:	ds.l	1

pp_wait:		ds.b	1
pp_waitcnt:	ds.b	1
pp_notecnt:	ds.b	1
pp_address:	ds.l	1
pp_songptr:	ds.l	1
pp_songcnt:	ds.w	1
pp_pattentry:	ds.l	1
pp_tmpdmacon:	ds.w	1
pp_variables:	ds.b	4*48

pp_nullwave:	ds.w	1

_song:		ds.l	1

;;; Local Variables:
;;; mode: asm
;;; indent-tabs-mode: t
;;; tab-width: 10
;;; comment-column: 40
;;; End:
