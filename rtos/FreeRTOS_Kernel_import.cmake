
if (DEFINED ENV{FREERTOS_KERNEL_PATH} AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
    message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
endif ()

set(FREERTOS_SRC
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/event_groups.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_SOURCE_DIR}/rtos/IdleMemory.c
)

add_library(FreeRTOS-Kernel STATIC ${FREERTOS_SRC})

target_include_directories(FreeRTOS-Kernel PUBLIC
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/rtos
)

target_link_libraries(FreeRTOS-Kernel
	stm32cubemx
    
    # Add user defined libraries
)
