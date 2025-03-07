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

	call	System_Init						; 上电初始化

	call	L_GetWeek

	movlw	1
	movwf	Primary_Status					; 设置系统状态
	clrf	Secondary_Status

MAIN:

Global_Run:									; 全局生效的功能
	call	F_KeyHandler
	call	F_BeepManage
	call	F_Time_Run						; 走时

Status_Juge:
	btfsc	Primary_Status,0
	goto	Status_DisClock
	btfsc	Primary_Status,1
	goto	Status_DisAlarm
	btfsc	Primary_Status,2
	goto	Status_SetClock
	btfsc	Primary_Status,3
	goto	Status_SetAlarm
	goto	MAIN

Status_DisClock:
	call	F_Clock_Display

	goto	MAIN

Status_DisAlarm:
	
	goto	MAIN

Status_SetClock:
	goto	MAIN

Status_SetAlarm:
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
include VoiceSwitch.inc
include Time.inc
include Calendar.inc
include Alarm.inc

end
