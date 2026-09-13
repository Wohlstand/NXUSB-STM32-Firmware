#############################################################################################################################
# file:  CMakeLists.txt
# brief: Template "CMakeLists.txt" for building of executables and static libraries.
#
# usage: Edit "VARIABLES"-section to suit project requirements.
#        For debug build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Debug -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
#          make -C Debug VERBOSE=1 -j
#        For release build:
#          cmake -DCMAKE_TOOLCHAIN_FILE=cubeide-gcc.cmake  -S ./ -B Release -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#          make -C Release VERBOSE=1 -j
#############################################################################################################################
cmake_minimum_required(VERSION 3.20)

###################### CONSTANTS ######################################
set(PROJECT_TYPE_EXECUTABLE          "exe")
set(PROJECT_TYPE_STATIC_LIBRARY      "static-lib")
set(MCPU_CORTEX_M0                   "-mcpu=cortex-m0")
set(MCPU_CORTEX_M0PLUS               "-mcpu=cortex-m0plus")
set(MCPU_CORTEX_M3                   "-mcpu=cortex-m3")
set(MCPU_CORTEX_M4                   "-mcpu=cortex-m4")
set(MCPU_CORTEX_M7                   "-mcpu=cortex-m7")
set(MCPU_CORTEX_M33                  "-mcpu=cortex-m33")
set(MCPU_CORTEX_M55                  "-mcpu=cortex-m55")
set(MCPU_CORTEX_M85                  "-mcpu=cortex-m85")
set(MFPU_FPV4_SP_D16                 "-mfpu=fpv4-sp-d16")
set(MFPU_FPV4                        "-mfpu=vfpv4")
set(MFPU_FPV5_D16                    "-mfpu=fpv5-d16")
set(RUNTIME_LIBRARY_REDUCED_C        "--specs=nano.specs")
set(RUNTIME_LIBRARY_STD_C            "")
set(RUNTIME_LIBRARY_SYSCALLS_MINIMAL "--specs=nosys.specs")
set(RUNTIME_LIBRARY_SYSCALLS_NONE    "")
set(MFLOAT_ABI_SOFTWARE              "-mfloat-abi=soft")
set(MFLOAT_ABI_HARDWARE              "-mfloat-abi=hard")
set(MFLOAT_ABI_MIX                   "-mfloat-abi=softfp")
#######################################################################

###################### VARIABLES ######################################
set(PROJECT_NAME             "STM32-ProController")
set(PROJECT_TYPE             "exe")
set(LINKER_SCRIPT            "../STM32F103C8TX_FLASH.ld")
set(MCPU                     "-mcpu=Cortex-M3")
set(MFLOAT_ABI               "")
set(RUNTIME_LIBRARY          "--specs=nano.specs")
set(RUNTIME_LIBRARY_SYSCALLS "--specs=nosys.specs")


set(PROJECT_SOURCES
    Startup/startup_stm32f103c8tx.s
    Sources/main.c
    Sources/syscalls.c
    Sources/sysmem.c
    Sources/usb/usb.c
    Sources/usb/usb_extra.c
    Sources/usb/usb_descriptors.c
    Sources/switch/switch_cb.c
    Sources/usart.c
    Sources/gpio.c
    Sources/debug_uart.c

    Sources/hal/stm32f1xx_hal_msp.c
    Sources/hal/stm32f1xx_it.c
    Sources/hal/system_stm32f1xx.c

    Sources/usb/tusb_dev/fsdev_common.c
    Sources/usb/tusb_dev/dcd_stm32_fsdev.c

    # TinyUSB hooks
    3rdparty/tinyusb/src/tusb.c
    3rdparty/tinyusb/src/common/tusb_fifo.c
    3rdparty/tinyusb/src/device/usbd.c
    3rdparty/tinyusb/src/class/hid/hid_device.c

    # HAL drivers
    3rdparty/hal/Src/stm32f1xx_hal.c
    3rdparty/hal/Src/stm32f1xx_hal_cortex.c
    3rdparty/hal/Src/stm32f1xx_hal_dma.c
    3rdparty/hal/Src/stm32f1xx_hal_dac_ex.c
    3rdparty/hal/Src/stm32f1xx_hal_exti.c
    3rdparty/hal/Src/stm32f1xx_hal_flash.c
    3rdparty/hal/Src/stm32f1xx_hal_flash_ex.c
    3rdparty/hal/Src/stm32f1xx_hal_gpio.c
    3rdparty/hal/Src/stm32f1xx_hal_pcd.c
    3rdparty/hal/Src/stm32f1xx_hal_pcd_ex.c
    3rdparty/hal/Src/stm32f1xx_hal_pwr.c
    3rdparty/hal/Src/stm32f1xx_hal_rcc.c
    3rdparty/hal/Src/stm32f1xx_hal_rcc_ex.c
    3rdparty/hal/Src/stm32f1xx_hal_uart.c
    3rdparty/hal/Src/stm32f1xx_hal_usart.c
    3rdparty/hal/Src/stm32f1xx_ll_usb.c
)

set(PROJECT_DEFINES
    -DSTM32F1
    -DSTM32F103xB
    -DSTM32F10X_MD
    -DDEBUG
    # -DHSE_VALUE=8000000U
    -DCFG_TUSB_DEBUG=0
    -DCFG_TUSB_FSDEV_PMA_SIZE=512u
    -DCFG_TUSB_MCU=OPT_MCU_STM32F1
    -DCFG_TUSB_DEBUG_PRINTF=mine_debug_print
)

set(PROJECT_INCLUDES
    include/cm
    include/Fx
    Sources
    Sources/configs
    3rdparty/tinyusb/src
    3rdparty/tinyusb/hw
    # 3rdparty/tinyusb/hw/bsp/stm32f1/boards/stm32f103_bluepill
    3rdparty/hal/Inc
)

############ MODIFY ACCORDING TO REQUIREMENTS) ########################

#######################################################################

################## PROJECT SETUP ######################################
project(${PROJECT_NAME})
enable_language(ASM)

if(${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_EXECUTABLE})
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
elseif (${PROJECT_TYPE} MATCHES ${PROJECT_TYPE_STATIC_LIBRARY})
    add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

add_compile_definitions (${PROJECT_DEFINES})
include_directories (${PROJECT_INCLUDES})

set (CMAKE_EXECUTABLE_SUFFIX ".elf")
set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set (CMAKE_C_FLAGS "${MCPU} -Og -std=gnu11 ${MFPU} ${MFLOAT_ABI} ${RUNTIME_LIBRARY} -mthumb -Wall -Werror")
set (CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -Wl,-Map=test.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
