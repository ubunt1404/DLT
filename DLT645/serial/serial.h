/**
 * @file:      serial.h
 * @brief:	   串口头文件
 * @version:   V1.0.0
 * @author:    guhaiming
 * @date:      2021-1-07
 * @update:    [][][]
 */
#ifndef _SERIAL_H_
#define _SERIAL_H_

/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

	/* PRIVATE VARIABLES ---------------------------------------------------------*/
#define BOOL int
#define TRUE 1
#define FALSE 0
	/* PUBLIC VARIABLE -----------------------------------------------------------*/
	typedef struct sSerialPort* SerialPort;
	typedef enum 
	{
		SERIAL_PORT_ERROR_NONE = 0,
		SERIAL_PORT_ERROR_INVALID_ARGUMENT = 1,
		SERIAL_PORT_ERROR_INVALID_BAUDRATE = 2,
		SERIAL_PORT_ERROR_OPEN_FAILED = 3,
		SERIAL_PORT_ERROR_UNKNOWN = 99
	} SerialPortError;
	
	/*串口属性 */
	struct sSerialPort {
		char interfaceName[100];//串口路径名
		int fd;                 //串口号                                                             
		int baudRate;           //波特率
		uint8_t dataBits;       //数据位
		char parity;            //校验位
		uint8_t stopBits;       //停止位
		uint64_t lastSentTime;  //最后发送时间
		struct timeval timeout; //超时
		SerialPortError lastError;
	};
	typedef struct 
	{
		char name[100];
		int fd;
	}serial_name_fd_str;

	/* PRIVATE FUNCTION PROTOTYPES -----------------------------------------------*/

	/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
	SerialPort SerialPort_create(const uint8_t portNo, int baudRate, uint8_t dataBits, uint8_t parity, uint8_t stopBits);
	void SerialPort_destroy(SerialPort self);
	int SerialPort_open(SerialPort self,const uint8_t portNo);
	void SerialPort_close(SerialPort self);
	int  SerialPort_getBaudRate(SerialPort self);
	void SerialPort_setTimeout(SerialPort self, int timeout);
	void SerialPort_discardInBuffer(SerialPort self);
	int SerialPort_readByte(uint8_t fd, uint8_t *pbuf, uint16_t count);
	int SerialPort_write(int fd, uint8_t* buffer, int startPos, int bufSize);
	SerialPortError SerialPort_getLastError(SerialPort self);
	int portNo_to_name(char *name, uint8_t portNo);
	int portNo_to_fd(int portNo);

#ifdef __cplusplus
}
#endif

#endif /* _SERIAL_H_ */
/* END OF FILE ----------------------------------------------------------------*/
