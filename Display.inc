F_Display_Time:
	bcf		RP0
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




; 显示日期
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




F_Display_NightPeriod_SW:
	call	F_ClrScreen_HoldAL					; 显示ON/OFF需要清除了al之外的所有点

	btfsc	NightPeriod_Flag,0
	goto	Display_NightPeriod_ON

	movlw	0xee								; 显示OFF
	iorwf	DRAM,F
	movlw	0x2c
	iorwf	DRAM+1,F
	movlw	0x24
	iorwf	DRAM+2,F
	movlw	0x70
	iorwf	DRAM+3,F
	return

Display_NightPeriod_ON:
	movlw	0xdd								; 显示ON
	iorwf	DRAM,F
	movlw	0x3d
	iorwf	DRAM+2,F
	return



; 显示夜间模式开始时间
F_Display_NMode_StartTime:
	bcf		RP0
	call	L_DisNStartTime_Min
	call	L_DisNStartTime_Hour
	return

; 显示分钟
L_DisNStartTime_Min:
	movlw	led_d3
	movwf	R_X
	movf	R_Night_StartMin,W
	call	F_Hex2BCD
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	R_Night_StartMin,W
	call	F_Hex2BCD
	movf	P_Temp6,W
	call	F_7Digit_Dis
	return

; 根据12H或24H模式显示小时
L_DisNStartTime_Hour:
	btfss	Clock_Flag,0
	goto	L_24hMode_NStartTime

	movlw	12
	subwf	R_Night_StartHour,W				; 判断当前时间是上午或下午
	btfsc	C
	goto	L_NStartTime12h_PM
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 上午需灭PM点

	movf	R_Night_StartHour,W				; 再判断是否在0点
	btfsc	Z
	goto	L_NStartTime_0Hour
	goto	L_Start_DisNStartTime_Hour
L_NStartTime12h_PM:
	movlw	led_pm
	movwf	R_X
	call	F_DisSymbol						; 下午亮PM点
	movlw	12
	subwf	R_Night_StartHour,W				; 调整为12h模式显示
	btfss	Z
	goto	L_Start_DisNStartTime_Hour
L_NStartTime_0Hour:
	movlw	12								; 0点在12h模式里是12点
	goto	L_Start_DisNStartTime_Hour

L_24hMode_NStartTime:
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 24h模式下没有PM点
	movf	R_Night_StartHour,W
L_Start_DisNStartTime_Hour:
	call	F_Hex2BCD						; 显示最终的结果
	movlw	led_d1
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movf	P_Temp6,W
	movwf	R_X
	call	F_6Digit_Dis
	return




; 显示夜间模式结束时间
F_Display_NMode_EndTime:
	bcf		RP0
	call	L_DisNEndTime_Min
	call	L_DisNEndTime_Hour
	return

; 显示分钟
L_DisNEndTime_Min:
	movlw	led_d3
	movwf	R_X
	movf	R_Night_EndMin,W
	call	F_Hex2BCD
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d2
	movwf	R_X
	movf	R_Night_EndMin,W
	call	F_Hex2BCD
	movf	P_Temp6,W
	call	F_7Digit_Dis
	return

; 根据12H或24H模式显示小时
L_DisNEndTime_Hour:
	btfss	Clock_Flag,0
	goto	L_24hMode_NEndTime

	movlw	12
	subwf	R_Night_EndHour,W				; 判断当前时间是上午或下午
	btfsc	C
	goto	L_NEndTime12h_PM
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 上午需灭PM点

	movf	R_Night_EndHour,W				; 再判断是否在0点
	btfsc	Z
	goto	L_NEndTime_0Hour
	goto	L_Start_DisNEndTime_Hour
L_NEndTime12h_PM:
	movlw	led_pm
	movwf	R_X
	call	F_DisSymbol						; 下午亮PM点
	movlw	12
	subwf	R_Night_EndHour,W				; 调整为12h模式显示
	btfss	Z
	goto	L_Start_DisNEndTime_Hour
L_NEndTime_0Hour:
	movlw	12								; 0点在12h模式里是12点
	goto	L_Start_DisNEndTime_Hour

L_24hMode_NEndTime:
	movlw	led_pm
	movwf	R_X
	call	F_ClrSymbol						; 24h模式下没有PM点
	movf	R_Night_StartHour,W
L_Start_DisNEndTime_Hour:
	call	F_Hex2BCD						; 显示最终的结果
	movlw	led_d1
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movf	P_Temp6,W
	movwf	R_X
	call	F_6Digit_Dis
	return


; 显示贪睡时间
F_Display_SNZ:
	movlw	led_d2
	movwf	R_X
	movf	R_Snooze_Time,W
	call	F_Hex2BCD
	movf	P_Temp5,W
	call	F_7Digit_Dis

	movlw	led_d1
	movwf	R_X
	movf	R_Snooze_Time,W
	call	F_Hex2BCD
	movf	P_Temp6,W
	call	F_7Digit_Dis
	return


; 显示工作日闹钟
F_AlarmWorkDay_Dis:
	call	F_ClrScreen_HoldAL
	movlw	0x20
	iorwf	DRAM,F
	movlw	0x28
	iorwf	DRAM+1,F						; 显示固定的“1-”

	movlw	led_d3
	movwf	R_X
	btfsc	WorkDay_Alarm,0
	movlw	7
	btfsc	WorkDay_Alarm,1
	movlw	6
	btfsc	WorkDay_Alarm,2
	movlw	5
	call	F_7Digit_Dis					; 根据工作日闹钟的设置值显示7~5
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


; 除了AL点全清屏
F_ClrScreen_HoldAL:
	call	F_ClearScreen					; 先全清屏
	btfss	Clock_Flag,7
	return

	movlw	led_al							; 再根据闹钟是否开启决定亮不亮AL点
	movwf	R_X
	call	F_DisSymbol
	return




; 显示和清除秒点
F_DisCol:
	movlw	led_col
	movwf	R_X
	call	F_DisSymbol
	return

F_ClrCol
	movlw	led_col
	movwf	R_X
	call	F_ClrSymbol
	return




F_SymbolRegulate:
	return




F_Display_Temper:
	call	F_ClearScreen				; 先清屏

	btfss	ADC_Flag,1
	goto	Display_CelsiusDegree
	goto	Display_FahrenheitDegree

Display_CelsiusDegree:
	movlw	led_c
	movwf	R_X
	call	F_DisSymbol					; 显示摄氏度符号

	movf	R_Temperature,W
	call	F_Hex2BCD
	movlw	led_d2
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis				; 显示温度个位

	movlw	led_d1
	movwf	R_X
	movf	P_Temp6,W
	call	F_7Digit_Dis				; 显示温度十位

	btfss	ADC_Flag,0					; 若有负号则显示负号
	return
	movlw	3
	movwf	R_X
	call	F_6Digit_Dis				; D0显示负号
	return

Display_FahrenheitDegree:
	movlw	led_f
	movwf	R_X
	call	F_DisSymbol					; 显示华氏度符号

	movf	R_Temperature_F,W
	call	F_Hex2BCD
	movlw	led_d2
	movwf	R_X
	movf	P_Temp5,W
	call	F_7Digit_Dis				; 显示温度个位

	movlw	led_d1
	movwf	R_X
	movf	P_Temp6,W
	call	F_7Digit_Dis				; 显示温度十位

	movf	P_Temp7,W
	movwf	R_X
	call	F_6Digit_Dis				; 显示温度百位
	return




L_Dis_xxHr:
	call	F_UnDisplay_D2_3
	movlw	0xb2
	iorwf	DRAM,F
	movlw	0x2c
	iorwf	DRAM2,F						; 显示Hr

	btfsc	Clock_Flag,0				; 根据当前是24h模式还是12h模式显示数字
	goto	L_24hMode_Set
	movlw	led_d0
	movwf	R_X
	movlw	1
	call	F_7Digit_Dis
	movlw	led_d0
	movwf	R_X
	movlw	2
	call	F_7Digit_Dis
L_24hMode_Set:
	movlw	led_d0
	movwf	R_X
	movlw	2
	call	F_7Digit_Dis
	movlw	led_d0
	movwf	R_X
	movlw	4
	call	F_7Digit_Dis
	return




; W==要转化的值
; P_Temp5、6、7为个、十、百位
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
