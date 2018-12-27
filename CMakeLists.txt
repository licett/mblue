cmake_minimum_required(VERSION 3.12)
project(mblue C)

set(CMAKE_C_STANDARD 11)

include_directories(app/scenario)
include_directories(middleware/common)
include_directories(middleware/components/data_schema)
include_directories(middleware/components/event_queue)
include_directories(middleware/components/log)
include_directories(middleware/components/map)
include_directories(middleware/components/message)
include_directories(middleware/components/message_queue)
include_directories(middleware/components/persistence_manager)
include_directories(middleware/components/power)
include_directories(middleware/components/protocal_buffer)
include_directories(middleware/components/ring_buffer)
include_directories(middleware/components/rpc)
include_directories(middleware/components/segment)
include_directories(middleware/components/state_machine)
include_directories(middleware/components/system_bus)
include_directories(middleware/components/task_manager)
include_directories(pal/clock)
include_directories(pal/heap)
include_directories(pal/int)
include_directories(pal/network)
include_directories(pal/semaphore)
include_directories(pal/system)
include_directories(pal/task)
include_directories(pal/timer)
include_directories(service/log)
include_directories(service/system_bus)
include_directories(util/crc)
include_directories(util/hash)

find_package(Threads REQUIRED)
set(SOURCE_FILES
        app/scenario/simple_scenario.c
        app/scenario/simple_scenario.h
        main/main.c
        middleware/common/event.h
        middleware/common/mblue_assert.h
        middleware/common/mblue_comm.h
        middleware/common/mblue_errcode.h
        middleware/common/mblue_list.h
        middleware/common/mblue_logger.h
        middleware/common/mblue_macro.h
        middleware/common/mblue_stddefine.h
        middleware/common/mblue_task.h
        middleware/common/mblue_task_cfg.h
        middleware/common/mblue_task_entry.h
        middleware/components/data_schema/data_schema.c
        middleware/components/data_schema/data_schema.h
        middleware/components/data_schema/mblue.pb.c
        middleware/components/data_schema/mblue.pb.h
        middleware/components/data_schema/pb_schema.c
        middleware/components/data_schema/pb_schema.h
        middleware/components/event_queue/event_helper.c
        middleware/components/event_queue/event_helper.h
        middleware/components/event_queue/event_queue.c
        middleware/components/event_queue/event_queue.h
        middleware/components/log/log.c
        middleware/components/log/log.h
        middleware/components/log/custom_log.h
        middleware/components/message/mblue_message.c
        middleware/components/message/mblue_message.h
        middleware/components/message/message_def.h
        middleware/components/message_queue/message_queue.c
        middleware/components/message_queue/message_queue.h
        middleware/components/protocal_buffer/pb.h
        middleware/components/protocal_buffer/pb_common.c
        middleware/components/protocal_buffer/pb_common.h
        middleware/components/protocal_buffer/pb_decode.c
        middleware/components/protocal_buffer/pb_decode.h
        middleware/components/protocal_buffer/pb_encode.c
        middleware/components/protocal_buffer/pb_encode.h
        middleware/components/ring_buffer/mblue_fifo.c
        middleware/components/ring_buffer/mblue_fifo.h
        middleware/components/rpc/mblue_global_rpc_method.c
        middleware/components/rpc/mblue_global_rpc_method.h
        middleware/components/rpc/rpc.c
        middleware/components/rpc/rpc.h
        middleware/components/rpc/rpc_helper.c
        middleware/components/rpc/rpc_helper.h
        middleware/components/segment/mblue_segment.c
        middleware/components/segment/mblue_segment.h
        middleware/components/system_bus/system_bus.c
        middleware/components/system_bus/system_bus.h
        middleware/components/task_manager/task_manager.c
        middleware/components/task_manager/task_manager.h
        pal/clock/custom_clock.h
        pal/clock/mblue_clock.c
        pal/clock/mblue_clock.h
        pal/heap/custom_heap.h
        pal/heap/mblue_heap.c
        pal/heap/mblue_heap.h
        pal/int/custom_int.c
        pal/int/custom_int.h
        pal/int/custom_int_mt.h
        pal/int/custom_int_ti.h
        pal/semaphore/custom_semaphore.h
        pal/semaphore/custom_semaphore_type.h
        pal/semaphore/mblue_semaphore.c
        pal/semaphore/mblue_semaphore.h
        pal/system/custom_system.h
        pal/task/custom_task.h
        pal/task/custom_task_pthread.h
        pal/task/mblue_stdtask.c
        pal/task/mblue_stdtask.h
        pal/timer/custom_timer.h
        pal/timer/mblue_timer.c
        pal/timer/mblue_timer.h
        service/log/log_service.c
        service/log/log_service.h
        service/system_bus/system_bus_service.c
        service/system_bus/system_bus_service.h
        util/crc/mblue_crc.c
        util/crc/mblue_crc.h
        util/hash/mblue_hash.c
        util/hash/mblue_hash.h)

IF( NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE Release ... FORCE )
ENDIF()

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

add_executable(mblue ${SOURCE_FILES})
target_link_libraries(mblue pthread rt)
print_all_variables()
#target_compile_options(mblue
#	PRIVATE -g
#)
