################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_database.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_default_fobjects.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_container.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_databus.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_eventgroup.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_function.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_mcu.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_vartype.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_function_engine.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_buffer.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_deserializer.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_handler.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_serializer.c 

OBJS += \
./faraabin/faraabin.o \
./faraabin/faraabin_database.o \
./faraabin/faraabin_default_fobjects.o \
./faraabin/faraabin_fobject_container.o \
./faraabin/faraabin_fobject_databus.o \
./faraabin/faraabin_fobject_eventgroup.o \
./faraabin/faraabin_fobject_function.o \
./faraabin/faraabin_fobject_mcu.o \
./faraabin/faraabin_fobject_vartype.o \
./faraabin/faraabin_function_engine.o \
./faraabin/faraabin_link_buffer.o \
./faraabin/faraabin_link_deserializer.o \
./faraabin/faraabin_link_handler.o \
./faraabin/faraabin_link_serializer.o 

C_DEPS += \
./faraabin/faraabin.d \
./faraabin/faraabin_database.d \
./faraabin/faraabin_default_fobjects.d \
./faraabin/faraabin_fobject_container.d \
./faraabin/faraabin_fobject_databus.d \
./faraabin/faraabin_fobject_eventgroup.d \
./faraabin/faraabin_fobject_function.d \
./faraabin/faraabin_fobject_mcu.d \
./faraabin/faraabin_fobject_vartype.d \
./faraabin/faraabin_function_engine.d \
./faraabin/faraabin_link_buffer.d \
./faraabin/faraabin_link_deserializer.d \
./faraabin/faraabin_link_handler.d \
./faraabin/faraabin_link_serializer.d 


# Each subdirectory must supply rules for building sources it contributes
faraabin/faraabin.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_database.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_database.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_default_fobjects.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_default_fobjects.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_container.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_container.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_databus.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_databus.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_eventgroup.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_eventgroup.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_function.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_function.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_mcu.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_mcu.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_fobject_vartype.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_fobject_vartype.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_function_engine.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_function_engine.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_link_buffer.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_buffer.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_link_deserializer.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_deserializer.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_link_handler.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_handler.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/faraabin_link_serializer.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/src/faraabin/faraabin_link_serializer.c faraabin/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../src/Core/Inc -I../../src/Drivers/CMSIS/Include -I../../src/Drivers/CMSIS/DSP/Include -I../../src/chrono -I../../src/faraabin -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../src/Drivers/STM32F1xx_HAL_Driver/Inc -I../../src/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../src/USB_DEVICE/App -I../../src/USB_DEVICE/Target -I../../src/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../src/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../src/bsp -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-faraabin

clean-faraabin:
	-$(RM) ./faraabin/faraabin.cyclo ./faraabin/faraabin.d ./faraabin/faraabin.o ./faraabin/faraabin.su ./faraabin/faraabin_database.cyclo ./faraabin/faraabin_database.d ./faraabin/faraabin_database.o ./faraabin/faraabin_database.su ./faraabin/faraabin_default_fobjects.cyclo ./faraabin/faraabin_default_fobjects.d ./faraabin/faraabin_default_fobjects.o ./faraabin/faraabin_default_fobjects.su ./faraabin/faraabin_fobject_container.cyclo ./faraabin/faraabin_fobject_container.d ./faraabin/faraabin_fobject_container.o ./faraabin/faraabin_fobject_container.su ./faraabin/faraabin_fobject_databus.cyclo ./faraabin/faraabin_fobject_databus.d ./faraabin/faraabin_fobject_databus.o ./faraabin/faraabin_fobject_databus.su ./faraabin/faraabin_fobject_eventgroup.cyclo ./faraabin/faraabin_fobject_eventgroup.d ./faraabin/faraabin_fobject_eventgroup.o ./faraabin/faraabin_fobject_eventgroup.su ./faraabin/faraabin_fobject_function.cyclo ./faraabin/faraabin_fobject_function.d ./faraabin/faraabin_fobject_function.o ./faraabin/faraabin_fobject_function.su ./faraabin/faraabin_fobject_mcu.cyclo ./faraabin/faraabin_fobject_mcu.d ./faraabin/faraabin_fobject_mcu.o ./faraabin/faraabin_fobject_mcu.su ./faraabin/faraabin_fobject_vartype.cyclo ./faraabin/faraabin_fobject_vartype.d ./faraabin/faraabin_fobject_vartype.o ./faraabin/faraabin_fobject_vartype.su ./faraabin/faraabin_function_engine.cyclo ./faraabin/faraabin_function_engine.d ./faraabin/faraabin_function_engine.o ./faraabin/faraabin_function_engine.su ./faraabin/faraabin_link_buffer.cyclo ./faraabin/faraabin_link_buffer.d ./faraabin/faraabin_link_buffer.o ./faraabin/faraabin_link_buffer.su ./faraabin/faraabin_link_deserializer.cyclo ./faraabin/faraabin_link_deserializer.d ./faraabin/faraabin_link_deserializer.o ./faraabin/faraabin_link_deserializer.su ./faraabin/faraabin_link_handler.cyclo ./faraabin/faraabin_link_handler.d ./faraabin/faraabin_link_handler.o ./faraabin/faraabin_link_handler.su ./faraabin/faraabin_link_serializer.cyclo ./faraabin/faraabin_link_serializer.d ./faraabin/faraabin_link_serializer.o ./faraabin/faraabin_link_serializer.su

.PHONY: clean-faraabin

