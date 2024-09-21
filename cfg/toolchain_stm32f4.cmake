set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb -mlittle-endian ")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m4 -mthumb -mlittle-endian " )
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m4 -mthumb -mlittle-endian -x assembler-with-cpp " )
set(CMAKE_LINK_FLAGS "-mcpu=cortex-m4 -mthumb -mlittle-endian ")


set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
