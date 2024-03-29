################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/FXOS8700CQ.c \
../source/SysTick.c \
../source/accelerometer.c \
../source/i2c.c \
../source/pinsHandler.c \
../source/spi.c \
../source/uart.c 

OBJS += \
./source/App.o \
./source/FXOS8700CQ.o \
./source/SysTick.o \
./source/accelerometer.o \
./source/i2c.o \
./source/pinsHandler.o \
./source/spi.o \
./source/uart.o 

C_DEPS += \
./source/App.d \
./source/FXOS8700CQ.d \
./source/SysTick.d \
./source/accelerometer.d \
./source/i2c.d \
./source/pinsHandler.d \
./source/spi.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


