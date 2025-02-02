#include "OneWire.h"

/**
 * @brief  GPIO模式转换
 * @param  无
 * @retval IN(0)为输入，OUT(1)为输出
 */

void GPIOMode(uint8_t Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = OneWire_Pin;
  if (Mode == IN)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(OneWire_GPIO, &GPIO_InitStructure);
  }
  if (Mode == OUT)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD; // 开漏输出
    GPIO_Init(OneWire_GPIO, &GPIO_InitStructure);
  }
}

/**
 * @brief  单总线初始化
 * @param  无
 * @retval 从机响应位，0为响应，1为未响应
 */
uint8_t OneWire_Init(void)
{
  uint8_t AckBit;
  RCC_APB2PeriphClockCmd(RCCOneWire, ENABLE);
  // 配置GPIOA.6为输出
  GPIOMode(OUT);
  OneWire_High;        // 释放总线
  OneWire_Low;         // 输出低电平
  Delay_us(500);       // Delay 500us
  OneWire_High;        // 释放总线
  Delay_us(70);        // Delay 70us 复位+从机响应时间
  GPIOMode(IN);        // 配置GPIOA.6为输入
  AckBit = OneWire_DQ; // 读取从机响应位
  Delay_us(500);       // Delay 500us 释放总线
  GPIOMode(OUT);       // 恢复GPIOA.6为开漏输出
  return AckBit;
}

/**
 * @brief  单总线发送一位
 * @param  Bit 要发送的位
 * @retval 无
 */
void OneWire_SendBit(unsigned char Bit)
{
  if (Bit)
  {               // 发送1
    OneWire_Low;  // 输出低电平
    Delay_us(10);  // Delay 5us
    OneWire_High; // 释放总线
    Delay_us(50); // Delay 55us
    OneWire_High; // 释放总线
  }
  else
  {               // 发送0
    OneWire_Low;  // 输出低电平
    Delay_us(60); // Delay 70us
    OneWire_High; // 释放总线
  }
}

/**
 * @brief  单总线接收一位
 * @param  无
 * @retval 读取的位
 */
uint8_t OneWire_ReceiveBit(void)
{
  uint8_t Bit;
  OneWire_Low;  // 输出低电平
  Delay_us(5);  // Delay 5us
  OneWire_High; // 释放总线
  Delay_us(5); // Delay 10us
  GPIOMode(IN); // 配置GPIOA.6为输入
  Bit = OneWire_DQ;
  Delay_us(50); // Delay 50us 释放总线
  GPIOMode(OUT); // 恢复GPIOA.6为开漏输出
  return Bit;
}

/**
 * @brief  单总线发送一个字节
 * @param  Byte 要发送的字节
 * @retval 无
 */
void OneWire_SendByte(unsigned char Byte)
{
  // 发送8位 低位在前
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    OneWire_SendBit(Byte & (0x01 << i));
  }
}

/**
 * @brief  单总线接收一个字节
 * @param  无
 * @retval 接收的一个字节
 */
uint8_t OneWire_ReceiveByte(void)
{
  // 接收8位 低位在前
  unsigned char i;
  unsigned char Byte = 0x00;
  for (i = 0; i < 8; i++)
  {
    if (OneWire_ReceiveBit())
    {
      Byte |= (0x01 << i);
    }
  }
  return Byte; // 返回接收到的字节 低位在前
}

// 模拟一次读写
// DS18B20 欲发送字节 0101 0101
// DS18B20 发送字节 1010 1010 低位在前
// 主机接受字节 byte=0101 0101 得到最终结果
