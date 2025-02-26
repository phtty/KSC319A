F_Alarm_Display:
	bcf		RP0
	call	F_DisCol
	call	F_Display_Alarm

	return




; 闹钟设置模式
F_Clock_Set:
	bcf		RP0

	movf	Secondary_Status,W
	btfsc	Z
	goto	F_AlarmHour_Set						; 闹钟时设
	movlw	1
	subwf	Secondary_Status,W
	btfsc	Z
	goto	F_AlarmMin_Set						; 小时设置
	movlw	2
	subwf	Secondary_Status,W
	btfsc	Z
	goto	F_AlarmSnz_Set						; 贪睡时间
	movlw	3
	subwf	Secondary_Status,W
	btfsc	Z
	goto	F_AlarmWorkDay_Set					; 工作日闹钟
	return


F_AlarmHour_Set:
	bcf		RP0
	btfsc	Key_Flag,3
	goto	L_KeyTrigger_NoBlink_AlarmHour		; 存在快加时常亮
	btfss	Time_Flag,1
	return

	bcf		Time_Flag,1
	call	F_DisCol
	btfsc	Time_Flag,0
	goto	L_AlarmHour_Clear
L_KeyTrigger_NoBlink_AlarmHour:
	call	F_Display_Alarm						; 半S亮
	return
L_AlarmHour_Clear:
	bcf		Time_Flag,0
	call	F_UnDisplay_D0_1					; 1S灭
	return


F_AlarmMin_Set:
	bcf		RP0
	btfsc	Key_Flag,3
	goto	L_KeyTrigger_NoBlink_AlarmMin		; 存在快加时常亮
	btfss	Time_Flag,1
	return

	bcf		Time_Flag,1
	call	F_DisCol
	btfsc	Time_Flag,0
	goto	L_AlarmMin_Clear
L_KeyTrigger_NoBlink_AlarmMin:
	call	F_Display_Alarm						; 半S亮
	return
L_AlarmMin_Clear:
	bcf		Time_Flag,0
	call	F_UnDisplay_D2_3					; 1S灭
	return




; 闹钟处理
F_Alarm_Handler:
	call	Is_AlarmTrigger						; 判断闹钟是否触发
	btfsc	Clock_Flag,2						; 存在响闹标志位才会进处理
	call	L_Alarm_Process

	btfss	Beep_Flag,1							; 如果有按键提示音，则不关闭蜂鸣器
	call	L_NoBeep_Serial_Mode
	clrf	AlarmLoud_Counter
	return


Is_AlarmTrigger:
	btfsc	Clock_Flag,2
	goto	L_Alarm_NoStop						; 如此时仍在响闹，则直接进入响闹持续部分
	
	call	AlarmTrigger_Juge
	btfss	Clock_Flag,1
	goto	Is_Snooze							; 闹钟触发标志有了才会走响闹流程，否则进贪睡流程

	movf	R_Alarm_Hour,W						; 闹钟触发时，同步一次闹钟到贪睡闹钟
	movwf	R_Snooze_Hour						; 贪睡触发时，需要在这个贪睡时钟基础上加上贪睡时间
	movf	R_Alarm_Min,W
	movwf	R_Snooze_Min
	goto	L_AlarmTrigger
Is_Snooze:
	btfss	Clock_Flag,3						; 先判断闹钟是否触发，再判断是否存在贪睡
	return										; 如既无闹钟触发，又无贪睡，则不需要闹钟处理，直接退出

	movf	R_Time_Hour,W						; 贪睡模式下,用贪睡闹钟代替闹钟判断是否和当前时钟匹配
	subwf	R_Snooze_Hour,W
	btfsc	Z
	goto	L_Snooze_CloseLoud
	return




AlarmTrigger_Juge:
	movf	R_Time_Hour,W
	subwf	R_Alarm_Hour,W						; 判断小时是否匹配闹钟
	btfss	Z
	goto	Alarm_NoMatch

	movf	R_Time_Min,W
	subwf	R_Alarm_Min,W						; 判断分钟是否匹配闹钟
	btfss	Z
	goto	Alarm_NoMatch

	movf	R_Time_Sec,F						; 判断当前秒是否为0
	btfss	Z
	goto	Alarm_NoMatch

	call	L_NoSnooze_CloseLoud				; 重置贪睡状态
	btfsc	Clock_Flag,4
	goto	Alarm_Blocked						; 判断有闹钟触发阻塞标志，防止闹钟没到1s被关闭，还会重复触发闹钟
	bsf		Clock_Flag,1						; 置位闹钟触发
Alarm_Blocked:
	bsf		Clock_Flag,4
	return

; 闹钟不匹配则退出
Alarm_NoMatch:
	bcf		Clock_Flag,1
	return
