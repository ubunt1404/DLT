#ifndef __DLT645_PORT_H
#define __DLT645_PORT_H
#include "dlt645.h"
#include "serial.h"

//对外提供环境声明
extern dlt645_t dlt645;
//645采集硬件层初始化
SerialPort dlt645_port_init(int portNo,int baudRate,int dataBits,int parity,int stopBits);
#endif /* __DLT645_PORT_H */
