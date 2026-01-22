cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME firmware)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/real_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/crc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/gpio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/programmer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/uart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/usb_descriptors.c

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/physicalW65C02/physicalw65c02.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/host/hub.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/host/usbh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/synopsys/dwc2/hcd_dwc2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/analog/max3421/hcd_max3421.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/valentyusb/eptri/dcd_eptri.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/ehci/ehci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/bridgetek/ft9xx/dcd_ft9xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nxp/lpc_ip3511/dcd_lpc_ip3511.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nxp/khci/dcd_khci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nxp/khci/hcd_khci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/ohci/ohci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/wch/dcd_ch32_usbhs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/wch/dcd_ch32_usbfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/template/dcd_template.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/template/hcd_template.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/mentor/musb/hcd_musb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/mentor/musb/dcd_musb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/st/typec/typec_stm32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/dialog/da146xx/dcd_da146xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/chipidea/ci_fs/dcd_ci_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/chipidea/ci_hs/hcd_ci_hs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/sunxi/dcd_sunxi_musb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/microchip/samx7x/dcd_samx7x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/microchip/samg/dcd_samg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/microchip/pic/dcd_pic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/microchip/pic32mz/dcd_pic32mz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/microchip/samd/dcd_samd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nuvoton/nuc121/dcd_nuc121.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nuvoton/nuc120/dcd_nuc120.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/nuvoton/nuc505/dcd_nuc505.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/renesas/rusb2/rusb2_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/renesas/rusb2/hcd_rusb2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/renesas/rusb2/dcd_rusb2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/sony/cxd56/dcd_cxd56.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/ti/msp430x5xx/dcd_msp430x5xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/common/tusb_fifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/typec/usbc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/cdc/cdc_rndis_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/cdc/cdc_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/cdc/cdc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/video/video_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/net/ncm_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/net/ecm_rndis_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/dfu/dfu_rt_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/dfu/dfu_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/midi/midi_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/midi/midi_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/usbtmc/usbtmc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/bth/bth_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/audio/audio_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/msc/msc_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/msc/msc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/hid/hid_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/hid/hid_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/vendor/vendor_host.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/class/vendor/vendor_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/device/usbd_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/device/usbd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/tusb.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/physicalW65C02
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/sim6502

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# tinyusb
# include(${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/CMakeLists.txt)
# tinyusb_target_add(${CMAKE_PROJECT_NAME})
#
# target_sources(${CMAKE_PROJECT_NAME} PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
# )
