################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_default_fobjects_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_databus_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_eventgroup_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_function_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_var_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_vartype_test.c \
C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/test_function_fn.c 

OBJS += \
./faraabin/test/faraabin_default_fobjects_test.o \
./faraabin/test/faraabin_fobject_databus_test.o \
./faraabin/test/faraabin_fobject_eventgroup_test.o \
./faraabin/test/faraabin_fobject_function_test.o \
./faraabin/test/faraabin_fobject_var_test.o \
./faraabin/test/faraabin_fobject_vartype_test.o \
./faraabin/test/test_function_fn.o 

C_DEPS += \
./faraabin/test/faraabin_default_fobjects_test.d \
./faraabin/test/faraabin_fobject_databus_test.d \
./faraabin/test/faraabin_fobject_eventgroup_test.d \
./faraabin/test/faraabin_fobject_function_test.d \
./faraabin/test/faraabin_fobject_var_test.d \
./faraabin/test/faraabin_fobject_vartype_test.d \
./faraabin/test/test_function_fn.d 


# Each subdirectory must supply rules for building sources it contributes
faraabin/test/faraabin_default_fobjects_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_default_fobjects_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/faraabin_fobject_databus_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_databus_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/faraabin_fobject_eventgroup_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_eventgroup_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/faraabin_fobject_function_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_function_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/faraabin_fobject_var_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_var_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/faraabin_fobject_vartype_test.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/faraabin_fobject_vartype_test.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
faraabin/test/test_function_fn.o: C:/Users/ASUS/Desktop/fw_exp_board_usb_v1/faraabin/test/test_function_fn.c faraabin/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/DSP/Include -I../../chrono -I../../faraabin -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-faraabin-2f-test

clean-faraabin-2f-test:
	-$(RM) ./faraabin/test/faraabin_default_fobjects_test.cyclo ./faraabin/test/faraabin_default_fobjects_test.d ./faraabin/test/faraabin_default_fobjects_test.o ./faraabin/test/faraabin_default_fobjects_test.su ./faraabin/test/faraabin_fobject_databus_test.cyclo ./faraabin/test/faraabin_fobject_databus_test.d ./faraabin/test/faraabin_fobject_databus_test.o ./faraabin/test/faraabin_fobject_databus_test.su ./faraabin/test/faraabin_fobject_eventgroup_test.cyclo ./faraabin/test/faraabin_fobject_eventgroup_test.d ./faraabin/test/faraabin_fobject_eventgroup_test.o ./faraabin/test/faraabin_fobject_eventgroup_test.su ./faraabin/test/faraabin_fobject_function_test.cyclo ./faraabin/test/faraabin_fobject_function_test.d ./faraabin/test/faraabin_fobject_function_test.o ./faraabin/test/faraabin_fobject_function_test.su ./faraabin/test/faraabin_fobject_var_test.cyclo ./faraabin/test/faraabin_fobject_var_test.d ./faraabin/test/faraabin_fobject_var_test.o ./faraabin/test/faraabin_fobject_var_test.su ./faraabin/test/faraabin_fobject_vartype_test.cyclo ./faraabin/test/faraabin_fobject_vartype_test.d ./faraabin/test/faraabin_fobject_vartype_test.o ./faraabin/test/faraabin_fobject_vartype_test.su ./faraabin/test/test_function_fn.cyclo ./faraabin/test/test_function_fn.d ./faraabin/test/test_function_fn.o ./faraabin/test/test_function_fn.su

.PHONY: clean-faraabin-2f-test

