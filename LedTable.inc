Led_byte:
led_d0	equ	$-Led_byte
	db	1				; 0a
	db	1				; 0b
	db	3				; 0c
	db	3				; 0d
	db	3				; 0e
	db	3				; 0g
led_d1	equ	$-Led_byte
	db	1				; 1a
	db	1				; 1b
	db	3				; 1c
	db	3				; 1d
	db	3				; 1e
	db	1				; 1f
	db	1				; 1g
led_d2	equ	$-Led_byte
	db	0				; 2a
	db	0				; 2b
	db	2				; 2c
	db	2				; 2d
	db	2				; 2e
	db	0				; 2f
	db	0				; 2g
led_d3	equ	$-Led_byte
	db	0				; 3a
	db	0				; 3b
	db	2				; 3c
	db	2				; 3d
	db	2				; 3e
	db	0				; 3f
	db	0				; 3g
led_col	equ	$-Led_byte
	db	2				; COL
led_al	equ	$-Led_byte
	db	1				; AL
led_pm	equ	$-Led_byte
	db	1				; PM
led_c	equ	$-Led_byte
	db	3				; C
led_f	equ	$-Led_byte
	db	2				; F


Led_bit:
	db	0x1				; 0a
	db	0x2				; 0b
	db	0x8				; 0c
	db	0x4				; 0d
	db	0x1				; 0e
	db	0x2				; 0g

	db	0x8				; 1a
	db	0x20			; 1b
	db	0x40			; 1c
	db	0x20			; 1d
	db	0x10			; 1e
	db	0x4				; 1f
	db	0x10			; 1g

	db	0x40			; 2a
	db	0x10			; 2b
	db	0x8				; 2c
	db	0x10			; 2d
	db	0x20			; 2e
	db	0x80			; 2f
	db	0x20			; 2g

	db	0x4				; 3a
	db	0x1				; 3b
	db	0x1				; 3c
	db	0x2				; 3d
	db	0x4				; 3e
	db	0x8				; 3f
	db	0x2				; 3g

	db	0x80			; COL
	db	0x40			; AL
	db	0x80			; PM
	db	0x80			; C
	db	0x40			; F
