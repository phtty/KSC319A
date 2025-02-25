cblock				100H
; 用于现场保护的寄存器备份
W_BAK
STATUS_BAK
PCLATH_BAK

; 变址寻址专用变量
R_X

; T0定时器计数
T0_Counter
; T1定时器计数
T1_Counter

Calendar_Flag
;bit0=1 闰年
;bit1=1 增日期

Beep_Flag
;bit0=1 开启蜂鸣

Clock_Flag
;bit0=1 12H模式
;bit1=1 闹钟触发
;bit2=1 响闹模式
;bit3=1 贪睡模式
;bit4=1 响闹阻塞标志，防止短时间内关闭响闹继续进闹钟触发
;bit5	上次的响闹模式，判断是响闹结束还是未响闹
;bit6=1 贪睡按键触发导致的响闹结束

Time_Flag
;bit0=1 1Hz标志		|走时显示
;bit1=1 2Hz标志		|走时显示
;bit2=1 4Hz标志		|快加
;bit3=1 16Hz标志	|响铃
;bit4=1 32Hz标志	|扫键
;bit5=1 增S标志		|增时走时

TimeFlag_SW
;bit0=1 4Hz标志开启
;bit1=1 16Hz标志开启
;bit2=1 32Hz标志开启

Primary_Status
;bit0=1 时钟显示
;bit1=1 闹钟显示
;bit2=1 设置时钟
;bit3=1 设置闹钟
Secondary_Status
;子状态序号

Key_Flag
;bit0=1 存在压键事件
;bit1=1 存在松键事件
;bit2=1 首次触发标志
;bit3=1 快加标志
;bit4=1 按键音标志
;bit5=1 SNZ键长按
;bit6=1 长按1S标志
;bit7=1 按键处理终止

SpecialKey_Flag
;bit0=1 Mode键待处理				|特殊按键
;bit1=1 Alarm键待处理				|特殊按键
;bit2=1 SNZ/Backlight键待处理		|特殊按键
;bit3=1 Up键待处理					|特殊按键
;bit4=1 Down键待处理				|特殊按键

Power_Flag

ADC_Flag
;bit0=1 负温度标志
;bit1=1 华氏度模式标志

PORTA_BackUp

QuickAdd_Counter

Beep_Serial

R_ADC_L
R_ADC_H
R_Temperature			; 温度（摄氏）
R_Temperature_F			; 温度（华氏）

R_Time_Sec				; 时间秒
R_Time_Min				; 时间分
R_Time_Hour				; 时间小时
R_Alarm_Min				; 闹钟分
R_Alarm_Hour			; 闹钟小时
R_Date_Day				; 日期，从1开始
R_Date_Month			; 月份，从1开始
R_Date_Year				; 年份，由于只计2000-2099，所以由0-99代替

R_Snooze_Min			;贪睡闹钟分钟
R_Snooze_Hour			;贪睡闹钟小时

R_Snooze_Time			;贪睡闹钟小时

; COM计数
COM_Counter
; 发送位数计数
L_Send_Counter

; 显存
DRAM:
DRAM1
DRAM2
DRAM3

DRAM_Buffer:
DRAM_Buffer0
DRAM_Buffer1

; 临时变量
P_Temp:
P_Temp1
P_Temp2
P_Temp3
P_Temp4
P_Temp5
P_Temp6
P_Temp7

endc

cblock	300H
endc
