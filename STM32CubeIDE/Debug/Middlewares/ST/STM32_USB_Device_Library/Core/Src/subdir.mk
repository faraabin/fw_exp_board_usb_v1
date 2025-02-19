################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 

OBJS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Core-2f-Src

clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Core-2f-Src:
	-$(RM) ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.cyclo ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.d ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.o ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.su ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.cyclo ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.d ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.o ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.su ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.cyclo ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.d ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.o ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.su

.PHONY: clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Core-2f-Src

