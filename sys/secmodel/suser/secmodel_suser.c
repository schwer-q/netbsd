/* $NetBSD: secmodel_suser.c,v 1.31 2009/11/14 18:36:56 elad Exp $ */
/*-
 * Copyright (c) 2006 Elad Efrat <elad@NetBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file contains kauth(9) listeners needed to implement the traditional
 * NetBSD superuser access restrictions.
 *
 * There are two main resources a request can be issued to: user-owned and
 * system owned. For the first, traditional Unix access checks are done, as
 * well as superuser checks. If needed, the request context is examined before
 * a decision is made. For the latter, usually only superuser checks are done
 * as normal users are not allowed to access system resources.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: secmodel_suser.c,v 1.31 2009/11/14 18:36:56 elad Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/kauth.h>

#include <sys/mutex.h>
#include <sys/mount.h>
#include <sys/socketvar.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/uidinfo.h>
#include <sys/module.h>

#include <secmodel/suser/suser.h>

MODULE(MODULE_CLASS_SECMODEL, suser, NULL);

static int secmodel_suser_curtain;
/* static */ int dovfsusermount;

static kauth_listener_t l_generic, l_system, l_process, l_network, l_machdep,
    l_device, l_vnode;

static struct sysctllog *suser_sysctl_log;

void
sysctl_security_suser_setup(struct sysctllog **clog)
{
	const struct sysctlnode *rnode;

	sysctl_createv(clog, 0, NULL, &rnode,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "security", NULL,
		       NULL, 0, NULL, 0,
		       CTL_SECURITY, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, &rnode,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "models", NULL,
		       NULL, 0, NULL, 0,
		       CTL_CREATE, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, &rnode,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "suser", NULL,
		       NULL, 0, NULL, 0,
		       CTL_CREATE, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, NULL,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_STRING, "name", NULL,
		       NULL, 0, __UNCONST("Traditional NetBSD: Superuser"), 0,
		       CTL_CREATE, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "curtain",
		       SYSCTL_DESCR("Curtain information about objects to "\
		       		    "users not owning them."),
		       NULL, 0, &secmodel_suser_curtain, 0,
		       CTL_CREATE, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "usermount",
		       SYSCTL_DESCR("Whether unprivileged users may mount "
				    "filesystems"),
		       NULL, 0, &dovfsusermount, 0,
		       CTL_CREATE, CTL_EOL);

	/* Compatibility: security.curtain */
	sysctl_createv(clog, 0, NULL, &rnode,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "security", NULL,
		       NULL, 0, NULL, 0,
		       CTL_SECURITY, CTL_EOL);

	sysctl_createv(clog, 0, &rnode, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "curtain",
		       SYSCTL_DESCR("Curtain information about objects to "\
		       		    "users not owning them."),
		       NULL, 0, &secmodel_suser_curtain, 0,
		       CTL_CREATE, CTL_EOL);

	/* Compatibility: vfs.generic.usermount */
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "vfs", NULL,
		       NULL, 0, NULL, 0,
		       CTL_VFS, CTL_EOL);

	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "generic",
		       SYSCTL_DESCR("Non-specific vfs related information"),
		       NULL, 0, NULL, 0,
		       CTL_VFS, VFS_GENERIC, CTL_EOL);

	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "usermount",
		       SYSCTL_DESCR("Whether unprivileged users may mount "
				    "filesystems"),
		       NULL, 0, &dovfsusermount, 0,
		       CTL_VFS, VFS_GENERIC, VFS_USERMOUNT, CTL_EOL);
}

void
secmodel_suser_init(void)
{
	secmodel_suser_curtain = 0;
}

void
secmodel_suser_start(void)
{
	l_generic = kauth_listen_scope(KAUTH_SCOPE_GENERIC,
	    secmodel_suser_generic_cb, NULL);
	l_system = kauth_listen_scope(KAUTH_SCOPE_SYSTEM,
	    secmodel_suser_system_cb, NULL);
	l_process = kauth_listen_scope(KAUTH_SCOPE_PROCESS,
	    secmodel_suser_process_cb, NULL);
	l_network = kauth_listen_scope(KAUTH_SCOPE_NETWORK,
	    secmodel_suser_network_cb, NULL);
	l_machdep = kauth_listen_scope(KAUTH_SCOPE_MACHDEP,
	    secmodel_suser_machdep_cb, NULL);
	l_device = kauth_listen_scope(KAUTH_SCOPE_DEVICE,
	    secmodel_suser_device_cb, NULL);
	l_vnode = kauth_listen_scope(KAUTH_SCOPE_VNODE,
	    secmodel_suser_vnode_cb, NULL);
}

void
secmodel_suser_stop(void)
{
	kauth_unlisten_scope(l_generic);
	kauth_unlisten_scope(l_system);
	kauth_unlisten_scope(l_process);
	kauth_unlisten_scope(l_network);
	kauth_unlisten_scope(l_machdep);
	kauth_unlisten_scope(l_device);
	kauth_unlisten_scope(l_vnode);
}

static int
suser_modcmd(modcmd_t cmd, void *arg)
{
	int error = 0;

	switch (cmd) {
	case MODULE_CMD_INIT:
		secmodel_suser_init();
		secmodel_suser_start();
		sysctl_security_suser_setup(&suser_sysctl_log);
		break;

	case MODULE_CMD_FINI:
		sysctl_teardown(&suser_sysctl_log);
		secmodel_suser_stop();
		break;

	case MODULE_CMD_AUTOUNLOAD:
		error = EPERM;
		break;

	default:
		error = ENOTTY;
		break;
	}

	return (error);
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: Generic
 * Responsibility: Superuser access
 */
int
secmodel_suser_generic_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
	bool isroot;
	int result;

	isroot = (kauth_cred_geteuid(cred) == 0);
	result = KAUTH_RESULT_DEFER;

	switch (action) {
	case KAUTH_GENERIC_ISSUSER:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;
		break;

	case KAUTH_GENERIC_CANSEE:     
		if (!secmodel_suser_curtain)
			result = KAUTH_RESULT_ALLOW;
		else if (isroot || kauth_cred_uidmatch(cred, arg0))
			result = KAUTH_RESULT_ALLOW;

		break;

	default:
		break;
	}

	return (result);
}

static int
suser_usermount_policy(kauth_cred_t cred, enum kauth_system_req req, void *arg1,
    void *arg2)
{
	struct mount *mp;
	u_long flags;
	int result;

	result = KAUTH_RESULT_DEFER;

	if (!dovfsusermount)
		return result;

	switch (req) {
	case KAUTH_REQ_SYSTEM_MOUNT_NEW:
		mp = ((struct vnode *)arg1)->v_mount;
		flags= (u_long)arg2;

		if (usermount_common_policy(mp, flags) != 0)
			break;

		result = KAUTH_RESULT_ALLOW;
			
		break;

	case KAUTH_REQ_SYSTEM_MOUNT_UNMOUNT:
		mp = arg1;

		/* Must own the mount. */
		if (mp->mnt_stat.f_owner != kauth_cred_geteuid(cred))
			break;

		result = KAUTH_RESULT_ALLOW;

		break;

	case KAUTH_REQ_SYSTEM_MOUNT_UPDATE:
		mp = arg1;
		flags = (u_long)arg2;

		/* Must own the mount. */
		if (mp->mnt_stat.f_owner != kauth_cred_geteuid(cred))
			break;

		if (usermount_common_policy(mp, flags) != 0)
			break;

		result = KAUTH_RESULT_ALLOW;

		break;

	default:
		break;
	}

	return result;
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: System
 * Responsibility: Superuser access
 */
int
secmodel_suser_system_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
	bool isroot;
	int result;
	enum kauth_system_req req;

	isroot = (kauth_cred_geteuid(cred) == 0);
	result = KAUTH_RESULT_DEFER;
	req = (enum kauth_system_req)arg0;

	switch (action) {
	case KAUTH_SYSTEM_CPU:
		switch (req) {
		case KAUTH_REQ_SYSTEM_CPU_SETSTATE:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_FS_QUOTA:
		switch (req) {
		case KAUTH_REQ_SYSTEM_FS_QUOTA_GET:
		case KAUTH_REQ_SYSTEM_FS_QUOTA_ONOFF:
		case KAUTH_REQ_SYSTEM_FS_QUOTA_MANAGE:
		case KAUTH_REQ_SYSTEM_FS_QUOTA_NOLIMIT:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_MOUNT:
		switch (req) {
		case KAUTH_REQ_SYSTEM_MOUNT_GET:
			if (isroot) {
				result = KAUTH_RESULT_ALLOW;
				break;
			}

			break;

		case KAUTH_REQ_SYSTEM_MOUNT_NEW:
		case KAUTH_REQ_SYSTEM_MOUNT_UNMOUNT:
		case KAUTH_REQ_SYSTEM_MOUNT_UPDATE:
			if (isroot) {
				result = KAUTH_RESULT_ALLOW;
				break;
			}

			result = suser_usermount_policy(cred, req, arg1, arg2);

			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_PSET:
		switch (req) {
		case KAUTH_REQ_SYSTEM_PSET_ASSIGN:
		case KAUTH_REQ_SYSTEM_PSET_BIND:
		case KAUTH_REQ_SYSTEM_PSET_CREATE:
		case KAUTH_REQ_SYSTEM_PSET_DESTROY:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_TIME:
		switch (req) {
		case KAUTH_REQ_SYSTEM_TIME_ADJTIME:
		case KAUTH_REQ_SYSTEM_TIME_NTPADJTIME:
		case KAUTH_REQ_SYSTEM_TIME_TIMECOUNTERS:
		case KAUTH_REQ_SYSTEM_TIME_SYSTEM:
		case KAUTH_REQ_SYSTEM_TIME_RTCOFFSET:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}
		break;

	case KAUTH_SYSTEM_SYSCTL:
		switch (req) {
		case KAUTH_REQ_SYSTEM_SYSCTL_ADD:
		case KAUTH_REQ_SYSTEM_SYSCTL_DELETE:
		case KAUTH_REQ_SYSTEM_SYSCTL_DESC:
		case KAUTH_REQ_SYSTEM_SYSCTL_MODIFY:
		case KAUTH_REQ_SYSTEM_SYSCTL_PRVT:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_SWAPCTL:
	case KAUTH_SYSTEM_ACCOUNTING:
	case KAUTH_SYSTEM_REBOOT:
	case KAUTH_SYSTEM_CHROOT:
	case KAUTH_SYSTEM_FILEHANDLE:
	case KAUTH_SYSTEM_MKNOD:
	case KAUTH_SYSTEM_SETIDCORE:
	case KAUTH_SYSTEM_MODULE:
	case KAUTH_SYSTEM_FS_RESERVEDSPACE:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;
		break;

	case KAUTH_SYSTEM_DEBUG:
		switch (req) {
		case KAUTH_REQ_SYSTEM_DEBUG_IPKDB:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}

		break;

	case KAUTH_SYSTEM_CHSYSFLAGS:
		/*
		 * Needs to be checked in conjunction with the immutable and
		 * append-only flags (usually). Should be handled differently.
		 * Infects ufs, ext2fs, tmpfs, and rump.
		 */
		if (isroot)
			result = KAUTH_RESULT_ALLOW;

		break;

	default:
		break;
	}

	return (result);
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: Process
 * Responsibility: Superuser access
 */
int
secmodel_suser_process_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
	struct proc *p;
	bool isroot;
	int result;

	isroot = (kauth_cred_geteuid(cred) == 0);
	result = KAUTH_RESULT_DEFER;
	p = arg0;

	switch (action) {
	case KAUTH_PROCESS_SIGNAL:
	case KAUTH_PROCESS_KTRACE:
	case KAUTH_PROCESS_PROCFS:
	case KAUTH_PROCESS_PTRACE:
	case KAUTH_PROCESS_SCHEDULER_GETPARAM:
	case KAUTH_PROCESS_SCHEDULER_SETPARAM:
	case KAUTH_PROCESS_SCHEDULER_GETAFFINITY:
	case KAUTH_PROCESS_SCHEDULER_SETAFFINITY:
	case KAUTH_PROCESS_SETID:
	case KAUTH_PROCESS_KEVENT_FILTER:
	case KAUTH_PROCESS_NICE:
	case KAUTH_PROCESS_FORK:
	case KAUTH_PROCESS_CORENAME:
	case KAUTH_PROCESS_STOPFLAG:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;

		break;

	case KAUTH_PROCESS_CANSEE: {
		unsigned long req;

		req = (unsigned long)arg1;

		switch (req) {
		case KAUTH_REQ_PROCESS_CANSEE_ARGS:
		case KAUTH_REQ_PROCESS_CANSEE_ENTRY:
		case KAUTH_REQ_PROCESS_CANSEE_OPENFILES:
			if (isroot) {
				result = KAUTH_RESULT_ALLOW;
				break;
			}

			if (secmodel_suser_curtain) {
				if (kauth_cred_uidmatch(cred, p->p_cred) != 0)
					result = KAUTH_RESULT_DENY;
			}

			break;

		case KAUTH_REQ_PROCESS_CANSEE_ENV:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}

		break;
		}

	case KAUTH_PROCESS_RLIMIT: {
		enum kauth_process_req req;

		req = (enum kauth_process_req)(unsigned long)arg1;

		switch (req) {
		case KAUTH_REQ_PROCESS_RLIMIT_SET:
		case KAUTH_REQ_PROCESS_RLIMIT_GET:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}

		break;
		}

	default:
		break;
	}

	return (result);
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: Network
 * Responsibility: Superuser access
 */
int
secmodel_suser_network_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
	bool isroot;
	int result;
	enum kauth_network_req req;

	isroot = (kauth_cred_geteuid(cred) == 0);
	result = KAUTH_RESULT_DEFER;
	req = (enum kauth_network_req)arg0;

	switch (action) {
	case KAUTH_NETWORK_ALTQ:
		switch (req) {
		case KAUTH_REQ_NETWORK_ALTQ_AFMAP:
		case KAUTH_REQ_NETWORK_ALTQ_BLUE:
		case KAUTH_REQ_NETWORK_ALTQ_CBQ:
		case KAUTH_REQ_NETWORK_ALTQ_CDNR:
		case KAUTH_REQ_NETWORK_ALTQ_CONF:
		case KAUTH_REQ_NETWORK_ALTQ_FIFOQ:
		case KAUTH_REQ_NETWORK_ALTQ_HFSC:
		case KAUTH_REQ_NETWORK_ALTQ_JOBS:
		case KAUTH_REQ_NETWORK_ALTQ_PRIQ:
		case KAUTH_REQ_NETWORK_ALTQ_RED:
		case KAUTH_REQ_NETWORK_ALTQ_RIO:
		case KAUTH_REQ_NETWORK_ALTQ_WFQ:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_NETWORK_BIND:
		switch (req) {
		case KAUTH_REQ_NETWORK_BIND_PORT:
		case KAUTH_REQ_NETWORK_BIND_PRIVPORT:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}
		break;

	case KAUTH_NETWORK_FIREWALL:
		switch (req) {
		case KAUTH_REQ_NETWORK_FIREWALL_FW:
		case KAUTH_REQ_NETWORK_FIREWALL_NAT:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}
		break;

	case KAUTH_NETWORK_FORWSRCRT:
	case KAUTH_NETWORK_ROUTE:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;

		break;

	case KAUTH_NETWORK_INTERFACE:
		switch (req) {
		case KAUTH_REQ_NETWORK_INTERFACE_GET:
		case KAUTH_REQ_NETWORK_INTERFACE_SET:
		case KAUTH_REQ_NETWORK_INTERFACE_GETPRIV:
		case KAUTH_REQ_NETWORK_INTERFACE_SETPRIV:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}
		break;

	case KAUTH_NETWORK_INTERFACE_PPP:
		switch (req) {
		case KAUTH_REQ_NETWORK_INTERFACE_PPP_ADD:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_NETWORK_INTERFACE_SLIP:
		switch (req) {
		case KAUTH_REQ_NETWORK_INTERFACE_SLIP_ADD:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_NETWORK_INTERFACE_STRIP:
		switch (req) {
		case KAUTH_REQ_NETWORK_INTERFACE_STRIP_ADD:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_NETWORK_INTERFACE_TUN:
		switch (req) {
		case KAUTH_REQ_NETWORK_INTERFACE_TUN_ADD:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;

	case KAUTH_NETWORK_NFS:
		switch (req) {
		case KAUTH_REQ_NETWORK_NFS_EXPORT:
		case KAUTH_REQ_NETWORK_NFS_SVC:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;

			break;

		default:
			break;
		}
		break;

	case KAUTH_NETWORK_SOCKET:
		switch (req) {
		case KAUTH_REQ_NETWORK_SOCKET_DROP:
		case KAUTH_REQ_NETWORK_SOCKET_OPEN:
		case KAUTH_REQ_NETWORK_SOCKET_RAWSOCK:
		case KAUTH_REQ_NETWORK_SOCKET_SETPRIV:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		case KAUTH_REQ_NETWORK_SOCKET_CANSEE:
			if (isroot) {
				result = KAUTH_RESULT_ALLOW;
				break;
			}

			if (secmodel_suser_curtain) {
				struct socket *so;
				uid_t so_uid;

				so = (struct socket *)arg1;
				so_uid = so->so_uidinfo->ui_uid;
				if (kauth_cred_geteuid(cred) != so_uid)
					result = KAUTH_RESULT_DENY;
			}

			break;

		default:
			break;
		}

		break;


	default:
		break;
	}

	return (result);
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: Machdep
 * Responsibility: Superuser access
 */
int
secmodel_suser_machdep_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
        bool isroot;
        int result;

        isroot = (kauth_cred_geteuid(cred) == 0);
        result = KAUTH_RESULT_DEFER;

        switch (action) {
	case KAUTH_MACHDEP_IOPERM_GET:
	case KAUTH_MACHDEP_LDT_GET:
	case KAUTH_MACHDEP_LDT_SET:
	case KAUTH_MACHDEP_MTRR_GET:
	case KAUTH_MACHDEP_CACHEFLUSH:
	case KAUTH_MACHDEP_IOPERM_SET:
	case KAUTH_MACHDEP_IOPL:
	case KAUTH_MACHDEP_MTRR_SET:
	case KAUTH_MACHDEP_NVRAM:
	case KAUTH_MACHDEP_UNMANAGEDMEM:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;
		break;

	default:
		break;
	}

	return (result);
}

/*
 * kauth(9) listener
 *
 * Security model: Traditional NetBSD
 * Scope: Device
 * Responsibility: Superuser access
 */
int
secmodel_suser_device_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
        bool isroot;
        int result;

        isroot = (kauth_cred_geteuid(cred) == 0);
        result = KAUTH_RESULT_DEFER;

	switch (action) {
	case KAUTH_DEVICE_BLUETOOTH_SETPRIV:
	case KAUTH_DEVICE_BLUETOOTH_SEND:
	case KAUTH_DEVICE_BLUETOOTH_RECV:
	case KAUTH_DEVICE_TTY_OPEN:
	case KAUTH_DEVICE_TTY_PRIVSET:
	case KAUTH_DEVICE_TTY_STI:
	case KAUTH_DEVICE_RND_ADDDATA:
	case KAUTH_DEVICE_RND_GETPRIV:
	case KAUTH_DEVICE_RND_SETPRIV:
		if (isroot)
			result = KAUTH_RESULT_ALLOW;
		break;

	case KAUTH_DEVICE_BLUETOOTH_BCSP:
	case KAUTH_DEVICE_BLUETOOTH_BTUART: {
		enum kauth_device_req req;

		req = (enum kauth_device_req)arg0;
		switch (req) {
		case KAUTH_REQ_DEVICE_BLUETOOTH_BCSP_ADD:
		case KAUTH_REQ_DEVICE_BLUETOOTH_BTUART_ADD:
			if (isroot)
				result = KAUTH_RESULT_ALLOW;
			break;

		default:
			break;
		}

		break;
		}

	case KAUTH_DEVICE_GPIO_PINSET:
		/*
		 * root can access gpio pins, secmodel_securlevel can veto
		 * this decision.
		 */
		if (isroot)
			result = KAUTH_RESULT_ALLOW;
		break;

	default:
		break;
	}

	return (result);
}

int
secmodel_suser_vnode_cb(kauth_cred_t cred, kauth_action_t action,
    void *cookie, void *arg0, void *arg1, void *arg2, void *arg3)
{
	bool isroot;
	int result;

	isroot = (kauth_cred_geteuid(cred) == 0);
	result = KAUTH_RESULT_DEFER;

	if (isroot)
		result = KAUTH_RESULT_ALLOW;

	return (result);
}

