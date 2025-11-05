################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/ioLibrary_Driver/Ethernet/W6300/w6300.c 

OBJS += \
./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.o 

C_DEPS += \
./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/ioLibrary_Driver/Ethernet/W6300/%.o libraries/ioLibrary_Driver/Ethernet/W6300/%.su libraries/ioLibrary_Driver/Ethernet/W6300/%.cyclo: ../libraries/ioLibrary_Driver/Ethernet/W6300/%.c libraries/ioLibrary_Driver/Ethernet/W6300/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F412Zx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/ST_Projects/STM32F412ZGT6_W6300_Loopback/libraries/ioLibrary_Driver/Application/loopback" -I"D:/ST_Projects/STM32F412ZGT6_W6300_Loopback/libraries/ioLibrary_Driver/Ethernet/W6300" -I"D:/ST_Projects/STM32F412ZGT6_W6300_Loopback/libraries/ioLibrary_Driver/Internet/DHCP" -I"D:/ST_Projects/STM32F412ZGT6_W6300_Loopback/libraries/ioLibrary_Driver/Ethernet" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libraries-2f-ioLibrary_Driver-2f-Ethernet-2f-W6300

clean-libraries-2f-ioLibrary_Driver-2f-Ethernet-2f-W6300:
	-$(RM) ./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.cyclo ./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.d ./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.o ./libraries/ioLibrary_Driver/Ethernet/W6300/w6300.su

.PHONY: clean-libraries-2f-ioLibrary_Driver-2f-Ethernet-2f-W6300

