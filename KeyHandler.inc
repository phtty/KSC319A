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
	call	SwitchState_ClockDis		; 时钟显示的子模式切换
	goto	L_KeyHandle_Over

L_KeyA_ShortHandle:
	btfsc	Primary_Status,1
	nop									; 闹钟显示下的开关闹钟
	goto	L_KeyHandle_Over

L_KeyB_ShortHandle:
	nop
	goto	L_KeyHandle_Over

L_KeyU_ShortHandle:
	nop
	goto	L_KeyHandle_Over

L_KeyD_ShortHandle:
	nop
	goto	L_KeyHandle_Over




; 按键触发函数，处理每个按键触发后的响应条件
L_KeyMTrigger:
	call	L_Universal_TriggerHandle	; 如果是熄屏后唤醒，则是无按键功能
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7					; 判断是否需要直接退出整个按键处理
	goto	L_KeyHandle_Over

	movlw	B'00000101'
	andwf	Primary_Status,W
	btfsc	Z
	nop									; 非时钟设置或显示模式，M键会直接回到时钟显示

	btfsc	Key_Flag,3
	goto	L_KeyMTrigger_LongTri:
	bsf		SpecialKey_Flag,0			; 长按标志到来前置位标志位，交给短按函数处理
	return								; 短按在按键触发部分只置标志位就退出

L_KeyMTrigger_LongTri:					; 长按部分处理
	btfsc	Primary_Status,0
	nop									; 切换到时钟设置
	btfsc	Primary_Status,2
	nop									; 根据子模式进不同的长按处理或者进模式切换
	return								; 看情况要不要直接退出按键处理，若不退，则return
	goto	L_KeyHandle_Over


L_KeyATrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over

	movlw	B'00001010'
	andwf	Primary_Status,W
	btfsc	Z
	nop									; 非闹钟设置或显示模式，A键会直接回到时钟显示

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
	btfsc	Primary_Status,6
	goto	AddNum_SNZS
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
	btfsc	Primary_Status,6
	goto	SubNum_SNZS
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




; 切换时钟显示
SwitchState_ClockDis:
	movlw	B'00000001'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2CD						; 若当前非时钟显示则只切换到时钟显示

	incf	Secondary_Status			; 时钟显示模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	Z
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
	
	incf	Secondary_Status			; 闹钟设置模式的子模式切换
	movlw	3
	subwf	Secondary_Status,W
	btfsc	Z
L_CLR_SS1:
	clrf	Secondary_Status			; 超过最后一个子模式序号则清零回到第一个
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理


SW2CS:
	movlw	B'00000100'
	movwf	Primary_Status
	goto	L_CLR_SS1




; 切换到闹钟设置模式
SwitchState_AlarmSet:
	movlw	B'00001000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2AS						; 若当前非闹钟设置则只切换到闹钟设置
	
	incf	Secondary_Status			; 时钟设置模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	Z
L_CLR_SS2:
	clrf	Secondary_Status			; 超过最后一个子模式序号则清零回到第一个
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理


SW2AS:
	movlw	B'00001000'
	movwf	Primary_Status
	goto	L_CLR_SS2





; 切换到日期设置模式
SwitchState_DateSet:
	movlw	B'00010000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2DS						; 若当前非日期设置则只切换到日期设置
	
	incf	Secondary_Status			; 日期设置模式的子模式切换
	movlw	3
	subwf	Secondary_Status,W
	btfsc	Z
L_CLR_SS3:
	clrf	Secondary_Status			; 超过最后一个子模式序号则清零回到第一个
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理


SW2DS:
	movlw	B'00010000'
	movwf	Primary_Status
	goto	L_CLR_SS3




; 切换到夜间时段设置模式
SwitchState_NightPeriodSet:
	movlw	B'00100000'
	andwf	Primary_Status,W
	btfsc	Z
	goto	SW2NS						; 若当前非夜间时段设置则只切换到夜间时段设置

	incf	Secondary_Status			; 夜间时段设置模式的子模式切换
	movlw	4
	subwf	Secondary_Status,W
	btfsc	Z
L_CLR_SS4:
	clrf	Secondary_Status			; 超过最后一个子模式序号则清零回到第一个
	goto	L_KeyHandle_Over			; 状态切换的功能会结束按键处理


SW2NS:
	movlw	B'00100000'
	movwf	Primary_Status
	goto	L_CLR_SS4




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
