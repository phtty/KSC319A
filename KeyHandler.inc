; 按键处理
F_KeyHandler:
	btfsc	Key_Flag,3
	goto	L_Key4Hz					; 存在快加则4Hz进一次按键功能
	btfss	Key_Flag,2
	goto	L_KeyScan					; 非首次触发直接跳到长按等待
	bcf		Key_Flag,2					; 清首次触发

	call	F_KeyDelay					; 延时消抖

	movlw	PORTA
	andlw	0x7c
	btfsc	Z
	goto	L_KeyExit					; 消抖后无按键则退出

	bsf		TimeFlag_SW,2
	movwf	PORTA_BackUp				; 更新按键IO历史状态
	goto	L_KeyHandle

L_Key4Hz:
	btfss	Time_Flag,2
	goto	L_KeyExit
	bcf		Time_Flag,2					; 清4Hz标志

L_KeyScan:
	btfsc	Key_Flag,1
	goto	F_SpecialKey_Handle			; 检测到松键，进特殊按键处理

	btfsc	Key_Flag,3
	goto	L_QuikAdd

	btfsc	PORTA,4
	bsf		Key_Flag,5

	btfss	Time_Flag,4					; 长按1S计时
	goto	L_KeyExit

	bcf		Time_Flag,4					; 清除32Hz标志
	incf	QuickAdd_Counter,F
	btfsc	Key_Flag,5
	goto	L_SNZ_LongPress
	movlw	64							; 非SNZ键长按计时2S
	goto	LongPress
L_SNZ_LongPress:
	movlw	96							; SNZ键长按计时3S
LongPress:
	subwf	QuickAdd_Counter,W
	btfss	C
	goto	L_KeyExit					; 未到达长按计数

	bsf		Key_Flag,3					; 设置快加标志
	bcf		TimeFlag_SW,2				; 关闭32Hz计时，开启4Hz计时
	bsf		TimeFlag_SW,0
	clrf	QuickAdd_Counter			; 清长按计数

L_QuikAdd:
	btfss	Key_Flag,3
	clrf	SpecialKey_Flag				; 长按触发，则不进任何短按处理
	call	L_Key_Beep


L_KeyHandle:
	btfsc	PORTA,2
	goto	L_KeyMTrigger
	btfsc	PORTA,3
	goto	L_KeyATrigger
	btfsc	PORTA,4
	goto	L_KeyBTrigger
	btfsc	PORTA,5
	goto	L_KeyUTrigger
	btfsc	PORTA,6
	goto	L_KeyDTrigger

L_KeyHandle_Over:
	clrf	TimeFlag_SW
	clrf	Key_Flag
	clrf	SpecialKey_Flag
	bcf		Time_Flag,2					; 清4Hz标志
	bcf		Time_Flag,4					; 清32Hz标志

L_KeyExit:
	return


; 特殊功能按键处理
F_SpecialKey_Handle:
	movf	SpecialKey_Flag,F
	btfsc	Z
	goto	L_KeyHandle_Over

	btfss	Key_Flag,3
	; call	L_Key_Beep

	btfsc	SpecialKey_Flag,0
	goto	L_KeyM_ShortHandle
	btfsc	SpecialKey_Flag,1
	goto	L_KeyA_ShortHandle
	btfsc	SpecialKey_Flag,2
	goto	L_KeyB_ShortHandle
	btfsc	SpecialKey_Flag,3
	goto	L_KeyU_ShortHandle
	btfsc	SpecialKey_Flag,4
	goto	L_KeyD_ShortHandle

L_KeyM_ShortHandle:
	btfsc	Primary_Status,0
	goto	SwitchState_ClockDis		; 时钟显示的子模式切换
	goto	L_KeyHandle_Over

L_KeyA_ShortHandle:
	btfsc	Primary_Status,1
	goto	SwitchState_AlarmSet		; 闹钟显示的子模式切换
	goto	L_KeyHandle_Over

L_KeyB_ShortHandle:
	nop									; 其实B键并没有松键短按功能，只是留个接口
	goto	L_KeyHandle_Over

L_KeyU_ShortHandle:
	nop									; 同上
	goto	L_KeyHandle_Over

L_KeyD_ShortHandle:
	nop									; 同上
	goto	L_KeyHandle_Over




; 按键触发函数，处理每个按键触发后的响应条件
L_KeyMTrigger:
	call	L_Universal_TriggerHandle	; 如果是熄屏后唤醒，则是无按键功能
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7					; 判断是否需要直接退出整个按键处理
	goto	L_KeyHandle_Over

	movlw	B'00001010'
	andwf	Primary_Status,W
	btfss	Z
	goto	SwitchState_ClockDis		; 若在闹钟相关模式，M键会直接回到时钟显示

	btfsc	Key_Flag,3
	goto	L_KeyMTrigger_LongTri		; 长按标志到来

	btfss	Primary_Status,0			; 处于时钟显示状态，M键才会进行松键处理
	goto	Depress_Event

	goto	SetMode_CHG					; 根据当前模式去切换不同的子模式

Depress_Event:
	bsf		SpecialKey_Flag,0
	return								; 短按在按键触发部分只置标志位就退出

L_KeyMTrigger_LongTri:
	btfsc	Primary_Status,0
	goto	CD_SetMode_CHG				; 时钟显示模式下根据子模式切换到对应的设置

	goto	L_KeyHandle_Over			; 非时钟显示模式直接退出


L_KeyATrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over

	movlw	B'00001010'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SwitchState_AlarmDis		; 非闹钟设置或显示模式，A键会直接回到时钟显示

	btfsc	Key_Flag,3
	goto	L_KeyATrigger_LongTri
	bsf		SpecialKey_Flag,1			; 长按标志到来前置位标志位，交给短按函数处理
	return								; 短按在按键触发部分只置标志位就退出

L_KeyATrigger_LongTri:					; 长按部分处理
	btfsc	Primary_Status,1
	nop									; 切换到闹钟设置
	btfsc	Primary_Status,3
	nop									; 根据子模式进不同的长按处理或者进模式切换
	return
	goto	L_KeyHandle_Over


L_KeyBTrigger:
	call	L_Universal_TriggerHandle
	bsf		SpecialKey_Flag,2			; B键等待松键事件
	nop									; 启动贪睡功能
	goto	L_KeyHandle_Over


L_KeyUTrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over

	btfsc	Primary_Status,0
	nop									; 时钟显示模式下，U键调亮度

	btfss	Key_Flag,3
	goto	L_KeyUTrigger_LongTri:
	bsf		SpecialKey_Flag,0			; 长按标志到来前置位标志位，交给短按函数处理
	return

L_KeyUTrigger_LongTri:
	btfsc	Primary_Status,2
	goto	AddNum_CS
	btfsc	Primary_Status,3
	goto	AddNum_AS
	btfsc	Primary_Status,4
	goto	AddNum_DS
	btfsc	Primary_Status,5
	goto	AddNum_NS
	return
	goto	L_KeyHandle_Over


L_KeyDTrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over
	
	btfsc	Primary_Status,0
	nop									; 时钟显示模式下，U键调亮度

	btfss	Key_Flag,3
	goto	L_KeyDTrigger_LongTri:
	bsf		SpecialKey_Flag,0			; 长按标志到来前置位标志位，交给短按函数处理
	return

L_KeyDTrigger_LongTri:
	btfsc	Primary_Status,2
	goto	SubNum_CS
	btfsc	Primary_Status,3
	goto	SubNum_AS
	btfsc	Primary_Status,4
	goto	SubNum_DS
	btfsc	Primary_Status,5
	goto	SubNum_NS
	return
	goto	L_KeyHandle_Over



L_Universal_TriggerHandle:
	nop									; 返回计时
	nop									; 亮屏唤醒判断
	return


L_Key_NoSnoozeLoud:
	movlw	B'00001100'
	andwf	Clock_Flag,W
	bcf		Key_Flag,7
	btfsc	Z
	return
	bsf		Key_Flag,7
	call	L_NoSnooze_CloseLoud		; 打断贪睡和响闹
	return




;======================================================================================

; 切换时钟显示
SwitchState_ClockDis:
	movlw	B'00000001'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2CD						; 若当前非时钟显示则只切换到时钟显示

	incf	Secondary_Status			; 时钟显示模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	C
L_CLR_SS:
	clrf	Secondary_Status			; 超过最后一个子模式序号则清零回到第一个
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

SW2CD:
	movlw	B'00000001'
	movwf	Primary_Status
	goto	L_CLR_SS




; 切换到闹钟显示模式
SwitchState_AlarmDis:
	movlw	B'00000010'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2AD						; 若当前非闹钟显示则只切换到闹钟显示

	movlw	B'10000000'
	xorwf	Clock_Flag,F				; 开启或关闭闹钟
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

SW2AD:
	movlw	B'00000010'
	movwf	Primary_Status
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理




; 切换到时钟设置模式
SwitchState_ClockSet:
	movlw	B'00000100'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2CS						; 若当前非时钟设置则只切换到时钟设置
	
	incf	Secondary_Status			; 时钟设置模式的子模式切换
	movlw	3
	subwf	Secondary_Status,W
	btfsc	C
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

	goto	SwitchState_ClockDis		; 子模式溢出后退出设置模式，回到时显模式

SW2CS:
	movlw	B'00000100'
	movwf	Primary_Status
	clrf	Secondary_Status
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理




; 切换到闹钟设置模式
SwitchState_AlarmSet:
	movlw	B'00001000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2AS						; 若当前非闹钟设置则只切换到闹钟设置
	
	incf	Secondary_Status			; 闹钟设置模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	C
	goto	SwitchState_AlarmDis		; 闹钟设置子模式溢出后会回到闹钟显示模式

	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

SW2AS:
	movlw	B'00001000'
	movwf	Primary_Status
	clrf	Secondary_Status
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理





; 切换到日期设置模式
SwitchState_DateSet:
	movlw	B'00010000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2DS						; 若当前非日期设置则只切换到日期设置
	
	incf	Secondary_Status			; 日期设置模式的子模式切换
	movlw	3
	subwf	Secondary_Status,W
	btfsc	C
	goto	SwitchState_ClockDis		; 日期设置子模式溢出后会回到时钟显示

	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

SW2DS:
	movlw	B'00010000'
	movwf	Primary_Status
	clrf	Secondary_Status
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理




; 切换到夜间时段设置模式
SwitchState_NightPeriodSet:
	movlw	B'00100000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2NS						; 若当前非夜间时段设置则只切换到夜间时段设置

	incf	Secondary_Status			; 夜间时段设置模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	C
	goto	SwitchState_ClockDis		; 夜间时段设置子模式溢出后会回到时钟显示

	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理

SW2NS:
	movlw	B'00100000'
	movwf	Primary_Status
	clrf	Secondary_Status
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理




;======================================================================================

; 根据时显的子模式选择切换至对应的设置模式
CD_SetMode_CHG:
	movlw	High(CD_Secondary_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
CD_Secondary_JumpTable:
	addwf	PCL,F
	goto	SwitchState_ClockSet		; 子模式为0，进时间设置
	goto	SwitchState_DateSet			; 子模式为1，进日期设置
	goto	L_KeyHandle_Over			; 子模式为2，没有状态切换直接退出按键处理
	goto	SwitchState_NightPeriodSet	; 子模式为3，进夜间时段设置
	goto	L_KeyHandle_Over			; 非法子模式值，退出按键处理


; 根据当前设置模式的状态选择切换哪个设置模式的子模式
SetMode_CHG:
	btfsc	Primary_Status,2
	goto	SwitchState_ClockSet		; 切换时间设置下的子模式
	btfsc	Primary_Status,4
	goto	SwitchState_DateSet			; 切换日期设置下的子模式
	btfsc	Primary_Status,5
	goto	SwitchState_NightPeriodSet	; 切换夜间时段设置下的子模式
	goto	L_KeyHandle_Over			; 非法值，退出按键处理


; 时设模式增数
AddNum_CS:
	movlw	High(CS_AddNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
CS_AddNumMode_JumpTable:
	addwf	PCL,F
	goto	Time_Mode_SW
	goto	Time_Hour_Add
	goto	Time_Min_Add
	goto	L_KeyHandle_Over

; 闹设模式增数
AddNum_AS:
	movlw	High(AS_AddNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
AS_AddNumMode_JumpTable:
	addwf	PCL,F
	goto	Alarm_Hour_Add				; 闹钟小时设置
	goto	Alarm_Min_Add				; 闹钟分钟设置
	goto	Alarm_SNZ_Time_Add			; 闹钟贪睡时间设置
	goto	Alarm_WorkClock_Add			; 工作日闹钟设置
	goto	L_KeyHandle_Over

; 日设模式增数
AddNum_DS:
	movlw	High(DS_AddNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
DS_AddNumMode_JumpTable:
	addwf	PCL,F
	goto	Date_Year_Add				; 年份设置
	goto	Date_Month_Add				; 月份设置
	goto	Date_Day_Add				; 日期设置
	goto	L_KeyHandle_Over

; 夜设模式增数
AddNum_NS:
	movlw	High(NS_AddNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
NS_AddNumMode_JumpTable:
	addwf	PCL,F
	goto	NMode_Start_Hour_Add		; 夜间模式开始小时
	goto	NMode_Start_Min_Add			; 夜间模式开始分钟
	goto	NMode_End_Hour_Add			; 夜间模式结束小时
	goto	NMode_End_Min_Add			; 夜间模式结束分钟
	goto	L_KeyHandle_Over


; 时设模式减数
SubNum_CS:
	movlw	High(CS_SubNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
CS_SubNumMode_JumpTable:
	addwf	PCL,F
	goto	Time_Mode_SW
	goto	Time_Hour_Sub
	goto	Time_Min_Sub
	goto	L_KeyHandle_Over

; 闹设模式减数
SubNum_AS:
	movlw	High(AS_SubNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
AS_SubNumMode_JumpTable:
	addwf	PCL,F
	goto	Alarm_Hour_Sub				; 闹钟小时设置
	goto	Alarm_Min_Sub				; 闹钟分钟设置
	goto	Alarm_SNZ_Time_Sub			; 闹钟贪睡时间设置
	goto	Alarm_WorkClock_Sub			; 工作日闹钟设置
	goto	L_KeyHandle_Over

; 日设模式减数
SubNum_DS:
	movlw	High(DS_SubNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
DS_SubNumMode_JumpTable:
	addwf	PCL,F
	goto	Date_Year_Sub				; 年份设置
	goto	Date_Month_Sub				; 月份设置
	goto	Date_Day_Sub				; 日期设置
	goto	L_KeyHandle_Over

; 夜设模式减数
SubNum_NS:
	movlw	High(NS_SubNumMode_JumpTable)
	movwf	PCLATH
	movf	Secondary_Status,W
NS_SubNumMode_JumpTable:
	addwf	PCL,F
	goto	NMode_Start_Hour_Sub		; 夜间模式开始小时
	goto	NMode_Start_Min_Sub			; 夜间模式开始分钟
	goto	NMode_End_Hour_Sub			; 夜间模式结束小时
	goto	NMode_End_Min_Sub			; 夜间模式结束分钟
	goto	L_KeyHandle_Over

;======================================================================================

; 小时制切换
Time_Mode_SW:
	movlw	1
	xorwf	Clock_Flag,F				; 取反12/24h标志位

	call	F_Display_Time				; 刷新显示的时间
	goto	L_KeyHandle_Over			; 切换时间显示模式没有快加

; 时增加
Time_Hour_Add:
	movlw	23
	subwf	R_Time_Hour,W
	btfsc	C
	goto	TimeHour_AddOverflow
	incf	R_Time_Hour
	goto	TimeHour_Add_Exit:
TimeHour_AddOverflow:
	clrf	R_Time_Hour
TimeHour_Add_Exit:
	nop									; 夜间模式的处理
	call	F_Display_Time
	return

; 时减少
Time_Hour_Sub:
	movf	R_Time_Hour,F
	btfsc	Z
	goto	TimeHour_SubOverflow
	decf	R_Time_Hour,F
	goto	TimeHour_Sub_Exit
TimeHour_SubOverflow:
	movlw	23
	movwf	R_Time_Hour
TimeHour_Sub_Exit:
	nop									; 夜间模式的处理
	call	F_Display_Time
	return

; 分增加
Time_Min_Add:
	clrf	R_Time_Sec					; 调分钟会清空秒
	movlw	59
	subwf	R_Time_Min,W
	btfsc	C
	goto	TimeMin_AddOverflow
	incf	R_Time_Min,F
	goto	TimeMin_Add_Exit:
TimeMin_AddOverflow:
	clrf	R_Time_Min
TimeMin_Add_Exit:
	call	F_Display_Time
	return

; 分减少
Time_Min_Sub:
	clrf	R_Time_Sec					; 调分钟会清空秒
	movf	R_Time_Min,F
	btfsc	Z
	goto	TimeMin_SubOverflow
	decf	R_Time_Min,F
	goto	TimeMin_Sub_Exit
TimeMin_SubOverflow:
	movlw	59
	movwf	R_Time_Min
TimeMin_Sub_Exit:
	call	F_Display_Time
	return


;======================================================================================

; 年增加
Date_Year_Add:
	movlw	99
	subwf	R_Date_Year,W
	btfss	C
	goto	DateYear_AddOverflow
	incf	R_Date_Year
	goto	DateYear_Add_Exit
DateYear_AddOverflow:
	clrf	R_Date_Year
DateYear_Add_Exit:
	call	L_DayOverflow_Juge
	call	F_Is_Leap_Year
	call	L_DisDate_Year
	return

; 年减少
Date_Year_Sub:
	movf	R_Date_Year,F
	btfsc	Z
	goto	DateYear_SubOverflow
	decf	R_Date_Year,F
	goto	DateYear_Sub_Exit
DateYear_SubOverflow:
	movlw	99
	movwf	R_Date_Year
DateYear_Sub_Exit:
	call	L_DayOverflow_Juge
	call	F_Is_Leap_Year
	call	L_DisDate_Year
	return

; 月增加
Date_Month_Add:
	movlw	12
	subwf	R_Date_Month,W
	btfss	C
	goto	DateMonth_AddOverflow
	incf	R_Date_Month
	goto	DateMonth_Add_Exit
DateMonth_AddOverflow:
	movlw	1
	movwf	R_Date_Month
DateMonth_Add_Exit:
	call	F_Date_Display
	return

; 月减少
Date_Month_Sub:
	movlw	1
	movf	R_Date_Month,W
	btfss	C
	goto	DateMonth_SubOverflow
	decf	R_Date_Month,F
	goto	DateMonth_Sub_Exit
DateMonth_SubOverflow:
	movlw	12
	movwf	R_Date_Month
DateMonth_Sub_Exit:
	call	F_Date_Display
	return

; 日增加
Date_Day_Add:
	incf	R_Date_Day,F
	call	L_DayOverflow_To_1
	call	F_Date_Display
	return

; 日减少
Date_Day_Sub:
	movlw	1
	movf	R_Date_Day,W
	btfss	C
	goto	DateDay_SubOverflow
	decf	R_Date_Day,F
	goto	DateDay_Sub_Exit
DateDay_SubOverflow:					; 溢出时天数变为此月最大天数
	decf	R_Date_Month,W
	movf	R_X
	btfsc	Calendar_Flag,0
	goto	L_LeapYear_Handle3			; 查闰年表
	movlw	High(L_Table_Month_Common)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Common)
	goto	L_Day_Juge2
L_LeapYear_Handle3:
	movlw	High(L_Table_Month_Leap)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Leap)
L_Day_Juge2:
	addwf	R_X,W
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W					; 查表得出当月最大天数
	movwf	R_Date_Day
	call	F_Date_Display
	return


;======================================================================================

L_DayOverflow_Juge:
	bcf		RP0
	call	Is_Leap_Year				; 判断当前是平年还是闰年
	
	decf	R_Date_Month,W
	movf	R_X
	btfsc	Calendar_Flag,0
	goto	L_LeapYear_Handle			; 查闰年表
	movlw	High(L_Table_Month_Common)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Common)
	goto	L_Day_Juge1
L_LeapYear_Handle:
	movlw	High(L_Table_Month_Leap)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Leap)
L_Day_Juge1:
	addwf	R_X,W
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W					; 查表得出当月最大天数
	movwf	P_Temp
	subwf	R_Date_Day,W
	btfsc	C
	goto	Day_Overflow_Yes			; 当前日期天数溢出
	return
Day_Overflow_Yes:
	movf	P_Temp,W
	movwf	R_Date_Day					; 溢出后不再增加天数
	return




L_DayOverflow_To_1:
	bcf		RP0
	call	Is_Leap_Year				; 判断当前是平年还是闰年
	
	decf	R_Date_Month,W
	movf	R_X
	btfsc	Calendar_Flag,0
	goto	L_LeapYear_Handle			; 查闰年表
	movlw	High(L_Table_Month_Common)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Common)
	goto	L_Day_Juge1
L_LeapYear_Handle:
	movlw	High(L_Table_Month_Leap)
	movwf	PMADRH
	movlw	Low(L_Table_Month_Leap)
L_Day_Juge1:
	addwf	R_X,W
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W					; 查表得出当月最大天数
	movwf	P_Temp
	subwf	R_Date_Day,W
	btfsc	C
	goto	Day_Overflow_Yes1			; 当前日期天数溢出
	return
Day_Overflow_Yes1:
	movlw	1
	movwf	R_Date_Day					; 溢出后天数清空
	return






F_KeyDelay:
	bcf		RP0

	movlw	80
	movwf	P_Temp

	decfsz	P_Temp,F
	goto	$-1
	return

L_Key_Beep:
	movlw	B'10'
	movwf	Beep_Serial
	return
