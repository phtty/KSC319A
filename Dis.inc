F_FillScreen:
	bcf		RP0
	movlw	0xff
	goto	L_FillLed
F_ClearScreen:
	bcf		RP0
	movlw	0x00
L_FillLed:
	movwf	DRAM
	movwf	DRAM+1
	movwf	DRAM+2
	movwf	DRAM+3
	return




; 显示七段数字
; W==要显示的数字，X==要显示位码的偏移
F_7Digit_Dis:
	bcf		RP0

	movwf	P_Temp
	movlw	High(Table_7bit_Digit)	; 先查表得出a~g的码对应的二进制位
	movwf	PMADRH
	movlw	Low(Table_7bit_Digit)
	addwf	P_Temp,W
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W
	movwf	P_Temp1					; 目标数字的a-g码放入Temp1中

	movf	R_X,W
	movwf	P_Temp

	movlw	7
	movwf	P_Temp2					; 循环位移7次计数

L_DRAM_OprationStart:
	movlw	High(Led_bit)			; 查表得出显存中bit偏移
	movwf	PMADRH
	movlw	Low(Led_bit)
	addwf	P_Temp,W				; 加上位码的偏移
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W
	movwf	P_Temp3					; 要操作的显存位存进Temp3

	movlw	High(Led_byte)			; 查表得出显存中byte偏移
	movwf	PMADRH
	movlw	Low(Led_byte)
	addwf	P_Temp,W				; 加上位码的偏移
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W
	movwf	R_X						; 显存偏移暂存进X

	rrf		P_Temp1,F
	btfss	C
	goto	L_DRAM_ClrBit
	movlw	Low(DRAM)
	addwf	R_X,W
	movwf	FSR0L
	movlw	High(DRAM)
	movwf	FSR0H
	movf	P_Temp3,W
	iorwf	INDF0,F
	goto	L_DRAM_OprationJuge
L_DRAM_ClrBit:
	movlw	Low(DRAM)
	addwf	R_X,W
	movwf	FSR0L
	movlw	High(DRAM)
	movwf	FSR0H
	movf	P_Temp3,W
	iorwf	INDF0,F
	xorwf	INDF0,F
L_DRAM_OprationJuge:
	incf	P_Temp,F
	decfsz	P_Temp2
	goto	L_DRAM_OprationStart
	return




F_6Digit_Dis:
	movlw	0xfc
	andwf	DRAM+1,F
	movlw	0xf0
	andwf	DRAM+3,F

	movlw	High(Table_6Digit_Jump)
	movwf	PCLATH
	movf	R_X,W
Table_6Digit_Jump:
	addwf	PCL,F
	goto	L_D0_Dis0
	goto	L_D0_Dis1
	goto	L_D0_Dis2
	goto	L_D0_DisMinus
	return
L_D0_Dis0:

	return
L_D0_Dis1:
	movlw	0x02
	iorwf	DRAM+1,F
	movlw	0x08
	iorwf	DRAM+3,F
	return
L_D0_Dis2:
	movlw	0x03
	iorwf	DRAM+1,F
	movlw	0x07
	iorwf	DRAM+3,F
	return
L_D0_DisMinus:
	movlw	0x02
	iorwf	DRAM+3,F
	return




; 画点、清点函数
; X==要显示位码的偏移
F_DisSymbol:
	bcf		RP0
	call	F_DisSymbol_Com
	return

F_ClrSymbol:
	bcf		RP0
	call	F_DisSymbol_Com
	xorwf	INDF0,F
	return

F_DisSymbol_Com:
	movlw	High(Led_bit)			; 查表得出显存中bit偏移
	movwf	PMADRH
	movlw	Low(Led_bit)
	addwf	R_X,W					; 加上位码的偏移
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W
	movwf	P_Temp					; 要操作的显存位存进Temp0

	movlw	High(Led_byte)			; 查表得出显存中byte偏移
	movwf	PMADRH
	movlw	Low(Led_byte)
	addwf	R_X,W					; 加上位码的偏移
	movwf	PMADRL
	bsf		RDON
	nop
	nop
	movf	PMDATH,W
	movwf	R_X

	movlw	Low(DRAM)
	addwf	R_X,W
	movwf	FSR0L
	movlw	High(DRAM)
	movwf	FSR0H
	movf	P_Temp,W
	iorwf	INDF0,F
	return




; 发送显存里一个COM的内容
L_Send_Buffer_COM:
	bcf		RP0

	bcf		PORTF,0					; 拉低LE锁存5020数据
	bcf		PORTF,2					; SDI空闲时低电平
	bcf		PORTF,1					; CLK空闲时低电平
	bcf		C
	rlf		COM_Counter,W			; 用COM序号乘以2算出显存的偏移
	movwf	R_X
	movlw	low(DRAM)
	addwf	R_X,W
	movwf	FSR0L
	movlw	High(DRAM)
	movwf	FSR0H
	movf	INDF0,W
	movwf	DRAM_Buffer

	incf	R_X,F
	movlw	low(DRAM)
	addwf	R_X,W
	movwf	FSR0L
	movlw	High(DRAM)
	movwf	FSR0H
	movf	INDF0,W
	movwf	DRAM_Buffer+1			; 将显存的值取出，放入发送缓存中

	movlw	16
	movwf	L_Send_Counter
L_Sending_Loop:						; 由于5020是MSB，发送必须高位先发
	rlf		DRAM_Buffer,F
	rlf		DRAM_Buffer+1,F
	btfss	C
	goto	L_Send_0
	bsf		PORTF,2					; SDI输出1
	goto	L_CLK_Change
L_Send_0:
	bcf		PORTF,2					; SDI输出0
L_CLK_Change:
	bcf		PORTF,1
	nop
	nop
	nop
	bsf		PORTF,1					; CLK产生一次上升沿，让5020内数据开始位移
	decfsz	L_Send_Counter
	goto	L_Sending_Loop

	bsf		RP0						; PWM寄存器在BANK1
	clrf	PWMEN					; 先关闭所有COM，避免漏光
	bcf		RP0						; 切回BACNK0
	bcf		PORTF,2					; SDI空闲时低电平
	bcf		PORTF,1					; CLK空闲时低电平
	bsf		PORTA,0
	bsf		PORTA,1
	bsf		PORTF,0					; 拉高LE，5020开始显示数据
	nop
	nop
	bcf		PORTF,0					; 重新锁存数据，防止意外改变

	movf	COM_Counter,F			; 根据COM号重新打开一个COM的输出
	bsf		RP0						; PWM寄存器在BANK1
	btfss	Z
	goto	L_COM0_Send
	bsf		PWMEN,3
	bcf		RP0
	return
L_COM0_Send:
	bsf		PWMEN,0
	bcf		RP0
	return


Table_7bit_Digit:
	db	0x3f	; 0
	db	0x06	; 1
	db	0x5b	; 2
	db	0x4f	; 3
	db	0x66	; 4
	db	0x6d	; 5
	db	0x7d	; 6
	db	0x07	; 7
	db	0x7f	; 8
	db	0x6f	; 9
	db	0x00	; undisplay
