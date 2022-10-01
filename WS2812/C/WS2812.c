#include "WS2812.h"
uint32_t WS2812_Data[WS2812_Num] = {0};

#ifdef WS2812_Software
void WS2812_Send_Byte(uint8_t Dat);
inline void WS2812_Code_0(void)
{
    HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_SET);
    for (int i = 0; i < 6; i++)
        __nop();
    HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_RESET);
    for (int i = 0; i < 11; i++)
        __nop();
}
inline void WS2812_Code_1(void)
{
    HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_SET);
    for (int i = 0; i < 12; i++)
        __nop();
    HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_RESET);
    for (int i = 0; i < 4; i++)
        __nop();
}
inline void WS2812_Send_Byte(uint8_t Dat)
{
    for (int i = 0; i < 8; i++)
    {
        if (Dat & 0x80)
        {
            WS2812_Code_1();
        }
        else
        {
            WS2812_Code_0();
        }
        Dat <<= 1;
    }
}
void WS2812_Code_Reast(void)
{
    HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}
void WS2812_Color_Decode(uint32_t Color)
{
    WS2812_Send_Byte((Color & 0x00ff00) >> 8);
    WS2812_Send_Byte((Color & 0xff0000) >> 16);
    WS2812_Send_Byte((Color & 0x0000ff) >> 0);
}
//动作函数，发送编码给灯
void WS2812_Send(void)
{
    for (int i = 0; i < 2; i++)
    {
        WS2812_Code_Reast();
        for (int i = 0; i < WS2812_Num; i++)
        {
            WS2812_Color_Decode(WS2812_Data[i]);
        }
        HAL_GPIO_WritePin(WS2812_GPIO_Group, WS2812_GPIO_Pin, GPIO_PIN_SET);
    }
}
#endif

#ifdef WS2812_Hardware
uint32_t WS2812_SendBuf0[25] = {0};
uint32_t WS2812_SendBuf1[25] = {0};
uint32_t WS2812_Rst[240] = {0};
uint32_t WS2812_En = 0;
void WS2812_uint32ToData(uint32_t Data, uint32_t *Ret)
{
    uint32_t zj = Data;
    uint8_t *p = (uint8_t *)&zj;
    uint8_t R = 0, G = 0, B = 0;
    B = *(p);     // B
    G = *(p + 1); // G
    R = *(p + 2); // R
    zj = (G << 16) | (R << 8) | B;
    for (int i = 0; i < 24; i++)
    {
        if (zj & (1 << 23))
            Ret[i] = WS2812_Code_1;
        else
            Ret[i] = WS2812_Code_0;
        zj <<= 1;
    }
    Ret[24] = 0;
}
// DMA中断函数调用
void WS2812_Send()
{
    static uint32_t j = 0;
    static uint32_t ins = 0;
    if (WS2812_En == 1)
    {
        if (j == WS2812_Num)
        {
            j = 0;
            HAL_TIM_PWM_Stop_DMA(&WS2812_TIM, WS2812_TIM_Channel);
            WS2812_En = 0;
            return;
        }
        j++;
        if (ins == 0)
        {
            HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel, WS2812_SendBuf0, 25);
            WS2812_uint32ToData(WS2812_Data[j], WS2812_SendBuf1);
            ins = 1;
        }
        else
        {
            HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel, WS2812_SendBuf1, 25);
            WS2812_uint32ToData(WS2812_Data[j], WS2812_SendBuf0);
            ins = 0;
        }
    }
}
void WS2812_Start(void)
{
    HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel, (uint32_t *)WS2812_Rst, 240);
    WS2812_uint32ToData(WS2812_Data[0], WS2812_SendBuf0);
    WS2812_En = 1;
}
void WS2812_Code_Reast(void)
{
    HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel, (uint32_t *)WS2812_Rst, 240);
    WS2812_En = 0;
}
#endif
