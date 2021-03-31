/**
 * @file:      serial.c
 * @brief:		linux 串口
 * @version:   V1.0.0
 * @author:    guhaiming
 * @date:      2021-1-07
 * @update:    [][][]
 */

#define LOG_TAG    "serial"
/* INCLUDE FILES -------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include "serial.h"
/* PRIVATE VARIABLES ---------------------------------------------------------*/

/*串口名路径*/
static const char *uart_dev[] =
{
	"/dev/ttyUSB0"		//笔记本上USB转串口 
};

/*打开对应串口设备文件得到的 fd号*/
int uartTofd[4] = {-1, -1, -1, -1};

/* PUBLIC FUNCTION PROTOTYPES ------------------------------------------------*/
/**
 * @brief : 初始化串口创建所需参数.
 * @param : [串口号]
 * @param : [波特率]
 * @param : [数据位]
 * @param : [校验]
 * @param : [停止位]
 * @return: [结构体数据指针]
 * @updata: [][][]
 */
SerialPort SerialPort_create(const uint8_t portNo, int baudRate, uint8_t dataBits, uint8_t parity, uint8_t stopBits)
{
	SerialPort self = (SerialPort) malloc(sizeof(struct sSerialPort));

	if (self != NULL)
	{
		self->fd = -1;
		self->baudRate = baudRate;
		self->dataBits = dataBits;
		self->stopBits = stopBits;
		self->parity = parity;
		self->lastSentTime = 0;
		self->timeout.tv_sec = 0;
		self->timeout.tv_usec = 100000; /* 100 ms */
		strcpy(self->interfaceName, uart_dev[portNo]);
		self->lastError = SERIAL_PORT_ERROR_NONE;
	}

	return self;
}

/**
 * @brief : 串口释放.
 * @param : [结构体指针]
 * @return: [NULL]
 * @updata: [][][]
 */
void SerialPort_destroy(SerialPort self)
{
	if (self != NULL)
	{
		free(self);
	}
}

/**
 * @brief : 设置相关参数，打开串口.
 * @param : [结构体指针]
 * @return: [ture or false]
 * @updata: [][][]
 */
int SerialPort_open(SerialPort self,const uint8_t portNo)
{
	int fd = 0;
	self->fd = open(self->interfaceName, O_RDWR | O_EXCL);
	fd = self->fd;
	uartTofd[portNo] = fd;/*打开串口设备文件后对应fd号*/

	if (self->fd == -1) {
		self->lastError = SERIAL_PORT_ERROR_OPEN_FAILED;
		printf("open function error!\n");
		return -1;
	}

	struct termios tios;
	speed_t baudrate;

	/*获取终端文件描述符fd相关的参数*/
	tcgetattr(self->fd, &tios);

	switch (self->baudRate) {
		case 110:
			baudrate = B110;
			break;
		case 300:
			baudrate = B300;
			break;
		case 600:
			baudrate = B600;
			break;
		case 1200:
			baudrate = B1200;
			break;
		case 2400:
			baudrate = B2400;
			break;
		case 4800:
			baudrate = B4800;
			break;
		case 9600:
			baudrate = B9600;
			break;
		case 19200:
			baudrate = B19200;
			break;
		case 38400:
			baudrate = B38400;
			break;
		case 57600:
			baudrate = B57600;
			break;
		case 115200:
			baudrate = B115200;
			break;
		case 230400:
			baudrate = B230400;
			break;
		default:
			baudrate = B9600;
			self->lastError = SERIAL_PORT_ERROR_INVALID_BAUDRATE;
	}

	/* Set baud rate */
	if ((cfsetispeed(&tios, baudrate) < 0) || (cfsetospeed(&tios, baudrate) < 0)) {
		close(self->fd);
		self->fd = -1;
		self->lastError = SERIAL_PORT_ERROR_INVALID_BAUDRATE;
		return -2;
	}

	tios.c_cflag |= (CREAD | CLOCAL);

	/* Set data bits (5/6/7/8) */
	tios.c_cflag &= ~CSIZE;
	switch (self->dataBits) {
		case 5:
			tios.c_cflag |= CS5;
			break;
		case 6:
			tios.c_cflag |= CS6;
			break;
		case 7:
			tios.c_cflag |= CS7;
			break;
		case 8:
		default:
			tios.c_cflag |= CS8;
			break;
	}

	/* Set stop bits (1/2) */
	if (self->stopBits == 1)
		tios.c_cflag &=~ CSTOPB;
	else /* 2 */
		tios.c_cflag |= CSTOPB;

	/* Set parity bits(N/E/O)
	 * 无校验(N)、偶校验(E)、奇校验(O)
	 * */
	if (self->parity == 'N') {
		tios.c_cflag &=~ PARENB;
	} else if (self->parity == 'E') {
		tios.c_cflag |= PARENB;
		tios.c_cflag &=~ PARODD;
	} else { /* 'O' */
		tios.c_cflag |= PARENB;
		tios.c_cflag |= PARODD;
	}

	tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	if (self->parity == 'N') {
		tios.c_iflag &= ~INPCK;
	} else {
		tios.c_iflag |= INPCK;
	}

	tios.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
	tios.c_iflag |= IGNBRK; /* Set ignore break to allow 0xff characters */
	tios.c_iflag |= IGNPAR;
	tios.c_oflag &=~ OPOST;

	tios.c_cc[VMIN] = 0;
	tios.c_cc[VTIME] = 0;

	/*设置终端参数*/
	if (tcsetattr(self->fd, TCSANOW, &tios) < 0) {
		close(self->fd);
		self->fd = -1;
		self->lastError = SERIAL_PORT_ERROR_INVALID_ARGUMENT;

		return -3;
	}

	return fd;
}

/**
 * @brief : 串口关闭 接口.
 * @param : [结构体指针]
 * @return: [void]
 * @updata: [][][]
 */
void SerialPort_close(SerialPort self)
{
	if (self->fd != -1) {
		close(self->fd);
		self->fd = 0;
	}
}

/**
 * @brief : 获取波特率 接口.
 * @param : [结构体指针]
 * @return: [波特率]
 * @updata: [][][]
 */
int SerialPort_getBaudRate(SerialPort self)
{
	return self->baudRate;
}

/**
 * @brief : 清空输入输出缓冲区 接口.
 * @param : [结构体指针]
 * @return: [void]
 * @updata: [][][]
 */
void SerialPort_discardInBuffer(SerialPort self)
{
	tcflush(self->fd, TCIOFLUSH);
}

/**
 * @brief : 设置超时时间 接口.
 * @param : [结构体指针]
 * @param : [毫秒]
 * @return: [void]
 * @updata: [][][]
 */
void SerialPort_setTimeout(SerialPort self, int timeout)
{
	self->timeout.tv_sec = timeout / 1000;
	self->timeout.tv_usec = (timeout % 1000) * 1000;
}

/**
 * @brief : 参数错误信息 接口.
 * @param : [结构体指针]
 * @return: [void]
 * @updata: [][][]
 */
SerialPortError SerialPort_getLastError(SerialPort self)
{
	return self->lastError;
}

/**
 * @brief : 将串口数据读入buffer 接口.
 * @param : [fd]
 * @param : [pbuf]
 * @param : [count]
 * @return: [数据长度]
 * @updata: [][][]
 */
int SerialPort_readByte(uint8_t fd, uint8_t *pbuf, uint16_t count)
{
	int ret,len = 0;
	fd_set set;
	struct timeval timeout;
	int i = 0;

	FD_ZERO(&set);
	FD_SET(fd, &set);

	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;/*10ms*/
	ret = select(fd + 1, &set, NULL, NULL, &(timeout));

	if ((ret == 0) || ((ret < 0)&&(errno!=EINTR)))
	{
		return 0;
	}
	else
	{
		len = read(fd, pbuf, count);
		/* 通道监听------------------------------------------------ */
		/* 根据fd查找端口号 */
		for (i=0; i<sizeof(uartTofd)/sizeof(int); i++)
		{
			if (uartTofd[i] == fd)
			{
				break;
			}
		}
		/* 监听数据发送 */
		//        send_monitor_data(SERIAL_PORT, i, pbuf, len, RECEIVE_FLAG);

		// log_i("len:%d errno:%d", len,errno);
		// perror("read");

		if ((len < 0) && ((errno == EAGAIN)||(errno == EWOULDBLOCK)))
			return 0;
		if (((len <= 0) || (len > count)) && ((errno!=EINTR)))
		{
			//            log_e("read error");
			return 0;
		}
		else
		{
			return len;
		}
	}
	return 0;
}

/**
 * @brief : 将 buffer数据写入串口 接口.
 * @param : [fd]
 * @param : [buffer]
 * @param : [start]
 * @param : [size]
 * @return: [write返回值]
 * @updata: [][][]
 */
int SerialPort_write(int fd, uint8_t* buffer, int startPos, int bufSize)
{
	//TODO assure minimum line idle time
	ssize_t result;
	int i = 0;
	/* 通道监听------------------------------------------------ */
	/* 根据fd查找端口号 */
	for (i=0; i<sizeof(uartTofd)/sizeof(int); i++)
	{
		if (uartTofd[i] == fd)
		{
			break;
		}
	}
	/* 监听数据发送 */
	//    send_monitor_data(SERIAL_PORT, i, buffer + startPos, bufSize, SEND_FLAG);

again:
	if ((result = write(fd, buffer + startPos, bufSize)) == -1)
	{
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return result;
}


/**
 * @brief : 根据串口号查找设备名
 * @param : [fd]
 * @param : [buffer]
 * @return: [TRUE/FALSE]
 * @updata: [][][]
 */
int portNo_to_name(char *name, uint8_t portNo)
{
	if (portNo >= 0 && portNo <= 3)
	{
		strcpy(name, uart_dev[portNo]);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief : 根据串口号查找fd
 * @param : [portNo]- 串口号
 * @return: [fd]
 * @update: [][][]
 */
int portNo_to_fd(int portNo)
{
	if (portNo >= 0 && portNo <= 3)
	{
		return uartTofd[portNo];
	}
	return -1;
}

/* END OF FILE ----------------------------------------------------------------*/
