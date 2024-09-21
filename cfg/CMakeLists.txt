cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_TOOLCHAIN_FILE toolchain_stm32f4.cmake)

set(ProjectName "bootloader")

project(${ProjectName} C ASM)

add_compile_definitions(USE_HAL_DRIVER STM32F429xx)

set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set_property(GLOBAL PROPERTY C_STANDARD 11)

add_compile_options(
    -O0
    -g3
    -fdata-sections
    -ffunction-sections
    -Wno-format
)

add_executable( ${ProjectName}
    "../src/core/startup_stm32f429xx.s"
    "../src/core/system_stm32f4xx.c"
    "../src/core/main.c"
    "../src/board.c"
    "../src/errorHandlers.c"
    "../src/gpio_ctrl.c"
    "../src/rcc_ctrl.c"
    "../src/retarget.c"
    "../src/terminal.c"
    "../src/timer_ctrl.c"
    "../src/uart_ctrl.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"
    "../import/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c" 

)

include_directories(
    "../inc"
    "../src/core"
    "../import/STM32F4xx_HAL_Driver/Inc"
    "../import/CMSIS/Include"
    "../import/CMSIS/Device/ST/STM32F4xx/Include"
)

set(LD_FILE ${CMAKE_SOURCE_DIR}/../cfg/STM32F429ZITx_FLASH.ld)

target_link_options(${ProjectName} PUBLIC
    -T ${CMAKE_BINARY_DIR}/${ProjectName}.ld
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,-Map,${CMAKE_BINARY_DIR}/${ProjectName}.map
    -Wl,--no-warn-rwx-segment
)

set_target_properties(${ProjectName} PROPERTIES SUFFIX ".elf")


add_custom_command(TARGET ${ProjectName} PRE_LINK
    COMMAND ${CMAKE_C_COMPILER}
    ARGS -E -P -x c -I${CMAKE_SOURCE_DIR} ${LD_FILE} >${CMAKE_BINARY_DIR}/${ProjectName}.ld
    DEPENDS ${LD_FILE}
)

add_custom_target(${ProjectName}-binary ALL DEPENDS ${ProjectName})
add_custom_command(TARGET ${ProjectName}-binary POST_BUILD
                   COMMAND ${CMAKE_OBJCOPY}
                   ARGS --gap-fill 0xFF -O ihex ${CMAKE_BINARY_DIR}/${ProjectName}.elf ${CMAKE_BINARY_DIR}/${ProjectName}.hex
                   DEPENDS ${ProjectName}
)                   

add_custom_target(${ProjectName}-size ALL DEPENDS ${ProjectName})
add_custom_command(TARGET ${ProjectName}-size POST_BUILD 
                   COMMAND ${CMAKE_SIZE} ${ProjectName}.elf
)