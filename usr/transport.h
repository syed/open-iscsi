/*
 * iSCSI transport
 *
 * Copyright (C) 2006 Mike Christie
 * Copyright (C) 2006 Red Hat, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 */
#ifndef ISCSI_TRANSPORT_H
#define ISCSI_TRANSPORT_H

#include "types.h"
#include "config.h"

struct iscsi_transport;
struct iscsi_conn;

struct iscsi_transport_template {
	const char *name;
	uint8_t rdma;
	/*
	 * Drivers should set this if they require iscsid to set
	 * the host's ip address.
	 */
	uint8_t set_host_ip;
	uint8_t use_boot_info;
	int (*ep_connect) (struct iscsi_conn *conn, int non_blocking);
	int (*ep_poll) (struct iscsi_conn *conn, int timeout_ms);
	void (*ep_disconnect) (struct iscsi_conn *conn);
	void (*create_conn) (struct iscsi_conn *conn);
	int (*set_net_config) (struct iscsi_transport *t,
			       struct iface_rec *iface,
			       struct iscsi_session *session);
};

/* represents data path provider */
struct iscsi_transport {
	struct list_head list;
	uint64_t handle;
	uint32_t caps;
	char name[ISCSI_TRANSPORT_NAME_MAXLEN];
#define ISCSI_TRANSPORT_SESSION_HASH_BITS 14
	struct hlist_head sessions[1 << 14];
	struct iscsi_transport_template *template;
};

extern int set_transport_template(struct iscsi_transport *t);
extern int transport_load_kmod(char *transport_name);
extern int transport_probe_for_offload(void);

/** From include/linux/hash.h **/
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
static inline uint32_t hash_32(uint32_t val, unsigned int bits)
{
	/* On some cpus multiply is faster, on others gcc will do shifts */
	uint32_t hash = val * GOLDEN_RATIO_PRIME_32;

	/* High bits are more random, so use them. */
	return hash >> (32 - bits);
}

#endif
