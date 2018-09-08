/*
 * =====================================================================================
 *
 *       Filename:  mblue_network_proxy.c
 *
 *    Description:  external interfaces of network proxy
 *
 *        Version:  1.0
 *        Created:  2017/8/10 21:00:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "task_manager.h"
#include "mblue_heap.h"
#include "data_schema.h"

#include "rpc_helper.h"
#include "mblue_network_proxy.h"

static int __default_get_tcp_remote_callback(struct message *s, struct message *d)
{
	struct device_common_param *dp;
	dp = (struct device_common_param *)s->data;

	_ASSERT(dp);
	if (dp) {
		dp->extra = d->data;
	}
	return 0;
}

static int32_t __invoke_remote_tcp_interface(uint16_t interface, 
	uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
	struct device_common_param dp;

	dp.code = (int32_t)fd;
	dp.num1 = len;
	dp.num2 = timeout_ms;
	dp.data = (uint8_t *)buf;

	sync_call(NETWORK, NET_TCP, interface,
		(void *)&dp, 
		(void *)__default_get_tcp_remote_callback);

	return (int32_t)dp.extra;
}

uintptr_t mblue_tcp_establish(const char *host, uint16_t port)
{
	LOGGER("mblue_tcp_establish %s|%d\r\n", host, port);
	return (uintptr_t)__invoke_remote_tcp_interface(
		MBLUE_NETWORK_TCP_CREATE, 0, (char *)host, port, 0);
}

int32_t mblue_tcp_destroy(uintptr_t fd)
{
	LOGGER("mblue_tcp_destroy %d\r\n", fd);
	return __invoke_remote_tcp_interface(
		MBLUE_NETWORK_TCP_RELEASE, fd, NULL, 0, 0);
}

int32_t mblue_tcp_write(uintptr_t fd, 
	const char *buf, uint32_t len, uint32_t timeout_ms)
{
	LOGGER("mblue_tcp_write %d|(%s)|%d|%d\r\n", fd, buf, len, timeout_ms);
	return __invoke_remote_tcp_interface(
		MBLUE_NETWORK_TCP_WRITE, fd, (char *)buf, len, timeout_ms);
}

int32_t mblue_tcp_read(uintptr_t fd, 
	char *buf, uint32_t len, uint32_t timeout_ms)
{
	LOGGER("mblue_tcp_read %d|%d|%d\r\n", fd, len, timeout_ms);
	return __invoke_remote_tcp_interface(
		MBLUE_NETWORK_TCP_READ, fd, buf, len, timeout_ms);
}
