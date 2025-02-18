F_Display_Time:
	call	L_DisTime_Min
	call	L_DisTime_Hour
	return

; 显示分钟
L_DisTime_Min:
	movlw	led_d3
	movwf	R_X
	movf	R_Time_Min,W
	call	F_Hex2BCD
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	R_Time_Min,W
	call	F_Hex2BCD
	movf	P_Temp6,W
	call	F_7Digit_Dis
	return

; 根据12H或24H模式显示小时
L_DisTime_Hour:
	btfss	Clock_Flag,0
	goto	L_24hMode_Time

	movlw	12
	subwf	R_Time_Hour,W					; 判断当前时间是上午或下午
	btfsc	C
	goto	L_Time12h_PM
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 上午需灭PM点

	movf	R_Time_Hour,W					; 再判断是否在0点
	btfsc	Z
	goto	L_Time_0Hour
	goto	L_Start_DisTime_Hour
L_Time12h_PM:
	movlw	led_pm
	movwf	R_X
	call	F_DisSymbol						; 下午亮PM点
	movlw	12
	subwf	R_Time_Hour,W					; 调整为12h模式显示
	btfss	Z
	goto	L_Start_DisTime_Hour
L_Time_0Hour:
	movlw	12								; 0点在12h模式里是12点
	goto	L_Start_DisTime_Hour

L_24hMode_Time:
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 24h模式下没有PM点
	movf	R_Time_Hour,W
L_Start_DisTime_Hour:
	call	F_Hex2BCD						; 显示最终的结果
	movlw	led_d1
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movf	P_Temp6,W
	movwf	R_X
	call	F_6Digit_Dis
	return




; 显示闹钟
F_Display_Alarm:
	call	L_DisAlarm_Min
	call	L_DisAlarm_Hour
	return

L_DisAlarm_Min:
	movlw	led_d3
	movwf	R_X
	movf	R_Alarm_Min,W
	call	F_Hex2BCD
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	R_Alarm_Min,W
	call	F_Hex2BCD
	movf	P_Temp6,W
	call	F_7Digit_Dis
	return


L_DisAlarm_Hour:
	btfss	Clock_Flag,0
	goto	L_24hMode_Alarm

	movlw	12
	subwf	R_Alarm_Hour,W					; 判断当前时间是上午或下午
	btfsc	C
	goto	L_Alarm12h_PM
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 上午需灭PM点

	movf	R_Alarm_Hour,W					; 再判断是否在0点
	btfsc	Z
	goto	L_Alarm_0Hour
	goto	L_Start_DisAlarm_Hour
L_Alarm12h_PM:
	movlw	led_pm
	movwf	R_X
	call	F_DisSymbol						; 下午亮PM点
	movlw	12
	subwf	R_Alarm_Hour,W					; 调整为12h模式显示
	btfss	Z
	goto	L_Start_DisAlarm_Hour
L_Alarm_0Hour:
	movlw	12								; 0点在12h模式里是12点
	goto	L_Start_DisAlarm_Hour

L_24hMode_Alarm:
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 24h模式下没有PM点
	movf	R_Alarm_Hour,W
L_Start_DisAlarm_Hour:
	call	F_Hex2BCD						; 显示最终的结果
	movlw	led_d1
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movf	P_Temp6,W
	movwf	R_X
	call	F_6Digit_Dis
	return




F_Display_Date:
	call	L_DisDate_Day
	call	L_DisDate_Month
	return

L_DisDate_Day:
	movf	R_Date_Day,W
	call	F_Hex2BCD
	movlw	led_d3
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	P_Temp6,W
	btfss	Z
	goto	L_Day_Tens_NoZero
	movlw	10
L_Day_Tens_NoZero:
	call	F_7Digit_Dis
	return

L_DisDate_Month:
	movf	R_Date_Month,W
	call	F_Hex2BCD
	movlw	led_d1
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movf	P_Temp6,W
	movwf	R_X
	call	F_6Digit_Dis
	return


L_DisDate_Year:
	movf	R_Date_Year,W
	call	F_Hex2BCD
	movlw	led_d3
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	P_Temp6,W
	call	F_7Digit_Dis

	movlw	led_d1
	movwf	R_X
	movlw	0
	call	F_7Digit_Dis

	movlw	2
	movwf	R_X
	call	F_6Digit_Dis
	return




F_UnDisplay_D0_1:
	movlw	led_d1
	movwf	R_X
	movlw	10
	call	F_7Digit_Dis

	movlw	0
	movwf	R_X
	call	F_6Digit_Dis
	return




F_UnDisplay_D2_3:
	movlw	led_d3
	movwf	R_X
	movlw	10
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movlw	10
	call	F_7Digit_Dis
	return




F_Display_Temper:
	return




; W==要转化的值
; P_Temp1、2、3为个、十、百位
F_Hex2BCD:
	movwf	P_Temp
	clrf	P_Temp+5
	clrf	P_Temp+6
	clrf	P_Temp+7
LOOP_HUND:
	movlw	D'100'
	subwf	P_Temp,W					; W = BIN - 100
	btfss	C
	goto	LOOP_TEN					; 不够减，退出循环
	movwf	P_Temp						; 保存减法结果到BIN
	incf	P_Temp7,F					; 百位加1
	goto	LOOP_HUND
LOOP_TEN:								; 计算十位
	movlw	D'10'
	subwf	P_Temp,W					; W = BIN - 10
	btfss	C
	goto	END_TEN
	movwf	P_Temp						; 保存减法结果到BIN
	incf 	P_Temp6,F					; 十位加1
	goto 	LOOP_TEN
END_TEN:
	movf	P_Temp,W					; 剩余值即为个位
	movwf	P_Temp5
	return




L_LSR_4Bit:
	bcf		C
	rrf		P_Temp
	rrf		P_Temp
	rrf		P_Temp
	rrf		P_Temp
	return
