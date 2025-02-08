#include "W25Q64.h"
//接口

//W25Q64初始化 初始化SPI即可
void W25Q64_Init(void)
{
	MySPI_Init();
}
//读取W25Q64ID
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Start(); //发送开始信号
	MySPI_SwapByte(W25Q64_JEDEC_ID); //发送指令
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE); //接收厂商ID
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE); //接收设备ID 存储类型
	*DID <<= 8; //左移八位 高八位
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE); //接收设备ID 容量
	MySPI_Stop();
}

//写使能
void W25Q64_WriteEnable(void)
{
	MySPI_Start(); //时序开始
	MySPI_SwapByte(W25Q64_WRITE_ENABLE); //发送写使能指令
	MySPI_Stop(); //时序结束
}
//忙等待
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;// 超时等待时长
	MySPI_Start(); //发送开始信号
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1); //读取状态寄存器指令
	Timeout = 100000; //延时时长
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01) //接受状态寄存器的值，获取第一位 状态寄存器可以被连续读出
	{
		Timeout --;//计时减1
		if (Timeout == 0) //时间到了 跳出循环
		{
			break;
		}
	}
	MySPI_Stop();//停止时序
}
//写使能
/*
@brief：单次写入只能写入一页256B，地址信息不能跨页，否则将在页头复写
*/
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q64_WriteEnable(); //必须先写使能
	
	MySPI_Start(); //时序开始
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);//发送指令
	MySPI_SwapByte(Address >> 16);//发送高八位
	MySPI_SwapByte(Address >> 8);//发送中八位
	MySPI_SwapByte(Address); //发送第八位
	for (i = 0; i < Count; i ++) //循环发送Data
	{
		MySPI_SwapByte(DataArray[i]);
	}
	MySPI_Stop();//停止时序
	
	W25Q64_WaitBusy();//等待buzy结束
}
/*
@brief:扇区擦除 一次擦除一个扇区为4KB，发送一个地址，改地址所在的扇区将全部擦除
*/
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable(); //必须先执行写使能 数据手册规定
	
	MySPI_Start();//时序开始
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB); //块擦除指令
	MySPI_SwapByte(Address >> 16);//发送高八位
	MySPI_SwapByte(Address >> 8);//发送中八位
	MySPI_SwapByte(Address);//发送第八位
	MySPI_Stop();//结束时序
	
	W25Q64_WaitBusy(); //等待写1结束
}
/*
@brief:读取目标地址字节，可以循环读取，地址自动+1
*/
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i; //读取计数
	MySPI_Start();//开始时序
	MySPI_SwapByte(W25Q64_READ_DATA);//读数据指令
	MySPI_SwapByte(Address >> 16);//高八位
	MySPI_SwapByte(Address >> 8);//中八位
	MySPI_SwapByte(Address);//低八位
	for (i = 0; i < Count; i ++) //循环读取
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();//停止时序
}
