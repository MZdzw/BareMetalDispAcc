cmake_minimum_required(VERSION 3.15.3)

project(BareMetalDispAcc)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(STM32_GENERATED_FILES

        Src/gpioCL.cpp
        Src/i2cCL.cpp
        Src/ioBaseCL.cpp
        Src/LIS3DH.cpp
        Src/main.cpp
        Src/rccCL.cpp
        Src/syscalls.c
        Src/sysmem.c
        Src/fonts.c
        Src/timCL.cpp
        Src/usartCL.cpp
        Src/st7735s.cpp
        Src/spiCL.cpp

        Startup/startup_stm32f103c8tx.s)

set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${STM32_GENERATED_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DSTM32
        -DSTM32F1
        -DSTM32F103C8Tx
        )

target_include_directories(${EXECUTABLE} PRIVATE
        Drivers/CMSIS/
        Inc/
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m3
        -mthumb

        -fdata-sections
        -ffunction-sections

        -Wall

        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/STM32F103C8TX_FLASH.ld
        -mcpu=cortex-m3
        -mthumb

        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)

