List	P = HC18P134L, R = DEC

include	HC18P133L.inc

include	RAM_DEF.inc

	org	0000H
	goto	START
	org	0004H
	goto	IRQ

START:
	nop
	nop

	call	System_Init

	; call	F_ClearScreen
	; call	F_Temperature_Get
	; call	F_Display_Temper

	bsf		Beep_Flag,0
	bsf		TimeFlag_SW,1
	movlw	B'100'
	movwf	Beep_Serial

MAIN:
	call	F_BeepManage
	call	F_KeyHandler
	
	goto	MAIN



IRQ:
	movwf	W_BAK
	swapf	STATUS,W
	movwf	STATUS_BAK
	movf	PCLATH,W
	movwf	PCLATH_BAK

	btfsc	T0IF
	goto	T0_IRQ_Handler

	btfsc	T1IF
	goto	T1_IRQ_Handler

	btfsc	RAIF
	goto	PA_IRQ_Handler

	btfsc	RBIF
	goto	PB_IRQ_Handler

	btfsc	RFIF
	goto	PF_IRQ_Handler

IRQ_EXIT:
	movf	PCLATH_BAK,W
	movwf	PCLATH
	swapf	STATUS_BAK,W
	movwf	STATUS
	swapf	W_BAK,F
	swapf	W_BAK,W

	retfie

include	Init.inc
include	IRQHandler.inc
include KeyHandler.inc
include LedTable.inc
include Dis.inc
include Display.inc
include Temper.inc
include ADCTable.inc
include Beep.inc

end
