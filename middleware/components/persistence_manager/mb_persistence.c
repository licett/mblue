/*
 * =====================================================================================
 *
 *       Filename:  mb_logger.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:   2017年3月24日 下午4:50:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hanxu
 *   Organization:
 *
 * =====================================================================================
 */

#include <string.h>
#include "mb_persistence.h"
#include "mb_flash.h"
#include "mblue_assert.h"
#include "mblue_heap.h"
// #include "log_util.h"
#include "mblue_clock.h"

#define LOG_HEAD_SPACE_SIZE (0x4000)
#define FLASH_START_ADDR (0x0800C000) // sector3
#define LOG_START_ADDR (0x08010000) // sector4开始 
#define LOG_END_ADDR (0x0803FFFF) // sector5结束
#define LOG_ERASE_ADDR LOG_END_ADDR
#define MAGIC 12345678
#define HEAD_VALID_FLAG (0xFFFFFFFF)

static int _get_head_size() {
    return sizeof(struct data_section_info);
}

static int persistence_init(struct mb_persistence *lg, struct data_section_info *section,
                            write write_cb, read read_cb, erase erase_cb)
{
    if (NULL == lg)
        return 0;

    lg->section_data = section;
    lg->write_cb = write_cb;
    lg->read_cb = read_cb;
    lg->erase_cb = erase_cb;
    return 1;
}

/**
 * write log data, the buffer length is limited to 128k byte
 * @param  lg
 * @param  buffer
 * @param  len
 * @return      0: fail 1: success
 */
static int persistence_write(struct mb_persistence *lg, uint8_t *buffer, size_t len)
{
    int nRet = 0;

    if (lg->section_data->cur_write_addr + len <= lg->section_data->end_addr)
    {
        if (lg->section_data->erase_addr < lg->section_data->cur_write_addr + len)
        {
            if (lg->section_data->end_addr == lg->section_data->erase_addr) {
                // 已经是日志的最后一个扇区，这个时候需要重新进行循环
                lg->section_data->erase_addr = lg->section_data->start_addr;
            }

            uint32_t size = _get_flash_sector_size(lg->section_data->erase_addr);
            lg->erase_cb(lg->section_data->erase_addr, size);
            lg->section_data->erase_addr += size;
            lg->section_data->data_start_addr = lg->section_data->erase_addr;
            if (lg->section_data->data_start_addr == lg->section_data->end_addr)
                lg->section_data->data_start_addr = lg->section_data->start_addr;
            // if (lg->section_data->data_start_addr > lg->section_data->data_end_addr)
            //     lg->section_data->data_start_addr = lg->section_data->erase_addr;
        }

        nRet = lg->write_cb(lg->section_data->cur_write_addr, len, buffer);
        if (nRet)
            lg->section_data->cur_write_addr += len;

        lg->section_data->data_end_addr = lg->section_data->cur_write_addr;
    }
    else
    {

        // 先填满剩余空间
        size_t _write_size = lg->section_data->end_addr - lg->section_data->cur_write_addr;
        nRet = lg->write_cb(
                   lg->section_data->cur_write_addr,
                   _write_size,
                   buffer);
        size_t moreLen = len - _write_size;
        lg->section_data->cur_write_addr = lg->section_data->start_addr;
        uint32_t size = _get_flash_sector_size(lg->section_data->cur_write_addr);
        lg->erase_cb(lg->section_data->cur_write_addr,
                     size);
        lg->section_data->erase_addr = lg->section_data->cur_write_addr + size;

        lg->section_data->data_start_addr = lg->section_data->erase_addr;

        nRet = lg->write_cb(lg->section_data->cur_write_addr,
                            moreLen,
                            (uint8_t*)buffer + _write_size);
        if (nRet)
            lg->section_data->cur_write_addr = lg->section_data->start_addr + moreLen;
        lg->section_data->data_end_addr = lg->section_data->cur_write_addr;

        nRet = len;
    }

    return nRet;
}

/**
 * read log data
 * @param  lg
 * @param  buffer
 * @param  len
 * @return        0: no more data 1: success
 */
static int persistence_read(struct mb_persistence *lg, uint8_t *buffer, size_t len)
{
    if (lg->section_data->cur_read_addr == lg->section_data->data_end_addr)
        return 0;

    bool ret = false;
    int read_size = 0;

    if (lg->section_data->data_end_addr > lg->section_data->data_start_addr)
    {
        // 日志区域没有填充满
        if (lg->section_data->cur_read_addr + len
                <= lg->section_data->data_end_addr)
        {
            ret = lg->read_cb(lg->section_data->cur_read_addr,
                              len,
                              buffer);
            if (ret)
                lg->section_data->cur_read_addr += len;
        }
        else
        {
            read_size = lg->section_data->data_end_addr - lg->section_data->cur_read_addr;
            ret = lg->read_cb(lg->section_data->cur_read_addr,
                              read_size,
                              buffer);
            if (ret)
                lg->section_data->cur_read_addr = lg->section_data->data_end_addr;
        }
    }
    else
    {
        // 日志区域已经填充满了，需要循环的进行写，此时日志的有效结束地址一定<有效开始地址
        if (lg->section_data->cur_read_addr >= lg->section_data->data_start_addr)
        {
            //先读data_start_addr --> end_addr
            if (lg->section_data->cur_read_addr + len <= lg->section_data->end_addr)
            {
                ret = lg->read_cb(
                          lg->section_data->cur_read_addr,
                          len,
                          buffer);
                if (ret)
                    lg->section_data->cur_read_addr += len;
            }
            else
            {
                if (lg->section_data->end_addr == lg->section_data->cur_read_addr) {
                    ret = 1;
                } else {
                    read_size = lg->section_data->end_addr - lg->section_data->cur_read_addr;
                    ret = lg->read_cb(lg->section_data->cur_read_addr,
                                      read_size,
                                      buffer);
                }
                if (ret)
                    lg->section_data->cur_read_addr = lg->section_data->start_addr;
            }
        }
        else
        {
            // 在读start_addr --> data_end_addr;
            if (lg->section_data->cur_read_addr + len <= lg->section_data->data_end_addr)
            {
                ret = lg->read_cb(
                          lg->section_data->cur_read_addr,
                          len,
                          buffer);
                if (ret)
                    lg->section_data->cur_read_addr += len;
            }
            else
            {
                read_size = lg->section_data->data_end_addr - lg->section_data->cur_read_addr;
                ret = lg->read_cb(lg->section_data->cur_read_addr,
                                  read_size,
                                  buffer);
                if (ret)
                    lg->section_data->cur_read_addr = lg->section_data->data_end_addr;
            }
        }
    }

    return ret;
}

static int persistence_clear(struct mb_persistence *lg)
{
    bool ret = lg->erase_cb(lg->section_data->start_addr,
                            0);
    return ret;
}

static struct mb_persistence persistence_handler = {
    .init = persistence_init,
    .write = persistence_write,
    .read = persistence_read,
    .clear = persistence_clear
};

static int _write_function(uint32_t addr, size_t len, uint8_t *buffer)
{
    return FLASH_COMPLETE == mb_flash_write(addr, buffer, len);
}

static int _read_function(uint32_t addr, size_t len, uint8_t *buffer)
{
    return mb_flash_read(addr, buffer, len);
}

static int _erase_function(uint32_t addr, size_t len)
{
    if (_get_falsh_sector(addr) == FLASH_Sector_5) {
        // 如果需要擦除的是第一个128K的扇区，这个时候需要把head_index重置，设置为0，headinfo重新开始写入
        struct mb_logger *lg = get_log_instance();
        _ASSERT(lg);
        lg->head_index = 0;
    }
    return FLASH_COMPLETE == mb_flash_erase(addr, len);
}

static int _log_init(struct mb_logger *lg)
{
    if (lg) {
        return lg->opr->init(lg->opr, &lg->config, _write_function, _read_function, _erase_function);
    }
    return 0;
}

static void _set_head_index_invalid(uint32_t index)
{
    // 重新设置head的最后一个参数valid参数为0x00000000
    uint32_t offset = FLASH_START_ADDR + index * _get_head_size() + _get_head_size() - 4;
    uint32_t valid = 0x00000000;
    mb_flash_write_word(offset, valid);
}

static bool __is_valid_head_index(uint32_t head_index)
{
    return (head_index + 1) * _get_head_size() > LOG_HEAD_SPACE_SIZE ? false : true;
}

static void _flush_head_index(struct mb_logger *lg)
{
    _ASSERT(lg);
    if (lg->head_index > 0) {
        _set_head_index_invalid(lg->head_index - 1);
    }

    if (__is_valid_head_index(lg->head_index)) {
        mb_flash_write(FLASH_START_ADDR + lg->head_index * _get_head_size(), (uint8_t *) & (lg->config), _get_head_size());
        lg->head_index++;
    } else {
        mb_flash_erase(FLASH_START_ADDR, LOG_HEAD_SPACE_SIZE);
        lg->head_index = 0;
    }
}

/**
 *  format current time
 * @param buffer [description]
 */
static void _prinf_timer(char *buffer)
{
    struct mblue_clock *_clock = mblue_clock_get_instance();
    if (_clock) {

        uint32_t time_tamp = _clock->get_utc(_clock);

        struct mblue_clock_entity *_cur_time = _clock->get_calendar(_clock);
        _ASSERT(_cur_time);
        sprintf(buffer, "%d-%d-%d %d:%d:%d", _cur_time->year,
                _cur_time->month,
                _cur_time->day,
                _cur_time->hour,
                _cur_time->minute,
                _cur_time->second);
    }
}

static int _log_write(struct mb_logger *lg, uint8_t *buffer, uint32_t len)
{
    _ASSERT(lg);
    char timer[32] = {0};
    _prinf_timer(timer);

    char *temp = mblue_malloc(strlen(timer) + len + 8);
    sprintf(temp, "%s %s \r\n", timer, buffer);
    int ret = lg->opr->write(lg->opr, (uint8_t *)temp,  strlen(temp));
    mblue_free(temp);
    temp = NULL;

    _flush_head_index(lg);
    return ret;
}

static int _log_read_next(struct mb_logger *lg, uint8_t *buffer, uint32_t len)
{
    _ASSERT(lg);

    int _ret = lg->opr->read(lg->opr, buffer, len);
    if (_ret) {
        int _len = len - strlen((char const *)buffer);
        if (_len > 0) {
            uint8_t *p = buffer;
            p = p + strlen((char const *)buffer);
            _ret = lg->opr->read(lg->opr, p, _len);
        }
    }
    return _ret;
}

static int _log_read(struct mb_logger *lg, uint8_t *buffer, uint32_t len)
{
    _ASSERT(lg);
    lg->config.cur_read_addr = lg->config.data_start_addr;
    return _log_read_next(lg, buffer, len);
}

static int _log_clear(struct mb_logger *lg)
{
    mb_flash_erase(FLASH_ADDR_HEAD, 0);
    mb_flash_erase(FLASH_ADDR_SECTOR1, 0);
    mb_flash_erase(FLASH_ADDR_SECTOR2, 0);
    return 1;
}

static bool __find_valid_head_index(struct mb_logger *lg)
{
    if (!__is_valid_head_index(lg->head_index))
        return false;

    struct data_section_info head;
    while (1) {
        memset(&head, 0x0, sizeof(struct data_section_info));
        if(__is_valid_head_index(lg->head_index)){
            mb_flash_read(FLASH_START_ADDR + lg->head_index * _get_head_size(), (uint8_t *)&head, sizeof(head));
            lg->head_index++;
            if (head.magic == MAGIC && head.valid == HEAD_VALID_FLAG) { // 该区域内已经写入过数据，并且是有效数据
            memcpy(&lg->config, &head, sizeof(struct data_section_info));
            return true;
             }
        }else{
            return false;
        }
    }
}

static bool __is_valid_flash_space(uint8_t *data, uint32_t len) {
    bool valid = true;
    uint8_t *p = data;
    while (len--) {
        uint8_t c = *(p++);
        valid &= (c == 0xFF);
    }
    return valid;
}

static void __confirm_head_and_data_section_valid(struct mb_logger *lg)
{
    uint32_t start_addr = lg->config.cur_write_addr;
    if (start_addr >= lg->config.start_addr && start_addr < lg->config.end_addr) {
        if (start_addr + 8 <= lg->config.end_addr) {
            uint8_t buffer[8] = {0};
            mb_flash_read(start_addr, buffer, sizeof(buffer));
            if (!__is_valid_flash_space(buffer, 8)) {
                // 擦除所有的数据
                _log_clear(lg);
                _flush_head_index(lg);
            }
        }
    }
}

static void _init_check(struct mb_logger *lg)
{
    struct data_section_info head;
    memset(&head, 0x0, sizeof(struct data_section_info));
    mb_flash_read(FLASH_START_ADDR, (uint8_t *)&head, sizeof(head));
    if (head.magic != MAGIC) {
        // 表示没写入过数据
        _log_clear(lg);
        _flush_head_index(lg);
        return;
    }

    if (!__find_valid_head_index(lg))
    {
        // 擦除所有的数据
        _log_clear(lg);
        _flush_head_index(lg);
    } else {
        __confirm_head_and_data_section_valid(lg);
    }
}

static struct mb_logger logger_handler = {
    .head_index = 0,
    .config = {
        .start_addr = LOG_START_ADDR,
        .end_addr = LOG_END_ADDR,
        .cur_write_addr = LOG_START_ADDR,
        .cur_read_addr = LOG_START_ADDR,
        .data_start_addr = LOG_START_ADDR,
        .data_end_addr = LOG_START_ADDR,
        .erase_addr = LOG_ERASE_ADDR,
        .magic = MAGIC,
        .valid = HEAD_VALID_FLAG
    },
    .init = _log_init,
    .write = _log_write,
    .read = _log_read,
    .read_next = _log_read_next,
    .clear = _log_clear
};

static struct mb_logger *instance = NULL;

struct mb_logger *get_log_instance()
{
    if (NULL == instance) {
        instance = &logger_handler;
        instance->opr = &persistence_handler;
        _init_check(instance);
    }
    return instance;
}
