################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/test/heater_test.c \
../src/test/temperature_sensor_test.c \
../src/test/toolhead_test.c 

OBJS += \
./src/test/heater_test.o \
./src/test/temperature_sensor_test.o \
./src/test/toolhead_test.o 

C_DEPS += \
./src/test/heater_test.d \
./src/test/temperature_sensor_test.d \
./src/test/toolhead_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/test/%.o: ../src/test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


