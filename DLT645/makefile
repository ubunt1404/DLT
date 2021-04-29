
#定义路径变量
SERIAL_DIR =./serial
PORT_DIR =./port
SRC_DIR =./src
SAMPLE_DIR =./sample
OBJ_DIR =./obj

#定义指定头文件路径参数 和 gcc编译参数 及 可执行文件
CC = gcc   
INC = -Iinc -Iserial -Iport
CFLAGS = ${INC}
BIN_TARGET = test

# 获取要求中工作目录下的所有的.c文件列表
SRC =$(wildcard ${SERIAL_DIR}/*.c)\
	 $(wildcard ${PORT_DIR}/*.c)\
	 $(wildcard ${SRC_DIR}/*.c)\
	 $(wildcard ${SAMPLE_DIR}/*.c)

# 通过替换获取所有目标文件名并放在 obj/目录下  
OBJECT =$(patsubst %.c,${OBJ_DIR}/%.o,$(notdir ${SRC}))  

#可以用 echo来帮助调试写的 makefile文件
#all:
	#@echo "start debug!"
	#@echo ${SRC}
	#@echo ${OBJECT}  

#用 *.o生成可执行文件test，其中可用 $^替换掉 ${OBJECT}  
${BIN_TARGET}:${OBJECT}  
	$(CC) $(CFLAGS) -o $@ ${OBJECT} -lm 

#在相应目录下依次将对应源文件生成对应目标文件  
${OBJ_DIR}/%.o:${SERIAL_DIR}/%.c 
	$(CC) $(CFLAGS) -c $< -o $@ 
${OBJ_DIR}/%.o:${PORT_DIR}/%.c   
	$(CC) $(CFLAGS) -c $< -o $@ 
${OBJ_DIR}/%.o:${SRC_DIR}/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
${OBJ_DIR}/%.o:${SAMPLE_DIR}/%.c 
	$(CC) $(CFLAGS) -c $< -o $@ 

#删除文件
.PHONY:clean  
clean:  
	find $(OBJ_DIR) -name *.o -exec rm -rf {} \; 
	rm -rf test a.out *.o 
