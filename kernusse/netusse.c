/*
 * netusse.c - fucking kernel net stacks fuzzer
 *
 * TODO:
 *  - recvfrom()
 *  - recvmsg()
 *
 * Copyright (c) Clément Lecigne, 2006-2009
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <net/if_arp.h>
#ifdef __linux__
#include <sys/klog.h>
#include <linux/atalk.h>
#include <linux/in.h>
#include <linux/can.h>
#else
#include <netinet/in.h>
#include <sys/uio.h>
#endif

//#define DEBUG_SENDMSG 1

#define SEED_FILE "netusse.seed"

#ifndef LLC_SAP_NULL
#define LLC_SAP_NULL 0x00
#endif

#ifndef __LLC_SOCK_SIZE__
#define __LLC_SOCK_SIZE__ 16
struct sockaddr_llc {
	sa_family_t     sllc_family;
	sa_family_t     sllc_arphrd;
	unsigned char   sllc_test;
	unsigned char   sllc_xid;
	unsigned char   sllc_ua;
	unsigned char   sllc_sap;
	unsigned char   sllc_mac[6];
	unsigned char   __pad[__LLC_SOCK_SIZE__ - sizeof(sa_family_t) * 2 -
	                      sizeof(unsigned char) * 4 - 6];
};
#endif

int snum = -1;

static void fuzzer(char *mm, size_t mm_size)
{
    size_t i;

    for ( i = 0 ; i < mm_size ; i++ )
    {
        if ( rand() % 10 == 0 && i < mm_size - 2 )
        {
            mm[i++] = '%';
            switch (rand() % 2)
            {
                case 0:
                    mm[i] = 'n';
                    break;
                case 1:
                    mm[i] = 'x';
                    break;
                default:
                    mm[i] = 'u';
                    break;
            }
        }
        else
        {
            mm[i] = rand() & 255;
            if (rand() % 2)
                mm[i] |= 0x80;
        }
    }

    return;
}

#ifdef __linux__
/**
 * see if linux kernel has oopsed :)
 */
static int linux_hasoopsed()
{
    char    buf[2048];
    int     sz;
    if ( (sz = klogctl(0x3, buf, 2048)) > -1 )
    {
        if (strstr(buf, "ecx: ") != NULL)
            return 1;

        /* sk_free: optmem leakage (28 bytes) detected. */
        if (strstr(buf, "leakage") != NULL)
            return 1;
    }

    return 0;
}
#endif

static void kernop(int fd)
{
    /* from Jon Oberheide sploit
    */
#ifdef __LINUX__
    const int   randcalls[] = {
        __NR_lseek, __NR_rt_sigaction, __NR_rt_sigprocmask, __NR_ioctl, 
        __NR_access, __NR_pipe, __NR_sched_yield, __NR_mremap, __NR_dup, 
        __NR_dup2, __NR_getitimer, __NR_setitimer, __NR_getpid, __NR_fcntl, 
        __NR_flock, __NR_getdents, __NR_getcwd, __NR_gettimeofday, 
        __NR_getrlimit, __NR_getuid, __NR_getgid, __NR_geteuid, __NR_getegid,
        __NR_getppid, __NR_getpgrp, __NR_getgroups, __NR_getresuid, 
        __NR_getresgid, __NR_getpgid, __NR_getsid,__NR_getpriority, 
        __NR_sched_getparam, __NR_sched_get_priority_max
    };
#endif
    const int   randsopts[] = { SOL_SOCKET };
    int         ret, len, o;
    char        buf[1024];

    do
    {
        switch ( rand() % 2 )
        {
            case 55:
#ifdef __LINUX__
                o = randcalls[rand() % sizeof(randcalls)/sizeof(randcalls[0])];
#else
                o = rand() % 64;
#endif
                ret = syscall(o);
                break;
            case 0:
            case 1:
                len = (rand() % 2) ? sizeof(int) : sizeof(buf);
                ret = getsockopt(fd, randsopts[rand() % sizeof(randsopts)/sizeof(randsopts[0])], rand() % 130, &buf, &len);
                break;
        }
    }
    while ( ret < 0 );
}

/**
 * return a random file descriptor
 */
static int getfd(void)
{
    int fd, flags;

    do {
        switch (rand() % 5)
        {
            case 0:
                fd = open("/etc/passwd", O_RDONLY);
                break;
            case 1:
                fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 2:
                fd = open("/dev/random", O_RDONLY);
                break;
            case 3:
                fd = open("/tmp/fusse", O_CREAT|O_RDWR, 0666);
                break;
            case 4:
                fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
            default:
                fd = open("/proc/self/maps", O_RDONLY);
                break;
        }
    }
    while (fd < 0);
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return fd;
}

static int evilint(void)
{
    int state;
    state = rand() % 20;
    switch ( state )
    {
        case 0:
            return rand();
            break;
        case 1:
            return( 0xffffff00 | (rand() % 256));
        case 2: return 0x8000;
        case 3: return 0xffff;
        case 4: return 0x80000000;
        case 5: return -1;
        case 6: return 0xff;
        case 7: return 0x7fffffff;
        case 8: return 0;
        case 9: return 4;
        case 10: return 8;
        case 11: return 12;
        case 12:
        case 13:
        case 14:
                 return rand() & 32;
        default:
                return rand() & 2048;

    }
}

static void dump(unsigned char * data, unsigned int len)
{
    unsigned int dp, p;
    const char trans[] =
        "................................ !\"#$%&'()*+,-./0123456789"
        ":;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm"
        "nopqrstuvwxyz{|}~...................................."
        "....................................................."
        "........................................";

    printf("\n");
    for ( dp = 1; dp <= len; dp++ )
    {
        printf("%02x ", data[dp-1]);
        if ( (dp % 8) == 0 )
        {
            printf("| ");
            p = dp;
            for ( dp -= 8; dp < p; dp++ ) {
                printf("%c", trans[data[dp]]);
            }
            printf("\n");
        }
    }

    return;
}

void ssoptusse(int s)
{
	unsigned int optval;
	int optlen, optname, level, ret, on = rand() % 2, tout = 50;
	do
	{
		switch (rand() % 15)
		{
		case 0:
			level = IPPROTO_IPV6;
			break;
		case 1:
			level = SOL_SOCKET;
			break;
		case 2:
			level = IPPROTO_UDP;
			break;
		case 3:
			level = IPPROTO_TCP;
			break;
		case 4:
			level = IPPROTO_IP;
			break;
        default:
            level = evilint();
            break;
		}

		if (rand() % 6)
		{
			optlen = evilint();
			optval = evilint();
		}
		else
		{
			optlen = sizeof (int);
			on = rand();
			optval = (unsigned int)&on;
		}

		if (rand() % 8)
			optname = rand() % 255;
		else
			optname = evilint();

#if defined(__FreeBSD__)
        /*
		 * anti well know FreeBSD mbufs exhaustion.
		 */
		if (optname == 182 || optname == 202 || optname == 254 || optname == 91 || optname == 25 || optname == IPV6_IPSEC_POLICY || 
				optname == IPV6_FW_ADD || optname == IPV6_FW_FLUSH
				|| optname == IPV6_FW_DEL || optname == IPV6_FW_ZERO)
			continue;
#endif
		ret = setsockopt(s, level, optname, (void *)optval, optlen);
	}
    while(ret == -1 && tout--);
}

void ioctlusse(int s)
{
    int          req, i, n, ret;
    char         *iav[6];
    unsigned int tout = 20;

    do
    {
        switch (rand() % 3)
        {
            case 0:
            case 1:
                req = rand() & 255;
                break;
            case 2:
                req = rand();
                break;
        }

        n = rand() % 7;
        for (i = 0; i < n; i++)
        {
            int len = rand() % 255;
            iav[i] = malloc(len);
            fuzzer(iav[i], len);
        }

        switch (n)
        {
            case 0:
                ret = ioctl(s, req);
                break;
            case 1:
                ret = ioctl(s, req, iav[0]);
                break;
            case 2:
                ret = ioctl(s, req, iav[0], iav[1]);
                break;
            case 3:
                ret = ioctl(s, req, iav[0], iav[1], iav[2]);
                break;
            case 4:
                ret = ioctl(s, req, iav[0], iav[1], iav[2], iav[3]);
                break;
            case 5:
                ret = ioctl(s, req, iav[0], iav[1], iav[2], iav[3], iav[4]);
                break;
            case 6:
                ret = ioctl(s, req, iav[0], iav[1], iav[2], iav[3], iav[4], iav[5]);
                break;
        }

        for (i = 0; i < n; i++)
            free(iav[i]);
    }
    while (ret == -1 && tout--);
}

void getsocknamusse(int s)
{
    char    buf[2048], pbuf[2048];
    int     len = 0, ret;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);

    for ( len = 0 ; len < 50 ; len++ )
    {
        int coin = len;
        ret = getsockname(s, (struct sockaddr *)&buf, &coin);
        if ( ret >= 0 )
        {
            kernop(s);
            getsockname(s, (struct sockaddr *)&pbuf, &len);
            if ( memcmp(&buf, &pbuf, (len < 0 || len > 2048) ? 2048 : len) != 0 )
            {
                printf("\nPOSSIBLE LEAK WITH :\n");
                printf("\tgetsockname(sock (%d), buf, &%d)\n", snum, len);
                len = (len < 0 || len > 2048) ? 2048 : len;
                printf("FIRST CALL:\n");
                dump(buf, len);
                printf("SECOND CALL:\n");
                dump(pbuf, len);
                getchar();
            }

            break;
        }
    }
}

void getpeernamusse(int s)
{
    char    buf[2048], pbuf[2048];
    int     len = 0, ret;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);

    for ( len = 0 ; len < 50 ; len++ )
    {
        ret = getpeername(s, (struct sockaddr *)&buf, &len);
        if ( ret >= 0 && memcmp(&buf, &pbuf, (len < 0 || len > 2048) ? 2048 : len) != 0 )
        {
            kernop(s);
            getpeername(s, (struct sockaddr *)&pbuf, &len);
            if ( memcmp(&buf, &pbuf, (len < 0 || len > 2048) ? 2048 : len) != 0 )
            {
                printf("\nPOSSIBLE LEAK WITH :\n");
                printf("\tgetpeername(sock (%d), buf, &%d)\n", snum, len);
                len = (len < 0 || len > 2048) ? 2048 : len;
                printf("FIRST CALL:\n");
                dump(buf, len);
                printf("SECOND CALL:\n");
                dump(pbuf, len);
                getchar();
            }

            break;
        }
    }
}

void gsoptusse(int s)
{
	char buf[2048], pbuf[2048], rbuf[2048];
	int optname, level, ret, len, tout;

	tout = 5;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);
    memset(&rbuf, 'A', 2048);

	do
	{
		optname = rand() % 255;
		len = evilint();
		switch (rand() % 15)
		{
		case 0:
			level = IPPROTO_IPV6;
			break;
		case 1:
			level = SOL_SOCKET;
			break;
		case 2:
			level = IPPROTO_UDP;
			break;
		case 3:
			level = IPPROTO_TCP;
			break;
		case 4:
			level = IPPROTO_IP;
			break;
        default:
            level = evilint();
            break;
		}
#if defined(__FreeBSD__)
        /*
		 * anti well know FreeBSD mbufs exhaustion.
		 */
		if (optname == 182 || optname == 202 || optname == 254 || optname == 91 || optname == 25 || optname == IPV6_IPSEC_POLICY || 
				optname == IPV6_FW_ADD || optname == IPV6_FW_FLUSH
				|| optname == IPV6_FW_DEL || optname == IPV6_FW_ZERO)
			continue;
#endif
		ret = getsockopt(s, level, optname, &buf, &len);
		tout--;
	}
    while (ret == -1 && tout);

    if (ret == -1 || tout == 0)
        return;

    /* linux false positive
     */
    if ( len == 104 && optname == 11 )
        return;

    kernop(s);

    getsockopt(s, level, optname, &pbuf, &len);

    if ( memcmp(&buf, &pbuf, (len < 0 || len > 2048) ? 2048 : len) != 0 && memcmp(&pbuf, &rbuf, (len > 2048) ? 2048 : len) != 0 )
    {
        printf("\nPOSSIBLE LEAK WITH :\n");
		printf("\tgetsockopt(sock (%d), %d, %u, buf, &%d)\n", snum, level, optname, len);
        len = (len < 0 || len > 2048) ? 2048 : len;
        printf("FIRST CALL:\n");
        dump(buf, len);
        printf("SECOND CALL:\n");
        dump(pbuf, len);
        getchar();
    }

	return;
}


/**
 * fucking bindusse #@!
 */
void bindusse(int fd)
{
    size_t  len;
    int     ret, tout = 5;
    char    *b;

    do
    {
        len = evilint();
        b = malloc(len);
        if ( b != NULL && len < 0xFFFFF )
            fuzzer(b, len);
        ret = bind(fd, (struct sockaddr *)&b, len);
        if (ret && (rand() % 2))
            listen(fd, rand());
        free(b); b = NULL;
    }
    while (ret < 0 && tout--);
}

void connectusse(int fd)
{
    int     ret, tout = 5;
    size_t  len;
    char    *b;

    do
    {
        len = evilint();
        b = malloc(len);
        if ( b != NULL && len < 0xFFFFF )
            fuzzer(b, len);
        ret = connect(fd, (struct sockaddr *)&b, len);
        if ( b ) free(b); b = NULL;
    }
    while (ret < 0 && tout--);
}

void sendtousse(int fd)
{
    char    *addr, *msg;
    size_t  alen, mlen;
    int     flags = 0;

    alen = evilint();
    addr = malloc(alen);
    if ( addr != NULL && alen < 0xFFFFF )
        fuzzer(addr, alen);

    mlen = evilint();
    msg = malloc(mlen);
    if ( msg != NULL && mlen < 0xFFFFF )
        fuzzer(msg, mlen);

    sendto(fd, msg, mlen, flags, (struct sockaddr *)addr, (socklen_t)alen);

    if ( addr ) free(addr);
    if ( msg ) free(msg);
}

#if defined(__linux__)
void sendfilusse(int fd)
{
    int i;

    for ( i = 0 ; i < 50 ; i++ )
    {
        off_t   offset;
        size_t  size;
        int     ifd, ofd;

        offset = evilint();
        size = evilint();
        ifd = evilint();
        ofd = evilint();

        switch (rand() % 5)
        {
            case 0:
                ifd = fd;
                break;
            case 1:
                ofd = fd;
                break;
            case 2:
                ifd = ofd = fd;
                break;
            case 3:
                ifd = fd;
                ofd = getfd();
                break;
            case 4:
                ofd = fd;
                ifd = getfd();
                break;
        }

        sendfile(ifd, ofd, &offset, size);
        if ( ifd != fd )
            close(ifd);
        if ( ofd != fd )
            close(ofd);
    }
}
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__)
void sendmsgusse(int fd)
{
    char name[1024], ctrl[1024], base[1024];
    struct iovec iov;
    struct msghdr msg;
    int i;

    for ( i = 0 ; i < 50 ; i++ )
    {
        fuzzer(name, 1024);
        fuzzer(base, 1024);
        fuzzer(ctrl, 1024);
        msg.msg_name    = name;
        msg.msg_namelen = evilint();
        msg.msg_iovlen  = evilint();
        msg.msg_flags   = rand() & 255;
        if ( rand() % 3 )
        {
            msg.msg_iov   = &iov;
            iov.iov_base  = base;
            iov.iov_len   = evilint();
        }
        else
            msg.msg_iov = NULL;
        msg.msg_control = ctrl;
        msg.msg_controllen = evilint();
#ifdef DEBUG_SENDMSG
        printf("sendmsg(%d, {nl = %x, iol = %x, ctl = %x}, 0);\n", fd, msg.msg_namelen, msg.msg_iovlen, msg.msg_controllen);
        getchar();
#endif
        sendmsg(fd, &msg, 0);
    }
    return;
}
#elif defined(__linux__)
void sendmsgusse(int fd)
{
	struct msghdr   msg;
	struct cmsghdr  *cmsg = NULL;
	struct iovec    iov;
    char            *b = NULL;
    int             i, flags;

    for ( i = 0 ; i < 50 ; i++ )
    {
        msg.msg_controllen = (rand() % 50) ? rand() & 0xFFFF : 0;
        if (msg.msg_controllen)
        {
            if (msg.msg_controllen < sizeof (struct cmsghdr))
                cmsg = (struct cmsghdr *)malloc(sizeof (struct cmsghdr));
            else
                cmsg = (struct cmsghdr *)malloc(msg.msg_controllen);
            if (cmsg == NULL) goto nocmsghdr;
            msg.msg_control = cmsg;
            fuzzer(cmsg, msg.msg_controllen);
            if ( rand()%10 == 0)
            {
                cmsg->cmsg_level = (rand() % 2) ? IPPROTO_IP : evilint();
                cmsg->cmsg_type = (rand() % 2) ? rand() % 255 : evilint();
                cmsg->cmsg_len = (rand() % 2) ? msg.msg_controllen : evilint();
            }
        }
        else
        {
nocmsghdr:
            msg.msg_control = (rand() % 5) ? NULL : (void*)evilint();
            msg.msg_controllen = (rand() % 2) ? rand() : 0;
        }
        iov.iov_len = (rand() % 2) ? evilint() : 1;
        iov.iov_base = ((rand() % 5) == 0) ? (void*)evilint() : &msg;
        msg.msg_iov = ((rand() % 5) == 0) ? (void*)evilint() : &iov;
        if (rand() % 10)
        {
            msg.msg_namelen = evilint() & 4096;
            b = malloc(msg.msg_namelen);
            if ( b != NULL && msg.msg_namelen < 0xFFFFF)
                fuzzer(b, msg.msg_namelen);
            msg.msg_name = b;
        }
        else
        {
            msg.msg_name = (caddr_t)evilint();
            msg.msg_namelen = evilint();
        }
        if ( rand() % 5 )
            flags = evilint() % MSG_CMSG_CLOEXEC;
        else
            flags = evilint();

        msg.msg_flags = evilint();
        sendmsg(fd, &msg, MSG_DONTWAIT);
        free(cmsg);
        cmsg = NULL;
        free(b);
        b = NULL;
    }
}

void splicusse(int fd)
{
    unsigned int    flags;
    off_t           offin, offout;
    size_t          len;
    int             pipes[2], fdout, fdin;

    fdin = fdout = fd;
    if (rand() % 3)
    {
        if (pipe(pipes) == 0)
            fdout = pipes[rand() % 2];
    }
    else
    {
        if (rand() % 2)
        {
            if (pipe(pipes) == 0)
                fdin = pipes[rand() % 2];
        }
    }

    switch (rand() % 5)
    {
        case 0:
            flags = evilint();
            break;
        default:
            flags = evilint() % 8;
            break;
    }

    switch (rand() % 5)
    {
        case 0:
            offin = evilint();
            offout = evilint();
            break;
        default:
            offin = offout = 0;
            break;
    }

    switch (rand() % 5)
    {
        case 0:
            len = evilint();
            break;
        default:
            len = evilint() % 128;
            break;
    }

    splice(fd, offin, fdout, offout, len, flags);
}
#endif

void mmapusse(int fd)
{
    void    *addr, *raddr;
    int     flags, prot;
    size_t  len, off;

    switch (rand() % 5)
    {
        case 0:
            flags = evilint();
            prot  = evilint();
            break;
        default:
            flags = evilint() % 0x20;
            prot = evilint() % 0x4;
            break;
    }

    switch (rand() % 5)
    {
        case 0:
            addr = (void *)evilint();
            flags |= MAP_FIXED;
            break;
        default:
            addr = NULL;
            flags &= ~MAP_FIXED;
            break;
    }

    raddr = mmap(addr, (len=evilint()), prot, flags, fd, (off=evilint()));

    if ( raddr != MAP_FAILED )
    {
        printf("\nmmap(%p, %u, %d, %d, %d (snum = %d), %d); -> %p\n", addr, len, prot, flags, fd, snum, off, raddr);
        munmap(addr, len);
    }
}

void usage(char *prog)
{
    printf("NETUSSE. KERNEL NET STACK DESTROYER.\n");
    printf("0xbadc0ded by Clement LECIGNE <root @ clem1.be>.\n\n");
	printf("\tusage: %s [-r seed][-n occ][-o occ][-f] && rm -fr /\n", prog);
    printf("RTFS. RTFS. RTFS.\n");
	exit(EXIT_FAILURE);
}

int main(int ac, char **av)
{
	char c;
	int s, i, opts, type, domain, proto, flags;
	unsigned int seed, occ;
    const int types[] = {
        SOCK_DGRAM,
        SOCK_STREAM,
#ifdef SOCK_PACKET
        SOCK_PACKET,
#endif
        SOCK_SEQPACKET,
        SOCK_RDM,
    };
    const int protos[] = {
        IPPROTO_IP,
        IPPROTO_ICMP,
        IPPROTO_IGMP,
        IPPROTO_IPIP,
        IPPROTO_TCP,
        IPPROTO_EGP,
        IPPROTO_PUP,
        IPPROTO_UDP,
        IPPROTO_IDP,
#ifdef IPPROTO_DCCP
        IPPROTO_DCCP,
#endif
        IPPROTO_RSVP,
        IPPROTO_GRE,
        IPPROTO_IPV6,
        IPPROTO_ESP,
        IPPROTO_AH,
#ifdef IPPROTO_BEETPH
        IPPROTO_BEETPH,
#endif
        IPPROTO_PIM,
#ifdef IPPROTO_COMP
        IPPROTO_COMP,
#endif
#ifdef IPPROTO_SCTP
        IPPROTO_SCTP,
#endif
#ifdef IPPROTO_UDPLITE
        IPPROTO_UDPLITE,
#endif
    };
    const int domains[] = {
#ifdef         AF_UNSPEC
        AF_UNSPEC,
#endif
#ifdef         AF_UNIX
        AF_UNIX,
#endif
#ifdef         AF_LOCAL
        AF_LOCAL,
#endif
#ifdef         AF_INET
        AF_INET,
#endif
#ifdef         AF_AX25
        AF_AX25,
#endif
#ifdef         AF_IPX
        AF_IPX,
#endif
#ifdef AF_APPLETALK
        AF_APPLETALK,
#endif
#ifdef         AF_NETROM
        AF_NETROM,
#endif
#ifdef         AF_BRIDGE
        AF_BRIDGE,
#endif
#ifdef         AF_ATMPVC
        AF_ATMPVC,
#endif
#ifdef        AF_X25
        AF_X25,
#endif
#ifdef        AF_INET6
        AF_INET6,
#endif
#ifdef        AF_ROSE
        AF_ROSE,
#endif
#ifdef        AF_DECnet
        AF_DECnet,
#endif
#ifdef        AF_NETBEUI
        AF_NETBEUI,
#endif
#ifdef        AF_SECURITY
        AF_SECURITY,
#endif
#ifdef        AF_KEY
        AF_KEY,
#endif
#ifdef        AF_NETLINK
        AF_NETLINK,
#endif
#ifdef        AF_ROUTE
        AF_ROUTE,
#endif
#ifdef        AF_PACKET
        AF_PACKET,
#endif
#ifdef        AF_ASH
        AF_ASH,
#endif
#ifdef        AF_ECONET
        AF_ECONET,
#endif
#ifdef        AF_ATMSVC
        AF_ATMSVC,
#endif
#ifdef        AF_RDS
        AF_RDS,
#endif
#ifdef        AF_SNA
        AF_SNA,
#endif
#ifdef        AF_IRDA
        AF_IRDA,
#endif
#ifdef        AF_PPPOX
        AF_PPPOX,
#endif
#ifdef        AF_WANPIPE
        AF_WANPIPE,
#endif
#ifdef        AF_LLC
        AF_LLC,
#endif
#ifdef        AF_CAN
        AF_CAN,
#endif
#ifdef        AF_TIPC
        AF_TIPC,
#endif
#ifdef        AF_BLUETOOTH
        AF_BLUETOOTH,
#endif
#ifdef        AF_IUCV
        AF_IUCV,
#endif
#ifdef        AF_RXRPC
        AF_RXRPC,
#endif
#ifdef        AF_ISDN
        AF_ISDN,
#endif
#ifdef        AF_PHONET
        AF_PHONET
#endif
    };

	seed = getpid() ^ time(NULL);
	occ = 5000000;
	opts = 10;

    signal(SIGPIPE, SIG_IGN);

	while ((c = getopt(ac, av, "r:o:n:f")) != EOF)
	{
		switch (c)
		{
		case 'r':
			seed = atoi(optarg);
			break;
		case 'o':
			opts = atoi(optarg);
			break;
		case 'n':
			occ = atoi(optarg);
			break;
        case 'f':
            s = open(SEED_FILE, O_RDONLY);
            if ( s > 0 )
            {
                read(s, &seed, sizeof(seed));
                close(s);
            }
            break;
		case 'h':
			usage(av[0]);
			break;
		default:
			usage(av[0]);
			break;
		}
	}

	printf(" + using seed: %u\n", seed);
    s = open(SEED_FILE, O_WRONLY|O_CREAT, 0666);
    if ( s > 0 )
    {
        /* l33t */
        write(s, &seed, sizeof(seed));
        close(s);
    }
	srand(seed);

	while (occ)
	{
        switch ((snum = rand() % 30))
        {
            case 0:
                s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 1:
                s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case 2:
                s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 3:
                s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
#ifdef __linux__
            case 4:
            {
                struct sockaddr_at sat;
                s = socket(AF_APPLETALK, SOCK_DGRAM, 0);
                memset(&sat, 0, sizeof(sat));
                sat.sat_family = AF_APPLETALK;
                sat.sat_addr.s_net = htons(ATADDR_ANYNET);
                sat.sat_addr.s_node = ATADDR_ANYNODE;
                sat.sat_port = ATADDR_ANYPORT;
                bind(s, (struct sockaddr *) &sat, sizeof(sat));
                break;
            }
            case 5:
                s = socket(AF_X25, SOCK_SEQPACKET, 0);
                break;
            case 6:
                s = socket(AF_IRDA, SOCK_DGRAM, 0);
                break;
            case 7:
            {
                struct sockaddr_llc sllc;
                s = socket(AF_LLC, SOCK_DGRAM, 0);
                memset(&sllc, 0, sizeof(sllc));
                sllc.sllc_family = AF_LLC;
                sllc.sllc_arphrd = ARPHRD_ETHER;
                sllc.sllc_sap = LLC_SAP_NULL;
                sendto(s, "COIN", 4, 0, (struct sockaddr *) &sllc, sizeof(sllc));
                break;
            }
#endif
#ifdef IPPROTO_SCTP
            case 8:
                s = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
                break;
#endif
            case 9:
                s = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 10:
                s = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case 11:
                s = socket(AF_IPX, SOCK_DGRAM, IPPROTO_UDP);
                break;
#ifdef __linux__
            case 12:
                s = socket(AF_ATMPVC, SOCK_DGRAM, 0);
                break;
            case 13:
                s = socket(AF_X25, SOCK_SEQPACKET, 0);
                break;
#endif
#ifdef AF_PACKET
            case 14:
                s = socket(AF_PACKET, SOCK_DGRAM, 0);
                break;
            case 15:
                s = socket(AF_PACKET, SOCK_PACKET, 0);
                break;
            case 16:
                s = socket(AF_PACKET, SOCK_SEQPACKET, 0);
                break;
#endif
#ifdef PF_CAN
            case 17:
            case 18:
            case 19:
                s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);
                break;
#endif
            default:
                if (rand() % 10 == 0)
                {
                    domain = rand() % 255;
                    proto  = rand() % 255;
                    type   = rand() % 255;
                }
                else
                {
                    domain = domains[rand() % (sizeof(domains)/sizeof(domains[0]))];
                    proto = protos[rand() % (sizeof(protos)/sizeof(protos[0]))];
                    type = types[rand() % (sizeof(types)/sizeof(types[0]))];
                }
                s = socket(domain, type, proto);
                break;
        }
		if (s == -1) continue;

        if (snum > 16)
            printf("\nsocket(%d, %d, %d);\n", domain, type, proto);

        flags = fcntl(s, F_GETFL, 0);
        fcntl(s, F_SETFL, flags | O_NONBLOCK);

        if (rand() % 4)
            connectusse(s);
        else
            bindusse(s);

		for (i = 0; i < opts; i++)
		{
			ssoptusse(s);
			gsoptusse(s);
            ioctlusse(s);
            sendmsgusse(s);
            sendtousse(s);
#if defined(__linux__)
            sendfilusse(s);
#endif
            //usleep(500);
		}
#ifdef __linux__
        splicusse(s);
#endif
        sendtousse(s);
        getsocknamusse(s);
        getpeernamusse(s);
        mmapusse(s);
        printf(".");fflush(stdout);
		close(s);

#ifdef __LINUX__
        if ( linux_hasoopsed() )
        {
            printf("\ndude, check your kernel!! It's fucked\n");
            getchar();
        }
#endif

		occ--;
	}

	exit(EXIT_SUCCESS);
}
