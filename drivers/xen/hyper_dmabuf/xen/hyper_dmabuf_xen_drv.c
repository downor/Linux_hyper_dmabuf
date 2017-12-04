/*
 * Copyright © 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Dongwon Kim <dongwon.kim@intel.com>
 *    Mateusz Polrola <mateuszx.potrola@intel.com>
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <xen/grant_table.h>
#include "../hyper_dmabuf_msg.h"
#include "../hyper_dmabuf_drv.h"
#include "hyper_dmabuf_xen_drv.h"
#include "hyper_dmabuf_xen_comm.h"
#include "hyper_dmabuf_xen_shm.h"

struct hyper_dmabuf_backend_ops xen_backend_ops = {
	.get_vm_id = hyper_dmabuf_get_domid,
	.share_pages = hyper_dmabuf_xen_share_pages,
	.unshare_pages = hyper_dmabuf_xen_unshare_pages,
	.map_shared_pages = (void *)hyper_dmabuf_xen_map_shared_pages,
	.unmap_shared_pages = hyper_dmabuf_xen_unmap_shared_pages,
	.init_comm_env = hyper_dmabuf_xen_init_comm_env,
	.destroy_comm = hyper_dmabuf_xen_destroy_comm,
	.init_rx_ch = hyper_dmabuf_xen_init_rx_rbuf,
	.init_tx_ch = hyper_dmabuf_xen_init_tx_rbuf,
	.send_req = hyper_dmabuf_xen_send_req,
};
