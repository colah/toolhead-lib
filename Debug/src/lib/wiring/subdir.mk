################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/wiring/pins_arduino.c \
../src/lib/wiring/wiring_analog.c \
../src/lib/wiring/wiring_digital.c 

OBJS += \
./src/lib/wiring/pins_arduino.o \
./src/lib/wiring/wiring_analog.o \
./src/lib/wiring/wiring_digital.o 

C_DEPS += \
./src/lib/wiring/pins_arduino.d \
./src/lib/wiring/wiring_analog.d \
./src/lib/wiring/wiring_digital.d 


# Each subdirectory must supply rules for building sources it contributes
src/lib/wiring/%.o: ../src/lib/wiring/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


