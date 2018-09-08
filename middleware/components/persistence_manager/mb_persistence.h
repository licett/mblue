/*
 * =====================================================================================
 *
 *       Filename:  mb_logger.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:   2017年3月24日 下午4:44:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hanxu
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef FRAMEWORK_LOGGING_MB_LOGGER_H_
#define FRAMEWORK_LOGGING_MB_LOGGER_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ERASE_SIZE 0x20000 // 128k

// 目前可用的范围 sector3,4,5 
struct data_section_info {
        size_t start_addr; // 日志存储的起始地址
        // size_t space_size; // 日志系统的分配的内存大小
        size_t end_addr; // 日志存储的结束地址
        size_t cur_write_addr; // 当前日志写入的最后地址
        size_t cur_read_addr; // 当前读日志的最后地址
        size_t data_start_addr; // 日志有效开始地址
        size_t data_end_addr; // 日志有效结束地址
        size_t erase_addr; // 擦除到的地址  
        // size_t erase_size; // 擦除的块大小
        size_t magic;
        size_t valid;// head是否有效
};

struct mb_logger *get_log_instance();

struct mb_logger {

        int (* init)(struct mb_logger *lg);
        int (* write)(struct mb_logger *lg, uint8_t *buffer, uint32_t len);
        int (* read)(struct mb_logger *lg, uint8_t *buffer, uint32_t len);
        int (* read_next)(struct mb_logger *lg, uint8_t *buffer, uint32_t len);
        int (* clear)(struct mb_logger *lg);
        struct mb_persistence *opr;
        struct data_section_info config;
        uint32_t head_index;
};

typedef int (*write)(uint32_t addr, size_t len, uint8_t *buffer);
typedef int (*read)(uint32_t addr, size_t len, uint8_t *buffer);
typedef int (*erase)(uint32_t addr, size_t len);

struct mb_persistence {

        int (*init)(struct mb_persistence *lg, struct data_section_info *section,
                    write write_cb, read read_cb, erase erase_cb);
        int (*write)(struct mb_persistence *lg, uint8_t *buffer, size_t len);
        int (*read)(struct mb_persistence *lg, uint8_t *buffer, size_t len);
        int (*clear)(struct mb_persistence *lg);

        struct data_section_info *section_data;
        write write_cb;
        read read_cb;
        erase erase_cb;
};

#endif /* FRAMEWORK_LOGGING_MB_LOGGER_H_ */
