################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/temperature_sensor.c \
../src/thermistor_tables.c \
../src/toolhead.c \
../src/toolhead_error.c 

OBJS += \
./src/temperature_sensor.o \
./src/thermistor_tables.o \
./src/toolhead.o \
./src/toolhead_error.o 

C_DEPS += \
./src/temperature_sensor.d \
./src/thermistor_tables.d \
./src/toolhead.d \
./src/toolhead_error.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


