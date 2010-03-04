/* 
 * Copyright (C) 2008-2009 National Institute of Advanced Industrial Science and Technology
 */

#include "xnbd.h"


static void fill_random(char *buff, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		long int rvalue = random();
		buff[i] = (char) rvalue;
	}
}


struct crequest {
	uint32_t iotype;

	off_t  iofrom;
	size_t iolen;

	char *write_buff;

	uint32_t index;
};


struct crequest eofmarker = { .write_buff = NULL };




struct bginfo_struct {
	off_t disksize;
	pthread_mutex_t lock;
	pthread_cond_t  init_done;
	pthread_t       tid;

	const char *ctlpath;
	int count;
} bginfo_data;

// uint64_t bgctl_disksize = 0;
// pthread_mutex_t bgthread_lock = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t  bgthread_init_done = PTHREAD_COND_INITIALIZER;
// pthread_t bgthread_tid;


void *bgctl_thread_main(void *data)
{
	struct bginfo_struct *bginfo = (struct bginfo_struct *) data;

	int bgctlfd = 0;


	for (;;) {
		info("try open %s", bginfo->ctlpath);
		bgctlfd = open(bginfo->ctlpath, O_WRONLY);
		if (bgctlfd < 0) {
			warn("open bgctl %s, %m", bginfo->ctlpath);
			sleep(1);
			continue;
		}
		break;
	}

	info("open %s done", bginfo->ctlpath);

	pthread_mutex_lock(&bginfo->lock);
	pthread_cond_signal(&bginfo->init_done);
	pthread_mutex_unlock(&bginfo->lock);


	for (;;) {
		off_t nblocks = bginfo->disksize / CBLOCKSIZE;
		unsigned long bindex =  (unsigned long) (1.0L * nblocks * random() / RAND_MAX);

#if 0
		/* this stops bgctl threads, so io with tester will fail */
		if (random() % 1000 == 0)
			bindex = XNBD_BGCTL_MAGIC_CACHE_ALL;
#endif

		ssize_t ret = write(bgctlfd, &bindex, sizeof(bindex));
		if (ret != sizeof(bindex))
			err("write bgctl");

		info("%d bgctl bindex %lu (iofrom %ju)\n", bginfo->count, bindex, (off_t) bindex * CBLOCKSIZE);

		bginfo->count += 1;

		poll(NULL, 0, (int) (20.0L * random() / RAND_MAX));


		if (bginfo->count > 1000)
			break;
	}

	close(bgctlfd);

	g_message("bgthread bye");

	return NULL;
}


void bgctl_thread_create(off_t disksize, const char *bgctlpath)
{
	struct bginfo_struct *bginfo = &bginfo_data;

	bzero(bginfo, sizeof(struct bginfo_struct));
	pthread_mutex_init(&bginfo->lock, NULL);
	pthread_cond_init(&bginfo->init_done, NULL);
	bginfo->disksize  = disksize;
	bginfo->ctlpath   = bgctlpath;

	if (!bginfo->ctlpath)
		return;

	g_message("bgctl is on");
	// unlink(bginfo->ctlpath);


	/* mutex_lock must come before pthread_create. Before cond_wait() is
	 * called, cond_signal() cannot notify anyone. Without mutex here, if
	 * bgthread finishes rapidly before cond_wait() is called, cond_wait()
	 * never wakes up. */
	pthread_mutex_lock(&bginfo->lock);

	int ret = pthread_create(&bginfo->tid, NULL, bgctl_thread_main, bginfo);
	if (ret < 0)
		err("create thread");

	pthread_cond_wait(&bginfo->init_done, &bginfo->lock);
	pthread_mutex_unlock(&bginfo->lock);

	g_message("bgthread creation done");
}

void bgctl_wait_shutdown(void)
{
	struct bginfo_struct *bginfo = &bginfo_data;

	if (!bginfo->ctlpath)
		return;


	g_message("wait bgctl");
	pthread_join(bginfo->tid, NULL);
	g_message("wait done, count %d done", bginfo->count);

	pthread_mutex_destroy(&bginfo->lock);
	pthread_cond_destroy(&bginfo->init_done);
	bzero(bginfo, sizeof(struct bginfo_struct));
}


GAsyncQueue *reply_pendings;
GAsyncQueue *check_pendings;

enum xnbd_tester_rwmode {
	TESTRDONLY = 1,
	TESTWRONLY,
	TESTRDWR,
};

struct parameters {
	uint32_t nreq;
	enum xnbd_tester_rwmode rwmode;
	off_t disksize;
	int remotefd;

	int tgtdiskfd;
};

void *sender_thread_main(void *data)
{
	struct parameters *params = (struct parameters *) data;


	for (uint32_t index = 0; index < params->nreq; index++) {
		struct crequest *req = g_malloc0(sizeof(struct crequest));
		dbg("address %p", req);

		if (params->rwmode == TESTRDWR)
			if (random() % 2)
				req->iotype = NBD_CMD_READ;
			else
				req->iotype = NBD_CMD_WRITE;
		else if (params->rwmode == TESTRDONLY)
			req->iotype = NBD_CMD_READ;
		else if (params->rwmode == TESTWRONLY)
			req->iotype = NBD_CMD_WRITE;
		else
			err("unkown rwmode");


		req->iofrom = (off_t) (1.0L * params->disksize * random() / RAND_MAX);

		size_t tmp_iolen = (size_t) (1 + 10000.0L * random() / RAND_MAX);
		/*
		 * MIN() is a macro. So, calling random() in its
		 * argument may result in twice calling of it.
		 **/
		req->iolen  = (size_t) MIN((off_t) tmp_iolen, params->disksize - req->iofrom);

		g_message("index %d req %p iotype %s iofrom %ju iolen %u", index, req,
				(req->iotype == NBD_CMD_READ) ? "read" : "write",
				req->iofrom, req->iolen);

		g_assert(req->iofrom + req->iolen <= params->disksize);

		nbd_client_send_request_header(params->remotefd, req->iotype, req->iofrom, req->iolen, (uint64_t) index);

		if (req->iotype == NBD_CMD_WRITE) {
			req->write_buff = g_malloc(req->iolen);
			fill_random(req->write_buff, req->iolen);
			net_send_all_or_abort(params->remotefd, req->write_buff, req->iolen);
		}

		req->index = index;
		g_async_queue_push(reply_pendings, req);

		poll(NULL, 0, (int) (10.0L * random() / RAND_MAX));
	}



	g_async_queue_push(reply_pendings, &eofmarker);

	g_message("%d requests were sent", params->nreq);

	return NULL;
}

void recv_reply_header(int remotefd, uint64_t expected_index)
{
	struct nbd_reply reply;
	bzero(&reply, sizeof(reply));


	net_recv_all_or_abort(remotefd, &reply, sizeof(reply));

	if (ntohl(reply.magic) != NBD_REPLY_MAGIC)
		err("unknown reply magic, %x %x", reply.magic, ntohl(reply.magic));

	uint32_t error = ntohl(reply.error);
	if (error)
		err("reply state error %d", error);

	uint64_t reply_index = 0;
	memcpy(&reply_index, reply.handle, 8);


	dbg("index %ju", reply_index);

	if (reply_index != expected_index)
		err("wrong reply ordering");
}

void *receiver_thread_main(void *data)
{
	struct parameters *params = (struct parameters *) data;

	for (;;) {
		struct crequest *req = g_async_queue_pop(reply_pendings);
		if (req == &eofmarker)
			break;

		recv_reply_header(params->remotefd, req->index);


		dbg("req %p index %d iofrom %ju iolen %u", req, req->index, req->iofrom, req->iolen);


		struct mmap_partial *tgtmp = mmap_partial_map(params->tgtdiskfd, req->iofrom, req->iolen, 0);

		if (req->iotype == NBD_CMD_WRITE) {
			g_message("index %d req %p write done", req->index, req);
			memcpy(tgtmp->iobuf, req->write_buff, req->iolen);


		} else if (req->iotype == NBD_CMD_READ) {
			net_recv_all_or_abort(params->remotefd, tgtmp->iobuf, req->iolen);
			g_message("index %d req %p read done", req->index, req);

		} else
			err("bug");

		mmap_partial_unmap(tgtmp);


		g_async_queue_push(check_pendings, req);
	}

	g_async_queue_push(check_pendings, &eofmarker);

	g_message("io finished");

	return NULL;
}

static int CoWID = 0;

int check_consistency_by_partial_mmap_for_cowdisk(char *srcdisk, int tgtdiskfd, struct crequest *req)
{
	int result = 0;

	struct disk_stack *ds = open_cow_disk(srcdisk, 0, CoWID);

	struct disk_stack_io *io = disk_stack_mmap(ds, req->iofrom, req->iolen, 1);


	struct mmap_partial *tgtmp = mmap_partial_map(tgtdiskfd, req->iofrom, req->iolen, 0);
	char *tgtiobuf = tgtmp->iobuf;

	unsigned long offset = 0;

	for (size_t i = 0; i < io->iov_size; i++) {
		int ret = memcmp(io->iov[i].iov_base, tgtiobuf + offset, io->iov[i].iov_len);
		if (ret)
			err("mismatch");

		offset += io->iov[i].iov_len;
	}

	if (req->iolen != offset)
		err("io size mismatch");

	free_disk_stack_io(io);
	close_cow_disk(ds, 0);

	mmap_partial_unmap(tgtmp);

#if 0
	if (ret) {
		g_warning("mismatch index %d iotype %s iofrom %ju iolen %u",
				req->index, (req->iotype == NBD_CMD_READ) ? "read" : "write",
				req->iofrom, req->iolen);

		unsigned long block_index_start;
		unsigned long block_index_end;
		get_io_range_index(req->iofrom, req->iolen, &block_index_start, &block_index_end);

		g_message("iofrom %ju (%ju KB), block_index_start %lu offset_in_start_block %ju",
				req->iofrom, req->iofrom / 1024,
				block_index_start, req->iofrom % CBLOCKSIZE);

		g_message("ioend %ju (%ju KB), block_index_end %lu offset_in_end_block %ju",
				req->iofrom + req->iolen, (req->iofrom + req->iolen) / 1024,
				block_index_end, (req->iofrom + req->iolen) % CBLOCKSIZE);

		g_message("srcbuf ...");
		dump_buffer_all(srciobuf, req->iolen);
		g_message("tgtbuf ...");
		dump_buffer_all(tgtiobuf, req->iolen);
		if (req->iotype == NBD_CMD_WRITE) {
			g_message("req->write_buff");
			dump_buffer_all(req->write_buff, req->iolen);
		}


		int found = 0;
		for (uint32_t j = 0; j < req->iolen; j++) {
			char x0 = *(srciobuf + j);
			char x1 = *(tgtiobuf + j);
			if (x0 != x1) {
				g_message("mismatch at %d byte, %c %c", j, x0, x1);
				found = 1;
				break;
			}
		}
		if (!found)
			g_message("not mismatched !?");

		result = -1;
	}
#endif




	return result;
}

int check_consistency_by_partial_mmap(char *srcdisk, int tgtdiskfd, struct crequest *req)
{
	int result = 0;

	int srcdiskfd = open(srcdisk, O_RDONLY);
	if (srcdiskfd < 0)
		err("open srcdisk %s", srcdisk);

	struct mmap_partial *srcmp = mmap_partial_map(srcdiskfd, req->iofrom, req->iolen, 1);
	struct mmap_partial *tgtmp = mmap_partial_map(tgtdiskfd, req->iofrom, req->iolen, 0);
	char *srciobuf = srcmp->iobuf;
	char *tgtiobuf = tgtmp->iobuf;



	int ret = memcmp(srciobuf, tgtiobuf, req->iolen);

	if (ret) {
		g_warning("mismatch index %d iotype %s iofrom %ju iolen %u",
				req->index, (req->iotype == NBD_CMD_READ) ? "read" : "write",
				req->iofrom, req->iolen);

		unsigned long block_index_start;
		unsigned long block_index_end;
		get_io_range_index(req->iofrom, req->iolen, &block_index_start, &block_index_end);

		g_message("iofrom %ju (%ju KB), block_index_start %lu offset_in_start_block %ju",
				req->iofrom, req->iofrom / 1024,
				block_index_start, req->iofrom % CBLOCKSIZE);

		g_message("ioend %ju (%ju KB), block_index_end %lu offset_in_end_block %ju",
				req->iofrom + req->iolen, (req->iofrom + req->iolen) / 1024,
				block_index_end, (req->iofrom + req->iolen) % CBLOCKSIZE);

		g_message("srcbuf ...");
		dump_buffer_all(srciobuf, req->iolen);
		g_message("tgtbuf ...");
		dump_buffer_all(tgtiobuf, req->iolen);
		if (req->iotype == NBD_CMD_WRITE) {
			g_message("req->write_buff");
			dump_buffer_all(req->write_buff, req->iolen);
		}


		int found = 0;
		for (uint32_t j = 0; j < req->iolen; j++) {
			char x0 = *(srciobuf + j);
			char x1 = *(tgtiobuf + j);
			if (x0 != x1) {
				g_message("mismatch at %d byte, %c %c", j, x0, x1);
				found = 1;
				break;
			}
		}
		if (!found)
			g_message("not mismatched !?");

		result = -1;
	}


	mmap_partial_unmap(srcmp);
	mmap_partial_unmap(tgtmp);

	close(srcdiskfd);

	return result;
}




int test_direct_mode(char *srcdisk, char *tgtdisk, int remotefd, int cowmode, enum xnbd_tester_rwmode rwmode, const char *bgctlpath)
{
	int result = 0;

	time_t now = time(NULL);
	srandom((unsigned int) now);

	reply_pendings = g_async_queue_new();
	check_pendings = g_async_queue_new();

	off_t disksize = nbd_negotiate_with_server(remotefd);
	g_message("remote disk size %ju", disksize);

	sleep(3);

	if (disksize != get_disksize_of_path(srcdisk))
		err("%s size not match to %ju", srcdisk, disksize);


	int tgtdiskfd = open(tgtdisk, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (tgtdiskfd < 0)
		err("tgt disk open %s", strerror(errno));

	int ret = ftruncate(tgtdiskfd, disksize);
	if (ret < 0)
		err("ftruncate %m");



	uint64_t testcount = 0;


	struct parameters params = {
		.nreq = 1000,
		.remotefd = remotefd,
		.disksize = disksize,
		.rwmode = rwmode,
		.tgtdiskfd = tgtdiskfd,
	};


	for (int loop_per_session = 0; loop_per_session < 100; loop_per_session++) {
		g_message("io start");

		bgctl_thread_create(disksize, bgctlpath);


		int aaa = (int) (1000.0L * random() / RAND_MAX);
		poll(NULL, 0, aaa);

		pthread_t tid_sender = pthread_create_or_abort(sender_thread_main, &params);
		pthread_t tid_receiver = pthread_create_or_abort(receiver_thread_main, &params);


		pthread_join(tid_sender, NULL);
		pthread_join(tid_receiver, NULL);

		bgctl_wait_shutdown();


		g_message("sender and receiver finished");
		/* wait here. make sure the last write is committed to the disk */
		//sleep(1);
		g_message("checking start ...");


		for (;;) {
			struct crequest *req = g_async_queue_pop(check_pendings);
			if (req == &eofmarker)
				break;

			if (cowmode)
				result = check_consistency_by_partial_mmap_for_cowdisk(srcdisk, tgtdiskfd, req);
			else
				result = check_consistency_by_partial_mmap(srcdisk, tgtdiskfd, req);
			if (result < 0)
				goto err_out;

			if (req->iotype == NBD_CMD_WRITE)
				g_free(req->write_buff);
			g_free(req);
		}



		g_message("checking done");

		g_message("## test %ju done", testcount);
		sleep(1);
		testcount +=1;
	}

err_out:
	close(tgtdiskfd);

	return result;
}



static void set_sigactions()
{
	struct sigaction act;

	bzero(&act, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, NULL);
}

#include <getopt.h>

static struct option longopts[] = {
	{"bgctlpath", required_argument, NULL, 'B'},
	{"rwmode", required_argument, NULL, 'm'},
	{"cow", required_argument, NULL, 'c'},
	{NULL, 0, NULL, 0},
};

void show_help_and_exit(const char *msg)
{
	if (msg)
		info("%s\n", msg);

	info("make xnbd-server-test xnbd-tester");
	info("For target mode");
	info("  ./xnbd-server-test --target disk1G.img --lport 8992");
	info("  ./xnbd-tester --rwmode 1 localhost 8992 disk1G.img /tmp/tmp.img");
	info(" ");
	info("For proxy mode");
	info("  ./xnbd-server-test --target disk1G.img --lport 8992");
	info("  ./xnbd-server-test --proxy localhost 8992 /tmp/disk.cache /tmp/disk.cache.bitmap --lport 8521 --bgctlprefix /tmp/xnbd-bg.ctl");
	info("  ./xnbd-tester --rwmode 1 --bgctlpath /tmp/xnbd-bg.ctl localhost 8521 /tmp/disk.cache /tmp/tmp.img");
	info(" ");
	err("See source code for detail.");
}

int main(int argc, char **argv) {
	enum xnbd_tester_rwmode rwmode  = TESTRDONLY;
	int cowmode = 0;
	char *bgctlpath = NULL;

	for (;;) {
		int c;
		int index = 0;

		c = getopt_long(argc, argv, "tphB:cm:", longopts, &index);
		if (c == -1)
			break;

		switch (c) {
			case 'm':
				rwmode = atoi(optarg);
				/* test readonly(1), writeonly(2), readwrite(3) */
				info("rw mode (%d)", rwmode);
				break;

			case 'c':
				cowmode = 1;
				CoWID = atoi(optarg);
				info("copy-on-write enabled, cowid %d", CoWID);
				break;

			case 'B':
				bgctlpath = optarg;
				info("enable background copy with %s", optarg);

				break;
		}
	}


	if (argc - optind != 4)
		show_help_and_exit("argument error");

	char *remotehost = argv[optind];
	char *remoteport = argv[optind + 1];
	char *srcpath = argv[optind + 2];     /* disk file (target), cache file (proxy) */
	char *dstpath = argv[optind + 3];     /* temporary space */


	set_sigactions();
	g_thread_init(NULL);

	/* @srcdisk: disk file for a direct mode, cache file for a redirect mode */
	/* @dstdisk: temporary space */
	g_message("srcdisk %s dstdisk %s", srcpath, dstpath);



	for (;;) {
		int remotefd = net_tcp_connect(remotehost, remoteport);

		int ret = test_direct_mode(srcpath, dstpath, remotefd, cowmode, rwmode, bgctlpath);
		if (ret < 0)
			return 1;

		nbd_client_send_disc_request(remotefd);
		close(remotefd);
	}

	return 0;
}
