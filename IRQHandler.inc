T0_IRQ_Handler:
	bcf		T0IF
	decf	T0_Counter

L_32Hz_Juge:
	btfss	TimeFlag_SW,2			; 32Hz标志开关
	goto	L_16Hz_Juge
	bsf		Time_Flag,4				; 32Hz标志位

L_16Hz_Juge:
	btfss	TimeFlag_SW,1
	goto	L_4Hz_Juge
	movf	T0_Counter,W
	andlw	0x01					; 对2求余
	btfsc	Z
	bsf		Time_Flag,3				; 16Hz标志位

L_4Hz_Juge:
	btfss	TimeFlag_SW,0
	goto	L_2Hz_Juge
	movf	T0_Counter,W
	andlw	0x07					; 对8求余
	btfsc	Z
	bsf		Time_Flag,2				; 4Hz标志位

L_2Hz_Juge:
	movf	T0_Counter,W
	andlw	0x0f					; 对16求余
	btfsc	Z
	bsf		Time_Flag,1				; 2Hz标志位

	movf	T0_Counter,F			; 判0
	btfsc	Z
	goto	T0_Counter_Reload

	goto	IRQ_EXIT

T0_Counter_Reload:
	movlw	32
	movwf	T0_Counter				; 重装载T0计数
	bsf		Time_Flag,0				; 1Hz标志
	bsf		Key_Flag,6				; 长按1S标志
	goto	IRQ_EXIT




T1_IRQ_Handler:
	bcf		T1IF
	movlw	0xff
	movwf	T1H
	movlw	0xff-0x7f
	movwf	T1L						; 重装载T1计数器的计数值


	btfsc	COM_Counter,0
	goto	COM_Display
	clrf	COM_Counter
COM_Display:
	call	L_Send_Buffer_COM
	incf	COM_Counter,F
	goto	IRQ_EXIT




PA_IRQ_Handler:
	movf	PORTA,F
	bcf		RAIF

	movlw	0x7c
	andwf	PORTA,W					; PA2~6有1个为1则有键压下
	btfss	Z
	goto	PortA_PressKey
	btfsc	Key_Flag,0				; 只有在压键事件存在时，才会判断有无松键事件
	goto	PortA_ReleaseKey
	goto	IRQ_EXIT
PortA_PressKey:
	bsf		Key_Flag,0				; 压键事件标志位
	bsf		Key_Flag,2				; 首次触发标志位
	goto	IRQ_EXIT
PortA_ReleaseKey:
	movlw	0x7c
	andwf	PORTA,W
	btfss	Z
	goto	PortA_ReleaseKey_Exit	; 若PA2~6都为0，则为抬起
	bsf		Key_Flag,1				; 松键事件标志位
	bcf		Key_Flag,0
PortA_ReleaseKey_Exit:
	goto	IRQ_EXIT




PB_IRQ_Handler:
	movf	PORTB,F
	bcf		RBIF
	
	goto	IRQ_EXIT




PF_IRQ_Handler:
	movf	PORTF,F
	bcf		RFIF

	goto	IRQ_EXIT
