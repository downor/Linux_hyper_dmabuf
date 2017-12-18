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

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/random.h>
#include "hyper_dmabuf_drv.h"
#include "hyper_dmabuf_id.h"
#include "hyper_dmabuf_msg.h"

extern struct hyper_dmabuf_private hyper_dmabuf_private;

void store_reusable_hid(hyper_dmabuf_id_t hid)
{
	struct list_reusable_id *reusable_head = hyper_dmabuf_private.id_queue;
	struct list_reusable_id *new_reusable;

	new_reusable = kmalloc(sizeof(*new_reusable), GFP_KERNEL);

	if (!new_reusable) {
		dev_err(hyper_dmabuf_private.device,
			"No memory left to be allocated\n");
		return;
	}

	new_reusable->hid = hid;

	list_add(&new_reusable->list, &reusable_head->list);
}

static hyper_dmabuf_id_t retrieve_reusable_hid(void)
{
	struct list_reusable_id *reusable_head = hyper_dmabuf_private.id_queue;
	hyper_dmabuf_id_t hid = {-1, {0,0,0}};

	/* check there is reusable id */
	if (!list_empty(&reusable_head->list)) {
		reusable_head = list_first_entry(&reusable_head->list,
						 struct list_reusable_id,
						 list);

		list_del(&reusable_head->list);
		hid = reusable_head->hid;
		kfree(reusable_head);
	}

	return hid;
}

void destroy_reusable_list(void)
{
	struct list_reusable_id *reusable_head = hyper_dmabuf_private.id_queue;
	struct list_reusable_id *temp_head;

	if (reusable_head) {
		/* freeing mem space all reusable ids in the stack */
		while (!list_empty(&reusable_head->list)) {
			temp_head = list_first_entry(&reusable_head->list,
						     struct list_reusable_id,
						     list);
			list_del(&temp_head->list);
			kfree(temp_head);
		}

		/* freeing head */
		kfree(reusable_head);
	}
}

hyper_dmabuf_id_t hyper_dmabuf_get_hid(void)
{
	static int count = 0;
	hyper_dmabuf_id_t hid;
	struct list_reusable_id *reusable_head;

	/* first call to hyper_dmabuf_get_id */
	if (count == 0) {
		reusable_head = kmalloc(sizeof(*reusable_head), GFP_KERNEL);

		if (!reusable_head) {
			dev_err(hyper_dmabuf_private.device,
				"No memory left to be allocated\n");
			return (hyper_dmabuf_id_t){-1, {0,0,0}};
		}

		reusable_head->hid.id = -1; /* list head has an invalid count */
		INIT_LIST_HEAD(&reusable_head->list);
		hyper_dmabuf_private.id_queue = reusable_head;
	}

	hid = retrieve_reusable_hid();

	/*creating a new H-ID only if nothing in the reusable id queue
	 * and count is less than maximum allowed
	 */
	if (hid.id == -1 && count < HYPER_DMABUF_ID_MAX) {
		hid.id = HYPER_DMABUF_ID_CREATE(hyper_dmabuf_private.domid, count++);
	}

	/* random data embedded in the id for security */
	get_random_bytes(&hid.rng_key[0], 12);

	return hid;
}

bool hyper_dmabuf_hid_keycomp(hyper_dmabuf_id_t hid1, hyper_dmabuf_id_t hid2)
{
	int i;

	/* compare keys */
	for (i=0; i<3; i++) {
		if (hid1.rng_key[i] != hid2.rng_key[i])
			return false;
	}

	return true;
}
