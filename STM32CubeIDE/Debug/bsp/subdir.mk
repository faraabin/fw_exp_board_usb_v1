################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/bsp/bsp.c 

OBJS += \
./bsp/bsp.o 

C_DEPS += \
./bsp/bsp.d 


# Each subdirectory must supply rules for building sources it contributes
bsp/bsp.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/bsp/bsp.c bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-bsp

clean-bsp:
	-$(RM) ./bsp/bsp.cyclo ./bsp/bsp.d ./bsp/bsp.o ./bsp/bsp.su

.PHONY: clean-bsp

