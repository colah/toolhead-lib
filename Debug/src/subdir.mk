################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/heater.c \
../src/solenoid.c \
../src/temperature_sensor.c \
../src/toolhead.c \
../src/toolhead_error.c 

OBJS += \
./src/heater.o \
./src/solenoid.o \
./src/temperature_sensor.o \
./src/toolhead.o \
./src/toolhead_error.o 

C_DEPS += \
./src/heater.d \
./src/solenoid.d \
./src/temperature_sensor.d \
./src/toolhead.d \
./src/toolhead_error.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


