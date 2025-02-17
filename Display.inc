F_Display_Time:
	call	L_DisTime_Min
	call	L_DisTime_Hour
	return

L_DisTime_Min:
	movf	R_Time_Min,W
	daw
	movwf	P_Temp
	movlw	led_d3
	movwf	R_X
	movlw	0x0f
	andwf	P_Temp,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movlw	0xf0
	andwf	P_Temp,W
	call	L_LSR_4Bit
	call	F_7Digit_Dis

	return

L_DisTime_Hour:
	btfss	Clock_Flag,0
	goto	L_24hMode_Time

	movlw	12
	subwf	R_Time_Hour,W
	btfsc	C
	goto	L_Time12h_PM
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol
	movf	R_Time_Hour,W
	btfsc	Z
	goto	L_Time_0Hour
	goto	L_Start_DisTime_Hour
L_Time12h_PM:
	movlw	led_pm
	movwf	R_X
	call	F_DisSymbol
	movlw	12
	subwf	R_Time_Hour,W
	btfss	Z
	goto	L_Start_DisTime_Hour
L_Time_0Hour:
	movlw	12
	goto	L_Start_DisTime_Hour

L_24hMode_Time:
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol
	movf	R_Time_Hour,W
L_Start_DisTime_Hour:
	daw
	movwf	P_Temp
	movlw	led_d1
	movwf	R_X
	movlw	0x0f
	andwf	P_Temp,W
	call	F_7Digit_Dis

	movlw	0xf0
	andwf	P_Temp,W
	call	L_LSR_4Bit
	call	F_6Digit_Dis
	return




F_Display_Alarm:
	return




F_Display_Date:
	return




F_UnDisplay_D0_1:
	return




F_Display_Temper:
	return

L_LSR_4Bit:
	bcf		C
	rrf		P_Temp
	rrf		P_Temp
	rrf		P_Temp
	rrf		P_Temp
	return
