/* $NetBSD: wsmoused.c,v 1.9 2003/03/04 22:31:15 jmmv Exp $ */

/*
 * Copyright (c) 2002, 2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Julio Merino.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name authors may not be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#ifndef lint
__RCSID("$NetBSD: wsmoused.c,v 1.9 2003/03/04 22:31:15 jmmv Exp $");
#endif /* not lint */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/tty.h>
#include <dev/wscons/wsconsio.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <util.h>

#include "pathnames.h"
#include "wsmoused.h"

#define IS_MOTION_EVENT(type) (((type) == WSCONS_EVENT_MOUSE_DELTA_X) || \
			       ((type) == WSCONS_EVENT_MOUSE_DELTA_Y) || \
			       ((type) == WSCONS_EVENT_MOUSE_DELTA_Z))
#define IS_BUTTON_EVENT(type) (((type) == WSCONS_EVENT_MOUSE_UP) || \
			       ((type) == WSCONS_EVENT_MOUSE_DOWN))

static struct mouse mouse;
static char *PidFile = NULL;
int XConsole = -1;

/*
 * Show program usage information
 */
static void
usage(void)
{
	(void)fprintf(stderr,
	    "Usage: %s [-d device] [-f config_file] [-n]\n",
	    getprogname());
	exit(EXIT_FAILURE);
}

/*
 * Handler for close signals
 */
static void
signal_terminate(int sig)
{
	mouse_cursor_hide(&mouse);
	config_free();
	exit(EXIT_SUCCESS);
}

/*
 * Open the mouse device (i.e. /dev/wsmouse). The argument secs
 * specifies the number of seconds we must wait before opening the
 * device. This is used when returning from X. See mouse_open_tty().
 */
static void
mouse_open_device(struct mouse *m, int secs)
{
	if (m->fd != -1) return;

	sleep(secs);

	/* Open mouse file descriptor */
	m->fd = open(m->device_name,
	             O_RDONLY | O_NONBLOCK, 0);
	if (m->fd == -1) {
		err(EXIT_FAILURE, "cannot open %s", m->device_name);
	}
}

/*
 * Prepare mouse file descriptors
 */
static void
open_files(void)
{
	/* Open wsdisplay status device */
	mouse.stat_fd = open(_PATH_TTYSTAT, O_RDONLY | O_NONBLOCK, 0);
	if (mouse.stat_fd == -1)
		err(EXIT_FAILURE, "cannot open %s", mouse.tstat_name);

	mouse.fd = -1;
	mouse_open_device(&mouse, 0);

	/* Open FIFO, if wanted */
	mouse.fifo_fd = -1;
	if (mouse.fifo_name != NULL) {
		mouse.fifo_fd = open(mouse.fifo_name, O_RDWR | O_NONBLOCK, 0);
		if (mouse.fifo_fd == -1)
			err(EXIT_FAILURE, "cannot open %s", mouse.fifo_name);
	}
}

/*
 * Mouse event loop
 */
static void
event_loop(void)
{
	int res;
	struct pollfd fds[2];
	struct wscons_event event;

	fds[0].fd = mouse.stat_fd;
	fds[0].events = POLLIN;

	for (;;) {
		fds[1].fd = mouse.fd;
		fds[1].events = POLLIN;
		if (mouse.disabled)
			res = poll(fds, 1, INFTIM);
		else
			res = poll(fds, 2, 300);

		if (res < 0)
			warn("failed to read from devices");

		if (fds[0].revents & POLLIN) {
			res = read(mouse.stat_fd, &event, sizeof(event));
			if (res != sizeof(event))
				warn("failed to read from mouse stat");
			screen_event(&mouse, &event);
		} else if (fds[1].revents & POLLIN) {
			res = read(mouse.fd, &event, sizeof(event));
			if (res != sizeof(event))
				warn("failed to read from mouse");

			if (mouse.fifo_fd >= 0) {
				res = write(mouse.fifo_fd, &event,
				            sizeof(event));
				if (res != sizeof(event))
					warn("failed to write to fifo");
			}

			if (IS_MOTION_EVENT(event.type)) {
				mouse_motion_event(&mouse, &event);
			} else if (IS_BUTTON_EVENT(event.type)) {
				mouse_button_event(&mouse, &event);
			} else {
				warn("unknown wsmouse event");
			}
		} else
			if (!mouse.selecting) mouse_cursor_hide(&mouse);
	}
}

/*
 * Initializes mouse structure coordinate information. The mouse will
 * be displayed at the center of the screen at start.
 */
static void
mouse_init(void)
{
	struct winsize ws;
	struct wsdisplay_char ch;
	int i;

	/* Get terminal size */
	if (ioctl(0, TIOCGWINSZ, &ws) < 0)
		err(EXIT_FAILURE, "cannot get terminal size");

	/* Open current tty */
	ioctl(mouse.stat_fd, WSDISPLAYIO_GETACTIVESCREEN, &i);
	mouse.tty_fd = -1;
	mouse_open_tty(&mouse, i);

	/* Check if the kernel has character functions */
	ch.row = ch.col = 0;
	if (ioctl(mouse.tty_fd, WSDISPLAYIO_GETWSCHAR, &ch) < 0)
		err(EXIT_FAILURE, "ioctl(WSDISPLAYIO_GETWSCHAR) failed");

	mouse.max_row = ws.ws_row - 1;
	mouse.max_col = ws.ws_col - 1;
	mouse.row = mouse.max_row / 2;
	mouse.col = mouse.max_col / 2;
	mouse.count_row = 0;
	mouse.count_col = 0;
	mouse.cursor = 0;
	mouse.selecting = 0;
}

/*
 * Hides the mouse cursor
 */
void
mouse_cursor_hide(struct mouse *m)
{
	if (!m->cursor) return;
	char_invert(m, m->row, m->col);
	m->cursor = 0;
}

/*
 * Shows the mouse cursor
 */
void
mouse_cursor_show(struct mouse *m)
{
	if (m->cursor) return;
	char_invert(m, m->row, m->col);
	m->cursor = 1;
}

/*
 * Opens the specified tty (we want it for ioctl's). If tty_fd in
 * mouse structure is -1, no close is performed. Otherwise, the old
 * file descriptor is closed and the new one opened.
 */
void
mouse_open_tty(struct mouse *m, int ttyno)
{
	char buf[20];

	if (m->tty_fd >= 0) close(m->tty_fd);
	if (ttyno == XConsole) {
		m->disabled = 1;
		(void)close(m->fd);
		m->fd = -1;
		return;
	}
	/* Open with delay. When returning from X, wsmoused keeps busy
	   some seconds so we have to wait. */
	mouse_open_device(m, 5);
	(void)snprintf(buf, sizeof(buf), _PATH_TTYPREFIX "%d", ttyno);
	m->tty_fd = open(buf, O_RDONLY | O_NONBLOCK);
	if (m->tty_fd < 0)
		errx(EXIT_FAILURE, "cannot open %s", buf);
	m->disabled = 0;
}

/*
 * Flip the foreground and background colors on char at coordinates
 */
void
char_invert(struct mouse *m, size_t row, size_t col)
{
	struct wsdisplay_char ch;
	int t;

	ch.row = row;
	ch.col = col;

	if (ioctl(m->tty_fd, WSDISPLAYIO_GETWSCHAR, &ch) == -1) {
		warn("ioctl(WSDISPLAYIO_GETWSCHAR) failed");
		return;
	}

	t = ch.foreground;
	ch.foreground = ch.background;
	ch.background = t;

	if (ioctl(m->tty_fd, WSDISPLAYIO_PUTWSCHAR, &ch) == -1)
		warn("ioctl(WSDISPLAYIO_PUTWSCHAR) failed");
}

/*
 * Main function
 */
int
main(int argc, char **argv)
{
	int opt, nodaemon = -1;
	int needconf = 0;
	char *conffile;
	struct block *mode;

	setprogname(argv[0]);

	memset(&mouse, 0, sizeof(struct mouse));
	conffile = _PATH_CONF;

	/* Parse command line options */
	while ((opt = getopt(argc, argv, "d:f:n")) != -1) {
		switch (opt) {
		case 'd': /* Mouse device name */
			mouse.device_name = optarg;
			break;
		case 'f': /* Configuration file name */
			needconf = 1;
			conffile = optarg;
			break;
		case 'n': /* No daemon */
			nodaemon = 1;
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}

	/* Read the configuration file and get our mode configuration */
	config_read(conffile, needconf);
	mode = config_get_mode("sel");

	/* Set values according to the configuration file */
	if (mouse.device_name == NULL)
		mouse.device_name = block_get_propval(mode, "device",
		    _PATH_DEFAULT_MOUSE);

	if (nodaemon == -1)
		nodaemon = block_get_propval_int(mode, "nodaemon", 0);

	mouse.slowdown_x = block_get_propval_int(mode, "slowdown_x", 0);
	mouse.slowdown_y = block_get_propval_int(mode, "slowdown_y", 3);
	mouse.tstat_name = block_get_propval(mode, "ttystat", _PATH_TTYSTAT);
	XConsole = block_get_propval_int(mode, "xconsole", -1);

	if (block_get_propval_int(mode, "lefthanded", 0)) {
		mouse.but_select = 2;
		mouse.but_paste = 0;
	} else {
		mouse.but_select = 0;
		mouse.but_paste = 2;
	}

	open_files();
	mouse_init();
	mouse_sel_init();

	/* Setup signal handlers */
	(void)signal(SIGINT,  signal_terminate);
	(void)signal(SIGKILL, signal_terminate);
	(void)signal(SIGQUIT, signal_terminate);
	(void)signal(SIGTERM, signal_terminate);

	if (!nodaemon) {
		/* Become a daemon */
		if (daemon(0, 0) == -1)
			err(EXIT_FAILURE, "failed to become a daemon");

		/* Create the pidfile, if wanted */
		PidFile = block_get_propval(mode, "pidfile", NULL);
		if (pidfile(PidFile) == -1)
			warn("pidfile %s", PidFile);
	}
	event_loop();

	/* NOTREACHED */
	return EXIT_SUCCESS;
}
