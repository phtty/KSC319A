F_BeepManage:
	bcf		RP0
	btfss	Beep_Flag,0
	return
	btfss	Time_Flag,3					; 必须有16Hz标志和开启蜂鸣标志才会进处理
	return
	bcf		Time_Flag,3

	movf	Beep_Serial,F
	btfsc	Z
	goto	L_NoBeep_Serial_Mode
	decf	Beep_Serial
	btfss	Beep_Serial,0
	goto	L_NoBeep_Serial_Mode
	bcf		TRISA,7						; PA7输出启用
	bsf		T2ON						; 开启TIM2
	return
L_NoBeep_Serial_Mode:
	bcf		T2ON						; 关闭TIM2
	bsf		TRISA,7
	return
