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
	call	L_Key_Beep

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
	nop
	goto	L_KeyHandle_Over

L_KeyA_ShortHandle:
	nop
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
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7					; 判断是否需要直接退出整个按键处理
	goto	L_KeyHandle_Over
	bsf		SpecialKey_Flag,0
	nop
	return
	goto	L_KeyHandle_Over


L_KeyATrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over
 	bsf		SpecialKey_Flag,1
	nop
	return
	goto	L_KeyHandle_Over


L_KeyBTrigger:
	call	L_Universal_TriggerHandle
	bsf		SpecialKey_Flag,2
	nop
	return
	goto	L_KeyHandle_Over


L_KeyUTrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over
	bsf		SpecialKey_Flag,3
	nop
	return
	goto	L_KeyHandle_Over


L_KeyDTrigger:
	call	L_Universal_TriggerHandle
	call	L_Key_NoSnoozeLoud
	btfsc	Key_Flag,7
	goto	L_KeyHandle_Over
	bsf		SpecialKey_Flag,4
	nop
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
	nop									; 打断响闹和贪睡
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
