#ifndef _WS2812_H_
#define _WS2812_H_
#include "main.h"
#include "Delay.h"
#include "main.h"
/*
软件模拟时序
需要
    1.GPIO
        开漏浮空输出模式，需要外接5V上拉，注意需要选择手册中标有FT的管脚
        将其设置为最高规格输出
*/
#define WS2812_GPIO_Group GPIOA
#define WS2812_GPIO_Pin GPIO_PIN_0
// #define WS2812_Software

/*
硬件定时器PWM+DMA:
需要:
    1.定时器:
        PWM输出一个通道
        不分频
        计数值为 1.25us(公式: 1.25 *系统频率(单位MHz))
        输出IO设置为开漏浮空输出(外接5V上拉)
    2.DMA
        内存到外设
        字(word)模式
        开启DMA中断
0码的是 0.4us,1码是0.8us
公式是 t(us)*系统频率(单位MHz)
 */
extern TIM_HandleTypeDef htim1;
#define WS2812_Hardware
#define WS2812_TIM htim1
#define WS2812_TIM_Channel TIM_CHANNEL_1
#define WS2812_Code_0 (32u)
#define WS2812_Code_1 (71u)

#define WS2812_Num 8

extern uint32_t WS2812_Data[WS2812_Num];
void WS2812_Code_Reast(void);
void WS2812_Send(void);

void WS2812_Start(void);
#endif
