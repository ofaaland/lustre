/* -*- mode: c; c-basic-offset: 8; indent-tabs-mode: nil; -*-
 * vim:expandtab:shiftwidth=8:tabstop=8:
 *
 *  Copyright (C) 2002 Cluster File Systems, Inc.
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
 */

#ifndef _LUSTRE_NET_H
#define _LUSTRE_NET_H

#ifdef __KERNEL__
#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
#include <linux/tqueue.h>
#else
#include <linux/workqueue.h>
#endif
#endif 

#include <linux/kp30.h>
// #include <linux/obd.h>
#include <portals/p30.h>
#include <linux/lustre_idl.h>
#include <linux/lustre_ha.h>
#include <linux/lustre_import.h>

/* The following constants determine how much memory is devoted to
 * buffering in the lustre services.
 *
 * ?_NEVENTS            # event queue entries
 *
 * ?_NBUFS              # request buffers
 * ?_BUFSIZE            # bytes in a single request buffer
 * total memory = ?_NBUFS * ?_BUFSIZE
 *
 * ?_MAXREQSIZE         # maximum request service will receive
 * larger messages will get dropped.
 * request buffers are auto-unlinked when less than ?_MAXREQSIZE
 * is left in them.
 */

#define LDLM_NUM_THREADS        4
#define LDLM_NEVENT_MAX 8192UL
#define LDLM_NEVENTS    min(num_physpages / 64, LDLM_NEVENT_MAX)
#define LDLM_NBUF_MAX   256UL
#define LDLM_NBUFS      min(LDLM_NEVENTS / 16, LDLM_NBUF_MAX)
#define LDLM_BUFSIZE    (8 * 1024)
#define LDLM_MAXREQSIZE 1024

#define MDT_NUM_THREADS 8
#define MDS_NEVENT_MAX  8192UL
#define MDS_NEVENTS     min(num_physpages / 64, MDS_NEVENT_MAX)
#define MDS_NBUF_MAX    512UL
#define MDS_NBUFS       min(MDS_NEVENTS / 16, MDS_NBUF_MAX)
#define MDS_BUFSIZE     (8 * 1024)
/* Assume file name length = FNAME_MAX = 256 (true for extN).
 *        path name length = PATH_MAX = 4096
 *        LOV MD size max  = EA_MAX = 4000
 * symlink:  FNAME_MAX + PATH_MAX  <- largest
 * link:     FNAME_MAX + PATH_MAX  (mds_rec_link < mds_rec_create)
 * rename:   FNAME_MAX + FNAME_MAX
 * open:     FNAME_MAX + EA_MAX
 *
 * MDS_MAXREQSIZE ~= 4736 bytes =
 * lustre_msg + ldlm_request + mds_body + mds_rec_create + FNAME_MAX + PATH_MAX
 *
 * Realistic size is about 512 bytes (20 character name + 128 char symlink),
 * except in the open case where there are a large number of OSTs in a LOV.
 */
#define MDS_MAXREQSIZE  (5 * 1024)

#define OST_NUM_THREADS 6
#define OST_NEVENT_MAX  32768UL
#define OST_NEVENTS     min(num_physpages / 16, OST_NEVENT_MAX)
#define OST_NBUF_MAX    1280UL
#define OST_NBUFS       min(OST_NEVENTS / 64, OST_NBUF_MAX)
#define OST_BUFSIZE     (8 * 1024)
/* OST_MAXREQSIZE ~= 1896 bytes =
 * lustre_msg + obdo + 16 * obd_ioobj + 64 * niobuf_remote
 *
 * single object with 16 pages is 576 bytes
 */
#define OST_MAXREQSIZE  (2 * 1024)

#define PTLBD_NUM_THREADS        4
#define PTLBD_NEVENTS    1024
#define PTLBD_NBUFS      20
#define PTLBD_BUFSIZE    (32 * 1024)
#define PTLBD_MAXREQSIZE 1024

#define CONN_INVALID 1

struct ptlrpc_peer {
        ptl_nid_t         peer_nid;
        struct ptlrpc_ni *peer_ni;
};

struct ptlrpc_connection {
        struct list_head        c_link;
        struct ptlrpc_peer      c_peer;
        struct obd_uuid         c_local_uuid;  /* XXX do we need this? */
        struct obd_uuid         c_remote_uuid;

        __u32                   c_generation;  /* changes upon new connection */
        __u32                   c_epoch;       /* changes when peer changes */
        __u32                   c_bootcount;   /* peer's boot count */

        spinlock_t              c_lock;        /* also protects req->rq_list */

        atomic_t                c_refcount;
        __u64                   c_token;
        __u64                   c_remote_conn;
        __u64                   c_remote_token;

        struct list_head        c_delayed_head;/* delayed until post-recovery XXX imp? */
        struct recovd_data      c_recovd_data;

        struct list_head        c_imports;
        struct list_head        c_exports;
        struct list_head        c_sb_chain;
        __u32                   c_flags; // can we indicate INVALID elsewhere?
};

struct ptlrpc_client {
        __u32                     cli_request_portal;
        __u32                     cli_reply_portal;

        __u32                     cli_target_devno;

        void                     *cli_data;
        char                     *cli_name;
};

/* state flags of requests */
#define PTL_RPC_FL_INTR      (1 << 0)  /* reply wait was interrupted by user */
#define PTL_RPC_FL_REPLIED   (1 << 1)  /* reply was received */
#define PTL_RPC_FL_SENT      (1 << 2)  /* request was sent */
#define PTL_RPC_FL_WANT_ACK  (1 << 3)  /* reply is awaiting an ACK */
#define PTL_BULK_FL_SENT     (1 << 4)  /* outgoing bulk was sent */
#define PTL_BULK_FL_RCVD     (1 << 5)  /* incoming bulk was recieved */
#define PTL_RPC_FL_ERR       (1 << 6)  /* request failed due to RPC error */
#define PTL_RPC_FL_TIMEOUT   (1 << 7)  /* request timed out waiting for reply */
#define PTL_RPC_FL_RESEND    (1 << 8)  /* retransmit the request */
#define PTL_RPC_FL_RESTART   (1 << 9)  /* operation must be restarted */
#define PTL_RPC_FL_RETAIN    (1 << 10) /* retain for replay after reply */
#define PTL_RPC_FL_REPLAY    (1 << 11) /* replay upon recovery */
#define PTL_RPC_FL_ALLOCREP  (1 << 12) /* reply buffer allocated */
#define PTL_RPC_FL_NO_RESEND (1 << 13) /* don't automatically resend this req */
#define PTL_RPC_FL_RESENT    (1 << 14) /* server rcvd resend of this req */

struct ptlrpc_request {
        int rq_type; /* one of PTL_RPC_MSG_* */
        struct list_head rq_list;
        struct obd_device *rq_obd;
        int rq_status;
        int rq_flags;
        atomic_t rq_refcount;

        int rq_request_portal; /* XXX FIXME bug 249 */
        int rq_reply_portal; /* XXX FIXME bug 249 */

        int rq_reqlen;
        struct lustre_msg *rq_reqmsg;

        int rq_timeout;
        int rq_replen;
        struct lustre_msg *rq_repmsg;
        __u64 rq_transno;
        __u64 rq_xid;

        int rq_level;
        wait_queue_head_t rq_wait_for_rep; /* XXX also _for_ack */

        /* incoming reply */
        ptl_md_t rq_reply_md;
        ptl_handle_me_t rq_reply_me_h;

        /* outgoing req/rep */
        ptl_md_t rq_req_md;

        struct ptlrpc_peer rq_peer; /* XXX see service.c can this be factored away? */
        struct obd_export *rq_export;
        struct ptlrpc_connection *rq_connection;
        struct obd_import *rq_import;
        struct ptlrpc_service *rq_svc;

        void (*rq_replay_cb)(struct ptlrpc_request *);
        void  *rq_replay_data;

        /* Only used on the server side for tracking acks. */
        struct ptlrpc_req_ack_lock {
                struct lustre_handle lock;
                __u32                mode;
        } rq_ack_locks[4];
};

#define DEBUG_REQ(level, req, fmt, args...)                                    \
do {                                                                           \
CDEBUG(level,                                                                  \
       "@@@ " fmt " req@%p x"LPD64"/t"LPD64" o%d->%s:%d lens %d/%d ref %d fl " \
       "%x/%x/%x rc %x\n" ,  ## args, req, req->rq_xid,                        \
       req->rq_reqmsg ? req->rq_reqmsg->transno : -1,                          \
       req->rq_reqmsg ? req->rq_reqmsg->opc : -1,                              \
       req->rq_connection ?                                                    \
          (char *)req->rq_connection->c_remote_uuid.uuid : "<?>",              \
       (req->rq_import && req->rq_import->imp_client) ?                        \
           req->rq_import->imp_client->cli_request_portal : -1,                \
       req->rq_reqlen, req->rq_replen,                                         \
       atomic_read (&req->rq_refcount), req->rq_flags,                         \
       req->rq_reqmsg ? req->rq_reqmsg->flags : 0,                             \
       req->rq_repmsg ? req->rq_repmsg->flags : 0,                             \
       req->rq_status);                                                        \
} while (0)

struct ptlrpc_bulk_page {
        struct ptlrpc_bulk_desc *bp_desc;
        struct list_head bp_link;
        void *bp_buf;
        int bp_buflen;
        struct page *bp_page;
        __u32 bp_xid;
        __u32 bp_flags;
        struct dentry *bp_dentry;
        int (*bp_cb)(struct ptlrpc_bulk_page *);
};


struct ptlrpc_bulk_desc {
        struct list_head bd_set_chain; /* entry in obd_brw_set */
        struct obd_brw_set *bd_brw_set;
        int bd_flags;
        struct ptlrpc_connection *bd_connection;
        struct ptlrpc_client *bd_client;
        __u32 bd_portal;
        struct lustre_handle bd_conn;
        void (*bd_ptl_ev_hdlr)(struct ptlrpc_bulk_desc *);

        wait_queue_head_t bd_waitq;
        struct list_head bd_page_list;
        __u32 bd_page_count;
        atomic_t bd_refcount;
        void *bd_desc_private;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0))
        struct work_struct bd_queue;
#else
        struct tq_struct bd_queue;
#endif

        ptl_md_t bd_md;
        ptl_handle_md_t bd_md_h;
        ptl_handle_me_t bd_me_h;

        atomic_t bd_source_callback_count;

        struct iovec bd_iov[16];    /* self-sized pre-allocated iov */
};

struct ptlrpc_thread {
        struct list_head t_link;

        __u32 t_flags;
        wait_queue_head_t t_ctl_waitq;
};

struct ptlrpc_request_buffer_desc {
        struct list_head       rqbd_list;
        struct ptlrpc_srv_ni  *rqbd_srv_ni;
        ptl_handle_me_t        rqbd_me_h;
        atomic_t               rqbd_refcount;
        char                  *rqbd_buffer;
};

struct ptlrpc_ni {
        /* Generic interface state */
        char                   *pni_name;
        ptl_handle_ni_t         pni_ni_h;
        ptl_handle_eq_t         pni_request_out_eq_h;
        ptl_handle_eq_t         pni_reply_in_eq_h;
        ptl_handle_eq_t         pni_reply_out_eq_h;
        ptl_handle_eq_t         pni_bulk_put_source_eq_h;
        ptl_handle_eq_t         pni_bulk_put_sink_eq_h;
        ptl_handle_eq_t         pni_bulk_get_source_eq_h;
        ptl_handle_eq_t         pni_bulk_get_sink_eq_h;
};

struct ptlrpc_srv_ni {
        /* Interface-specific service state */
        struct ptlrpc_service  *sni_service;    /* owning service */
        struct ptlrpc_ni       *sni_ni;         /* network interface */
        ptl_handle_eq_t         sni_eq_h;       /* event queue handle */
        struct list_head        sni_rqbds;      /* all the request buffer descriptors */
        __u32                   sni_nrqbds;     /* # request buffers */
        atomic_t                sni_nrqbds_receiving; /* # request buffers posted */
};

struct ptlrpc_service {
        time_t srv_time;
        time_t srv_timeout;

        struct list_head srv_ni_list;          /* list of interfaces */
        __u32            srv_max_req_size;     /* biggest request to receive */
        __u32            srv_buf_size;         /* # bytes in a request buffer */

        __u32 srv_req_portal;
        __u32 srv_rep_portal;

        __u32 srv_xid;

        wait_queue_head_t srv_waitq; /* all threads sleep on this */

        spinlock_t srv_lock;
        struct list_head srv_threads;
        int (*srv_handler)(struct ptlrpc_request *req);
        char *srv_name;  /* only statically allocated strings here; we don't clean them */

        int                  srv_interface_rover;
        struct ptlrpc_srv_ni srv_interfaces[0];
};

static inline void ptlrpc_hdl2req(struct ptlrpc_request *req,
                                  struct lustre_handle *h)
{
        req->rq_reqmsg->addr = h->addr;
        req->rq_reqmsg->cookie = h->cookie;
}

typedef void (*bulk_callback_t)(struct ptlrpc_bulk_desc *, void *);

typedef int (*svc_handler_t)(struct ptlrpc_request *req);

/* rpc/events.c */
extern struct ptlrpc_ni ptlrpc_interfaces[];
extern int              ptlrpc_ninterfaces;
extern int ptlrpc_uuid_to_peer (struct obd_uuid *uuid, struct ptlrpc_peer *peer);

/* rpc/connection.c */
void ptlrpc_readdress_connection(struct ptlrpc_connection *, struct obd_uuid *uuid);
struct ptlrpc_connection *ptlrpc_get_connection(struct ptlrpc_peer *peer,
                                                struct obd_uuid *uuid);
int ptlrpc_put_connection(struct ptlrpc_connection *c);
struct ptlrpc_connection *ptlrpc_connection_addref(struct ptlrpc_connection *);
void ptlrpc_init_connection(void);
void ptlrpc_cleanup_connection(void);

/* rpc/niobuf.c */
int ptlrpc_check_bulk_sent(struct ptlrpc_bulk_desc *bulk);
int ptlrpc_check_bulk_received(struct ptlrpc_bulk_desc *bulk);
int ptlrpc_bulk_put(struct ptlrpc_bulk_desc *);
int ptlrpc_bulk_get(struct ptlrpc_bulk_desc *);
int ptlrpc_register_bulk_put(struct ptlrpc_bulk_desc *);
int ptlrpc_register_bulk_get(struct ptlrpc_bulk_desc *);
int ptlrpc_abort_bulk(struct ptlrpc_bulk_desc *bulk);
struct obd_brw_set *obd_brw_set_new(void);
void obd_brw_set_add(struct obd_brw_set *, struct ptlrpc_bulk_desc *);
void obd_brw_set_del(struct ptlrpc_bulk_desc *);
void obd_brw_set_decref(struct obd_brw_set *set);
void obd_brw_set_addref(struct obd_brw_set *set);

int ptlrpc_reply(struct ptlrpc_service *svc, struct ptlrpc_request *req);
int ptlrpc_error(struct ptlrpc_service *svc, struct ptlrpc_request *req);
void ptlrpc_resend_req(struct ptlrpc_request *request);
int ptl_send_rpc(struct ptlrpc_request *request);
void ptlrpc_link_svc_me(struct ptlrpc_request_buffer_desc *rqbd);

/* rpc/client.c */
void ptlrpc_init_client(int req_portal, int rep_portal, char *name,
                        struct ptlrpc_client *);
void ptlrpc_cleanup_client(struct obd_import *imp);
struct obd_uuid *ptlrpc_req_to_uuid(struct ptlrpc_request *req);
struct ptlrpc_connection *ptlrpc_uuid_to_connection(struct obd_uuid *uuid);

int ll_brw_sync_wait(struct obd_brw_set *, int phase);

int ptlrpc_queue_wait(struct ptlrpc_request *req);
void ptlrpc_continue_req(struct ptlrpc_request *req);
int ptlrpc_replay_req(struct ptlrpc_request *req);
int ptlrpc_abort(struct ptlrpc_request *req);
void ptlrpc_restart_req(struct ptlrpc_request *req);
void ptlrpc_abort_inflight(struct obd_import *imp, int dying_import);

struct ptlrpc_request *ptlrpc_prep_req(struct obd_import *imp, int opcode,
                                       int count, int *lengths, char **bufs);
void ptlrpc_free_req(struct ptlrpc_request *request);
void ptlrpc_req_finished(struct ptlrpc_request *request);
struct ptlrpc_request *ptlrpc_request_addref(struct ptlrpc_request *req);
struct ptlrpc_bulk_desc *ptlrpc_prep_bulk(struct ptlrpc_connection *);
void ptlrpc_free_bulk(struct ptlrpc_bulk_desc *bulk);
struct ptlrpc_bulk_page *ptlrpc_prep_bulk_page(struct ptlrpc_bulk_desc *desc);
void ptlrpc_free_bulk_page(struct ptlrpc_bulk_page *page);
void ptlrpc_retain_replayable_request(struct ptlrpc_request *req,
                                      struct obd_import *imp);

/* rpc/service.c */
struct ptlrpc_service *
ptlrpc_init_svc(__u32 nevents, __u32 nbufs, __u32 bufsize, __u32 max_req_size,
                int req_portal, int rep_portal, svc_handler_t, char *name);
void ptlrpc_stop_all_threads(struct ptlrpc_service *svc);
int ptlrpc_start_thread(struct obd_device *dev, struct ptlrpc_service *svc,
                        char *name);
int ptlrpc_unregister_service(struct ptlrpc_service *service);

struct ptlrpc_svc_data {
        char *name;
        struct ptlrpc_service *svc;
        struct ptlrpc_thread *thread;
        struct obd_device *dev;
};

/* rpc/pack_generic.c */
int lustre_pack_msg(int count, int *lens, char **bufs, int *len,
                    struct lustre_msg **msg);
int lustre_msg_size(int count, int *lengths);
int lustre_unpack_msg(struct lustre_msg *m, int len);
void *lustre_msg_buf(struct lustre_msg *m, int n);

/* rpc/rpc.c */
__u32 ptlrpc_next_xid(void);

static inline void ptlrpc_bulk_decref(struct ptlrpc_bulk_desc *desc)
{
        CDEBUG(D_PAGE, "%p -> %d\n", desc, atomic_read(&desc->bd_refcount) - 1);

        if (atomic_dec_and_test(&desc->bd_refcount)) {
                CDEBUG(D_PAGE, "Released last ref on %p, freeing\n", desc);
                ptlrpc_free_bulk(desc);
        }
}

static inline void ptlrpc_bulk_addref(struct ptlrpc_bulk_desc *desc)
{
        atomic_inc(&desc->bd_refcount);
        CDEBUG(D_PAGE, "Set refcount of %p to %d\n", desc,
               atomic_read(&desc->bd_refcount));
}

#endif
