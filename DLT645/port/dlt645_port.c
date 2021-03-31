/*************************************************
 *  Copyright (c) 2021
 *  All rights reserved.
 *  File name:     dlt645_port.c
 *  Description:   DLT645 移植&使用例程文件
 *  History:
 *      1. Version:    
 *			Date:       2021-02-19
 *          Author:     guhaiming
 *          Modify:     
 **************************************************/
#include "dlt645.h"
#include "serial.h"
#include <unistd.h>
#include <stdio.h>

dlt645_t dlt645;

/**
 * Name:    dlt645_hw_read
 * Brief:   dlt645 硬件层接收数据
 * Input:
 *  @ctx:   645运行环境
 *  @msg:   接收数据存放地址
 *  @len:   数据最大接收长度 
 * Output:  读取数据的长度
 */
//原来是static声明的 
int dlt645_hw_read(SerialPort self, uint8_t *msg ,uint16_t len)
{
	//实际接收长度
	int read_len = 0;
	
	printf("file: %s at line %d is ok!\n",__FILE__,__LINE__);
	//每次读取一个字节的数据
	while (SerialPort_readByte(self->fd,msg + read_len, 1) == 1)
	{
		if(read_len > len)
		{
			return 0;
		}
		else
		{
			read_len ++;
		}
	}
	return read_len;
}

/**
 * Name:    dlt645_hw_write
 * Brief:   dlt645 硬件层发送数据
 * Input:
 *  @ctx:   645运行环境
 *  @buf:   待发送数据
 *  @len:   发送长度
 * Output:  实际发送的字节数，错误返回-1
 */
static int dlt645_hw_write(SerialPort self, uint8_t *buf, uint16_t len)
{
	//串口发送数据
	printf("hello 1 !\n");
	printf("file :%s  line :%d  fd:%d\n",__FILE__,__LINE__,self->fd);
	printf("hello 2 !\n");
	printf("self -> fd is :%d\n",self->fd);
	return SerialPort_write(self->fd, buf, 0, len);
}


/**
 * Name:    dlt645_port_init
 * Brief:   645采集硬件层初始化
 * Input:   None
 * Output:  None
 */
SerialPort dlt645_port_init(int portNo,int baudRate,int dataBits,int parity,int stopBits)
{
	SerialPort                  self;                                       
	int                         rt=0;


	//串口初始化
	self=SerialPort_create(portNo,baudRate,dataBits,parity,stopBits);    
	rt=SerialPort_open(self,portNo);
	printf("rt:%d fd:%d\n",rt,self->fd);                                                               
	if(rt<0)
	{
		printf("at line %d open serial failture!\n",__LINE__);
		exit(1);//处理可能不是很恰当
	}
	
	return  self;
}


//645结构体注册 原来是 static 现在改了
dlt645_t dlt645 = {
	{0},
	0,
	dlt645_hw_write,
	dlt645_hw_read};
