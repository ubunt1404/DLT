#可以用 echo来帮助调试写的 makefile文件

#定义路径变量
SERIAL_DIR =./serial
PORT_DIR =./port
SRC_DIR =./src
SAMPLE_DIR =./sample
OBJ_DIR =./obj

#定义指定头文件路径参数 和 gcc编译参数 及 可执行文件
CC = gcc   
INC = -Iinc -Iserial -Iport
CFLAGS = -g -Wall ${INC}
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
	# -exec 后面接执行命令
	# rm -rf {}命令就是删除文件 
	# \;这是格式要求的没有具体含义。
	# 嵌套执行make命令如下：
	# ${MAKE} -C +目录名(含有makefile文件的子目录)等价于 先cd到 目录名/ 再执行make命令  
	# ${MAKE} clean -C +目录名(含有makefile文件的子目录)等价于 先cd到 目录名/ 再执行make clean 命令 
	find $(OBJ_DIR) -name *.o -exec rm -rf {} \; 
	rm -rf test a.out *.o 
	#${MAKE} clean -C serial
