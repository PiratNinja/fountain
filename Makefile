
# инструменты, необходимые для сборки
CC      = arm-none-eabi-gcc
#LD      = arm-none-eabi-gcc
LD      = arm-none-eabi-ld
AR      = arm-none-eabi-ar
AS      = arm-none-eabi-as
CP      = arm-none-eabi-objcopy
OD		= arm-none-eabi-objdump
SZ      = arm-none-eabi-size

HAL = ./Drivers/STM32F1xx_HAL_Driver/
HAL_INC = $(HAL)Inc/
HAL_SRC = $(HAL)Src/

CMSIS = ./Drivers/CMSIS/
CMSIS_INC = $(CMSIS)/Include/

DEV = $(CMSIS)Device/ST/STM32F1xx/
DEV_INC = $(DEV)Include/

TMPL = $(DEV)Source/Templates/


DEF = -DSTM32F100xB
CPU = cortex-m3
LD_CMD = "./SW4STM32/fw Configuration/STM32F100C6Tx_FLASH.ld"

CFLAGS  = $(DEF) -Wall -std=c99 -O0 -g -mcpu=$(CPU) -mthumb -fno-common
AFLAGS  = -ahls -mapcs-32
LFLAGS  = -v -nostartfiles --gc-sections -T $(LD_CMD)
CPFLAGS = -Obinary
ODFLAGS	= -S

all: main

clean:
	-rm -f *.o startup*.list main.bin main.list main.elf

main: main.elf size
	@$(CP) $(CPFLAGS) main.elf main.bin
	@$(OD) $(ODFLAGS) main.elf > main.list
	
# ���������� ������
#-------------------------------------------------------------------------------
size:
	@echo "---------------------------------------------------"
	@$(SZ) main.elf

main.elf: startup.o system.o main.o stm32f1xx_hal_msp.o stm32f1xx_it.o stm32f1xx_hal.o stm32f1xx_hal_rcc.o stm32f1xx_hal_cortex.o stm32f1xx_hal_gpio.o stm32f1xx_hal_uart.o stm32f1xx_hal_rcc_ex.o stm32f1xx_hal_dma.o stm32f1xx_hal_tim.o stm32f1xx_hal_tim_ex.o cmd.o
	$(LD) $(LFLAGS) -o main.elf startup.o system.o main.o stm32f1xx_hal_msp.o stm32f1xx_it.o stm32f1xx_hal.o stm32f1xx_hal_rcc.o stm32f1xx_hal_cortex.o stm32f1xx_hal_gpio.o stm32f1xx_hal_uart.o stm32f1xx_hal_rcc_ex.o stm32f1xx_hal_dma.o stm32f1xx_hal_tim.o stm32f1xx_hal_tim_ex.o cmd.o 

startup.o: $(TMPL)gcc/startup_stm32f100xb.s
	$(AS) $(AFLAGS) $(TMPL)gcc/startup_stm32f100xb.s -o startup.o > startup.list

system.o: $(TMPL)system_stm32f1xx.c
	$(CC) $(CFLAGS) -I$(DEV_INC) -I$(CMSIS_INC) -c $(TMPL)system_stm32f1xx.c -o system.o

main.o: ./Src/main.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c ./Src/main.c -o main.o

stm32f1xx_hal_msp.o: ./Src/stm32f1xx_hal_msp.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c ./Src/stm32f1xx_hal_msp.c -o stm32f1xx_hal_msp.o

stm32f1xx_it.o: ./Src/stm32f1xx_it.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c ./Src/stm32f1xx_it.c -o stm32f1xx_it.o

stm32f1xx_hal.o: $(HAL_SRC)stm32f1xx_hal.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal.c -o stm32f1xx_hal.o

stm32f1xx_hal_rcc.o: $(HAL_SRC)stm32f1xx_hal_rcc.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_rcc.c -o stm32f1xx_hal_rcc.o

stm32f1xx_hal_cortex.o: $(HAL_SRC)stm32f1xx_hal_cortex.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_cortex.c -o stm32f1xx_hal_cortex.o

stm32f1xx_hal_gpio.o: $(HAL_SRC)stm32f1xx_hal_gpio.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_gpio.c -o stm32f1xx_hal_gpio.o

stm32f1xx_hal_uart.o: $(HAL_SRC)stm32f1xx_hal_uart.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_uart.c -o stm32f1xx_hal_uart.o

stm32f1xx_hal_rcc_ex.o: $(HAL_SRC)stm32f1xx_hal_rcc_ex.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_rcc_ex.c -o stm32f1xx_hal_rcc_ex.o

stm32f1xx_hal_dma.o: $(HAL_SRC)stm32f1xx_hal_dma.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_dma.c -o stm32f1xx_hal_dma.o

stm32f1xx_hal_tim.o: $(HAL_SRC)stm32f1xx_hal_tim.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_tim.c -o stm32f1xx_hal_tim.o

stm32f1xx_hal_tim_ex.o: $(HAL_SRC)stm32f1xx_hal_tim_ex.c	
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./Inc -c $(HAL_SRC)stm32f1xx_hal_tim_ex.c -o stm32f1xx_hal_tim_ex.o
	
cmd.o: ./dmx/src/cmd.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./dmx/inc -I./Inc -c ./dmx/src/cmd.c -o cmd.o
	
bulb.o: ./dmx/src/bulb.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./dmx/inc -I./Inc -c ./dmx/src/bulb.c -o bulb.o

queue.o: ./dmx/src/queue.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./dmx/inc -I./Inc -c ./dmx/src/queue.c -o queue.o
	
ticker.o: ./dmx/src/ticker.c
	$(CC) $(CFLAGS) -I$(HAL_INC) -I$(DEV_INC) -I$(CMSIS_INC) -I./dmx/inc -I./Inc -c ./dmx/src/ticker.c -o ticker.o