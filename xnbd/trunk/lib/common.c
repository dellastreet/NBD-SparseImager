/* 
 * Copyright (C) 2008-2009 National Institute of Advanced Industrial Science and Technology
 *
 * Author: Takahiro Hirofuchi
 */
#include "common.h"



#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#if 0
static pid_t mygettid(void)
{
	return syscall(SYS_gettid);
}
#endif

#include <sys/prctl.h>
#include <stdlib.h>

#define ALERT_LEVELS            (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)

void xutil_log_handler(const gchar   *log_domain, GLogLevelFlags log_level,
		const gchar   *message, gpointer data __attribute__((unused)))
{
	GString *gstring = g_string_new(NULL);

	/* we may not call _any_ GLib functions here */

	{
		char *header = getenv("LOG_HEADER");
		if (header)
			g_string_append(gstring, header);
	}

	{
		char name[20];
		int ret = prctl(PR_GET_NAME, (unsigned long) name);
		if (ret < 0)
			err("PR_GET_NAME, %m");

		g_string_append(gstring, name);
	}

	g_string_append(gstring, "(");

	{
		pid_t pid = getpid();
		pid_t tid = syscall(SYS_gettid);

		if (pid == tid)
			g_string_append_printf(gstring, "%d", pid);
		else
			g_string_append_printf(gstring, "%d.%d", pid, tid);
	}

	g_string_append(gstring, ") ");

	if (log_domain) {
		g_string_append(gstring, log_domain);
		g_string_append(gstring, ">");
	}

	switch (log_level & G_LOG_LEVEL_MASK)
	{
		case G_LOG_LEVEL_ERROR:
			g_string_append(gstring, "ERR");
			break;

		case G_LOG_LEVEL_CRITICAL:
			g_string_append(gstring, "CRIT");
			break;

		case G_LOG_LEVEL_WARNING:
			g_string_append(gstring, "WARN");
			break;

		case G_LOG_LEVEL_MESSAGE:
			g_string_append(gstring, "msg");
			break;

		case G_LOG_LEVEL_INFO:
			g_string_append(gstring, "info");
			break;

		case G_LOG_LEVEL_DEBUG:
			g_string_append(gstring, "dbg");
			break;

		default:
			g_string_append(gstring, "log");
			break;
	}



	if (log_level & G_LOG_FLAG_RECURSION)
		g_string_append(gstring, " (recursed)");

	g_string_append(gstring, ": ");

	//if (log_level & ALERT_LEVELS)
	//	g_string_append(gstring, "** ");


	if (message)
		g_string_append_printf(gstring, "%s", message);
	else
		g_string_append(gstring, "(NULL) message");


	gboolean is_fatal = (log_level & G_LOG_FLAG_FATAL) != 0;
	if (is_fatal)
		g_string_append(gstring, "\naborting...\n");
	else
		g_string_append(gstring, "\n");


	//struct xnbd_info *xnbd = (struct xnbd_info *) data;
	//printf("%d %d\n", gstring->len, strlen(gstring->str));

	write(2, gstring->str, gstring->len);

	g_string_free(gstring, TRUE);
}
