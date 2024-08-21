;;; sc68 wrapper for TSD TFMX 2
;;;
;;; by Benjamin Gerard
;;;
;;; Time-stamp: <2023-09-13 19:15:36 ben>
;;;
	bra	init
	bra	rep+4
	bra	rep+8

init:
	lea	rep+$54(pc),a1
	move.l	#$4e714e71,(a1)
rep:	incbin	"org/tsd1"
	even
	dc.w	0
