/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (C) 2001 Cluster File Systems, Inc. <braam@clusterfs.com>
 *
 *   This file is part of Lustre, http://www.lustre.org.
 *
 *   Lustre is free software; you can redistribute it and/or
 *   modify it under the terms of version 2 of the GNU General Public
 *   License as published by the Free Software Foundation.
 *
 *   Lustre is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Lustre; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * (Un)packing of OST requests
 *
 */

#define DEBUG_SUBSYSTEM S_OST
#ifndef __KERNEL__
#include <liblustre.h>
#endif

#include <linux/obd_ost.h>
#include <linux/lustre_net.h>

void ost_pack_ioo(struct obd_ioobj *ioo, struct lov_stripe_md *lsm, int bufcnt)
{
        ioo->ioo_id = HTON__u64(lsm->lsm_object_id);
        ioo->ioo_gr = HTON__u64(0);
        ioo->ioo_type = HTON__u32(S_IFREG);
        ioo->ioo_bufcnt = HTON__u32(bufcnt);
}

void ost_unpack_ioo(struct obd_ioobj *dst, struct obd_ioobj *src)
{
        dst->ioo_id = NTOH__u64(src->ioo_id);
        dst->ioo_gr = NTOH__u64(src->ioo_gr);
        dst->ioo_type = NTOH__u32(src->ioo_type);
        dst->ioo_bufcnt = NTOH__u32(src->ioo_bufcnt);
}

void ost_pack_niobuf(struct niobuf_remote *nb, __u64 offset, __u32 len,
                     __u32 flags, __u32 xid)
{
        nb->offset = HTON__u64(offset);
        nb->len = HTON__u32(len);
        nb->xid = HTON__u32(xid);
        nb->flags = HTON__u32(flags);
}

void ost_unpack_niobuf(struct niobuf_remote *dst, struct niobuf_remote *src)
{
        dst->offset = NTOH__u64(src->offset);
        dst->len = NTOH__u32(src->len);
        dst->xid = NTOH__u32(src->xid);
        dst->flags = NTOH__u32(src->flags);
}
