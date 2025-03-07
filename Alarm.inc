F_Alarm_Display:
	bcf		RP0
	call	F_DisCol
	call	F_Display_Alarm

	return




; 闹钟设置模式
F_Alarm_Set:
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
	goto	F_Display_SNZ						; 贪睡时间
	movlw	3
	subwf	Secondary_Status,W
	btfsc	Z
	goto	F_AlarmWorkDay_Dis					; 工作日闹钟
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
	goto	L_Alarm_Process

	btfsc	Beep_Flag,1							; 如果有按键提示音，则不关闭蜂鸣器
	goto	L_LoudingNoClose

	bcf		Time_Flag,3
	bcf		Time_Flag,6
	bcf		TimeFlag_SW,1
	call	L_NoBeep_Serial_Mode
L_LoudingNoClose:
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

L_AlarmTrigger:									; 闹钟被触发时的处理
	bsf		Time_Flag,6
	bsf		TimeFlag_SW,1						; 开启蜂鸣器定时器
	bsf		Clock_Flag,2						; 置位响闹标志位
	bcf		Clock_Flag,1						; 关闭闹钟触发标志位，避免重复触发闹钟

L_Alarm_NoStop:									; 响闹时进闹钟判断执行的操作
	btfss	Clock_Flag,5
	bsf		Clock_Flag,5						; 保存这次的响闹模式，用于贪睡闹钟判断
	return

L_Snooze_CloseLoud:								; 贪睡导致的响闹结束
	btfss	Clock_Flag,5
	return										; last==1 && now==0

	bcf		Clock_Flag,5						; 响闹结束时同步响闹标志位到Backup
	btfss	Clock_Flag,6						; 无贪睡键 && 处于贪睡模式 && 响闹结束
	goto	L_NoSnooze_CloseLoud				; 才会结束贪睡并关闭响闹

	bcf		Clock_Flag,6						; 清除贪睡触发标志
	goto	L_CloseLoud
L_NoSnooze_CloseLoud:
	bcf		Clock_Flag,3
	bcf		Clock_Flag,6
L_CloseLoud:
	clrf	AlarmLoud_Counter
	movlw	B'11011001'							; Clock_Flag 1、2、5 Bit置0
	andwf	Clock_Flag,F						; 关闭闹钟触发标志，关闭响闹模式

	btfsc	Key_Flag,4							; 如果有按键提示音，则不关闭蜂鸣器
	return

	bcf		TimeFlag_SW,1
	call	L_NoBeep_Serial_Mode
	
	bcf		Time_Flag,3
	bcf		Time_Flag,6
	return




L_Alarm_Process:
	bcf		RP0
	btfss	Time_Flag,6
	return

	bcf		Time_Flag,6
	movlw	60
	subwf	AlarmLoud_Counter,W
	btfsc	Z
	goto	L_NoSnooze_CloseLoud				; 响铃60S后关闭

	movlw	8
	movwf	Beep_Serial							; 重装载响铃序列
	incf	AlarmLoud_Counter					; 增加响铃计时
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
