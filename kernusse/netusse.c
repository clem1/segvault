/*
 * netusse.c - fucking kernel net stacks fuzzer.
 *
 * At least it successfully broke:
 *  FreeBSD
 *  NetBSD
 *  OpenBSD
 *  Solaris
 *
 * Copyright (c) Clément Lecigne, 2006-2012
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
#include <dirent.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <net/if_arp.h>
#ifdef __linux__
#include <sys/klog.h>
#include <linux/atalk.h>
#include <linux/can.h>
#include <netinet/in.h>
#else
#include <netinet/in.h>
#include <sys/uio.h>
#endif

#include <sys/un.h>

#if defined(__OpenBSD__)
#include <util.h>
#endif

// printf special debug shits!
//#define DEBUG 1
//#define DEBUG_SOCKET 1
//#define DEBUG_BIND 1

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

/* socket choosen
 */
int snum = -1;

/* handler for debug
 */
FILE *dbg;

/* current socket family
 */
int current_family;

/* current socket protocol
 */
int current_proto;

static int debug_socket(int domain, int type, int protocol)
{
#if defined(DEBUG) || defined(DEBUG_SOCKET)
    fprintf(dbg, "socket(%d, %d, %d)\n", domain, type, protocol);
#endif

    /* add exceptions here */

    current_proto = protocol;
    current_family = domain;
    return socket(domain, type, protocol);
}

/* create a random stream of mm_size bytes inside mm.
 */
static void fuzzer(char *mm, size_t mm_size)
{
    size_t i;

    for ( i = 0 ; i < mm_size ; i++ )
    {
        /* lame format string checker, evil values or random.
         */
        if ( rand() % 40 == 0 && i < mm_size - 2 )
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
        else if ( rand() % 40 == 0 )
            mm[i] = 255;
        else if ( rand() % 40 == 0 )
            mm[i] = 0;
        else
        {
            mm[i] = rand() & 255;
            if ( rand() % 10 == 0 )
                mm[i] |= 0x80;
        }
    }

    return;
}

#ifdef __FreeBSD__
static void fuzziovec(struct iovec *io, size_t len)
{
    size_t i;

    for (i = 0; i < len; io++, i++)
    {
        io->iov_len = rand() & 255;
        io->iov_base = malloc(io->iov_len);
        if (io->iov_base != NULL)
            fuzzer(io->iov_base, io->iov_len);
    }
}
#endif

#ifdef __linux__
/* lame piece of code to see if linux kernel has oopsed :)
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

/* do a random kernel operation, used to detect memory leak.
 */
static void kernop(int fd)
{
    /* from Jon Oberheide sploits
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
#ifdef __LINUX__
            case 0:
                o = randcalls[rand() % sizeof(randcalls)/sizeof(randcalls[0])];
                ret = syscall(o);
                break;
#else
            case 0: /* TODO: to bored to enumerate unevil syscall on other sys. */
#endif
            case 1:
                len = (rand() % 2) ? sizeof(int) : sizeof(buf);
                ret = getsockopt(fd, randsopts[rand() % sizeof(randsopts)/sizeof(randsopts[0])], rand() % 130, &buf, &len);
                break;
        }
    }
    while ( ret < 0 );
}

/* return random file on the FS or not.
 */
static char *getfile(void)
{
    switch (rand() % 5)
    {
        case 0:
            return "/etc/passwd";
        case 1:
            return "/dev/random";
        case 2:
            return "/tmp/fusse";
        case 3:
            return "/tmp/";
#ifdef __LINUX__
        case 4:
            return "/proc/self/maps";
#endif
        default:
            return "/";
    }
    return "foo";
}


/* return a random file descriptor
 */
static int getfd(void)
{
    int fd, flags;

    do {
        switch (rand() % 7)
        {
            case 0:
                fd = open("/etc/passwd", O_RDONLY);
                break;
            case 1:
                fd = debug_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 2:
                fd = open("/dev/random", O_RDONLY);
                break;
            case 3:
                fd = open("/tmp/fusse", O_CREAT|O_RDWR, 0666);
                break;
            case 4:
                fd = debug_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
            default:
                fd = open(getfile(), rand());
                break;
        }
    }
    while (fd < 0);
    flags = fcntl(fd, F_GETFL, 0);
    /* force non blocking more on fd
     */
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return fd;
}

/* ilja's stuff
 */
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
                 return rand() & 256;
        default:
                return rand();

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
		switch (rand() % 25)
		{
		case 0:
			level = IPPROTO_IPV6;
			break;
		case 1:
			level = SOL_SOCKET;
			break;
		case 2:
		case 3:
		case 4:
            level = current_proto;
			break;
        case 5:
        case 6:
            level = current_family;
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

#if 0
        /*
		 * anti well know FreeBSD mbufs exhaustion.
		 */
		if (optname == 182 || optname == 202 || optname == 254 || optname == 91 || optname == 25 || optname == IPV6_IPSEC_POLICY || 
				optname == IPV6_FW_ADD || optname == IPV6_FW_FLUSH
				|| optname == IPV6_FW_DEL || optname == IPV6_FW_ZERO || (current_family == AF_INET && optname == 21) || (current_family == AF_INET6 && optname == 21) )
			continue;
#endif
#ifdef DEBUG
        fprintf(dbg, "setsockopt(%u, %u, %u, %p, %u) family = %d proto = %d\n", s, level, optname, optval, optlen, current_family, current_proto);
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
        switch (rand() % 8)
        {
            case 0:
            case 1:
                req = rand() & 255;
                break;
            default:
                req = rand();
                break;
        }

        n = rand() % 7;
        for (i = 0; i < n; i++)
        {
            int len = rand() % 1024;
            iav[i] = malloc(len);
            fuzzer(iav[i], len);
        }

#define GETIAV(iii) (rand() % 5 == 0) ? iav[iii] : (void *)evilint()

        switch (n)
        {
            case 0:
                ret = ioctl(s, req);
                break;
            case 1:
                ret = ioctl(s, req, GETIAV(0));
                break;
            case 2:
                ret = ioctl(s, req, GETIAV(0), GETIAV(1));
                break;
            case 3:
                ret = ioctl(s, req, GETIAV(0), GETIAV(1), GETIAV(2));
                break;
            case 4:
                ret = ioctl(s, req, GETIAV(0), GETIAV(1), GETIAV(2), GETIAV(3));
                break;
            case 5:
                ret = ioctl(s, req, GETIAV(0), GETIAV(1), GETIAV(2), GETIAV(3), GETIAV(4));
                break;
            case 6:
                ret = ioctl(s, req, GETIAV(0), GETIAV(1), GETIAV(2), GETIAV(3), GETIAV(4), GETIAV(5));
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
    int     len = 0, ret, i;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);

    for ( i = 0 ; i < 20 ; i++ )
    {
        int coin;
        len = rand() % 2048;
        coin = len;
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
    int     len = 0, ret, i;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);

    for ( i = 0 ; i < 20 ; i++ )
    {
        len = rand() % 2048;
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
		case 3:
		case 4:
            level = current_proto;
			break;
        case 5:
        case 6:
            level = current_family;
            break;
        default:
            level = evilint();
            break;
		}
#if 0
        /*
		 * anti well know FreeBSD mbufs exhaustion.
		 */
		if (optname == 182 || optname == 202 || optname == 254 || optname == 91 || optname == 25 || optname == IPV6_IPSEC_POLICY || 
				optname == IPV6_FW_ADD || optname == IPV6_FW_FLUSH
				|| optname == IPV6_FW_DEL || optname == IPV6_FW_ZERO || (current_family == AF_INET && optname == 21) || (current_family == AF_INET6 && optname == 21) )
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
#if 0
#if defined(__linux__)
    if ( len == 104 && optname == 11 )
        return;
#endif
#endif

    kernop(s);

    getsockopt(s, level, optname, &pbuf, &len);

    if ( buf[0] == 0xc7 || buf[0] == 0xc8 || ( memcmp(&buf, &pbuf, (len < 0 || len > 2048) ? 2048 : len) != 0 && memcmp(&pbuf, &rbuf, (len > 2048) ? 2048 : len) != 0 ) )
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

/* sockaddr fuzzer
 */
static void sockaddrfuzz(char *buf, size_t len)
{
    struct sockaddr  *sa     = (struct sockaddr *)buf;
    struct sockaddr_un  *sun    = (struct sockaddr_un *)buf;

    /* mangling
     */
    fuzzer(buf, len);

    if (len < sizeof(struct sockaddr))
        return;

    sa->sa_family = current_family;

    /* patching
     */
    switch (rand() % 5)
    {
        case 0:
        case 1:
        case 3:
        case 2:
        /* path */
        if (len > 16)
        {
            char *f = getfile();
#define min(a, b) (a < b) ? a : b
            strncpy(sun->sun_path, f, min(strlen(f), len-12));
            break;
        }
        default:
        /* TODO */
        break;
    }
}

/**
 * fucking bindusse #@!
 */
void bindusse(int fd)
{
    size_t              len;
    int                 ret, tout = 5;
    char                *b;
    struct sockaddr     *sa;

    do
    {
        len = evilint() % 4096;
        b = malloc(len);
        if (!b)
            continue;
        sockaddrfuzz(b, len);
#if defined(DEBUG) || defined(DEBUG_BIND)
        fprintf(dbg, "bind(%d, x, %u)\n", fd, len);
#endif
        ret = bind(fd, (struct sockaddr *)&b, len);
        if (ret && (rand() % 2))
            listen(fd, rand());
        free(b); b = NULL;
    }
    while (ret < 0 && tout--);
}

void connectusse(int fd)
{
    int                 ret, tout = 5;
    size_t              len;
    char                *b;
    struct sockaddr     *sa;

    do
    {
        len = evilint() % 4096;
        b = malloc(len);
        if (!b)
            continue;
        sockaddrfuzz(b, len);
#if defined(DEBUG) || defined(DEBUG_CONNECT)
        fprintf(dbg, "connect(%d, x, %u)\n", fd, len);
#endif
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
    struct sockaddr *sa;

    alen = evilint() % 4096;
    addr = malloc(alen);
    if (addr)
        sockaddrfuzz(addr, alen);

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
#elif defined(__FreeBSD__)
void sendfilusse(int fd)
{
    int i;

    for ( i = 0 ; i < 50 ; i++ )
    {
        off_t           offset;
        size_t          size;
        int             ifd, flags;
        struct sf_hdtr  hdtr, *hdtrp;

        offset = evilint();
        size = evilint();
        ifd = evilint();
        flags = rand() % 5;
        hdtrp = NULL;

        /* ifd case
         */
        switch (rand() % 5)
        {
            case 0:
                ifd = fd;
                break;
            case 1:
                ifd = evilint();
                break;
            case 3:
                ifd = fd;
                break;
            case 2:
            case 4:
                ifd = getfd();
                break;
        }

        /* off
         */
        if ( rand() % 5 == 2 )
            offset = 0;

        /* size
         */
        if ( rand() % 5 == 2 )
            size = rand() & 0xfff;

        /* flags
         */
        if ( rand() % 5 == 2 )
            flags = rand();

        /* hdtr
         */
        if ( rand() % 5 == 2 )
        {
            hdtrp = &hdtr;
            hdtr.hdr_cnt = rand() % 10;
            hdtr.headers = malloc(hdtr.hdr_cnt * sizeof(struct iovec));
            fuzziovec(hdtr.headers, hdtr.hdr_cnt);
            hdtr.trl_cnt = rand() % 10;
            hdtr.trailers = malloc(hdtr.trl_cnt * sizeof(struct iovec));
            fuzziovec(hdtr.trailers, hdtr.trl_cnt);
        }

        sendfile(ifd, fd, offset, size, hdtrp, NULL, flags);
        if ( ifd != fd )
            close(ifd);
    }
}
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
void recvmsgusse(int fd)
{
    char name[1024], ctrl[1024], base[1024], iovb[sizeof(struct iovec)];
    struct iovec iov;
    struct msghdr msg;
    int i;

    for ( i = 0 ; i < 50 ; i++ )
    {
        fuzzer(name, 1024);
        fuzzer(base, 1024);
        fuzzer(ctrl, 1024);
        fuzzer(iovb, sizeof(struct iovec));
        msg.msg_name    = name;
        msg.msg_namelen = evilint();
        msg.msg_iovlen  = evilint();
        msg.msg_flags   = rand() & 255;
        msg.msg_iov     = iovb;
        if ( rand() % 3 )
        {
            msg.msg_iov     = &iov;
            msg.msg_iovlen  = 1;
            iov.iov_base    = base;
            iov.iov_len     = evilint();
        }
        else if ( rand() % 5 == 0 )
            msg.msg_iov = NULL;
        msg.msg_control = ctrl;
        msg.msg_controllen = evilint();
#ifdef DEBUG
        fprintf(dbg, "recvmsg(%d, {nl = %x, iol = %x, ctl = %x}, 0);\n", fd, msg.msg_namelen, msg.msg_iovlen, msg.msg_controllen);
#endif
        recvmsg(fd, &msg, MSG_DONTWAIT);
    }
    return;
}
#elif defined(__LINUX__) || defined(__FreeBSD__)
void recvmsgusse(int fd)
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
            fuzzer((char *)cmsg, msg.msg_controllen);
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
            flags = evilint() % 255;
        else
            flags = evilint();

        msg.msg_flags = evilint();
#ifdef DEBUG
        fprintf(dbg, "recvmsg(%d, {nl = %x, iol = %x, ctl = %x}, 0);\n", fd, msg.msg_namelen, msg.msg_iovlen, msg.msg_controllen);
#endif
        recvmsg(fd, &msg, MSG_DONTWAIT);
        free(cmsg);
        cmsg = NULL;
        free(b);
        b = NULL;
    }
}
#endif


#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__)
void sendmsgusse(int fd)
{
    char name[1024], ctrl[1024], base[1024], iovb[sizeof(struct iovec)], *b = NULL, *bb = NULL;
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    int i;

    for ( i = 0 ; i < 50 ; i++ )
    {
        msg.msg_controllen = (rand() % 50) ? rand() : 0;
        if (msg.msg_controllen)
        {
            b = malloc(CMSG_SPACE(msg.msg_controllen % 5000));
            if ( b == NULL )
                continue;
            fuzzer(b, CMSG_SPACE(msg.msg_controllen % 5000));
            msg.msg_control = b;
            msg.msg_controllen = CMSG_SPACE(msg.msg_controllen % 5000);
            cmsg = CMSG_FIRSTHDR(&msg);
            cmsg->cmsg_len = CMSG_LEN(msg.msg_controllen);
            cmsg->cmsg_type = (rand() % 2) ? rand() % 255 : evilint();
            cmsg->cmsg_len = (rand() % 2) ? msg.msg_controllen : evilint();
        }
        else
        {
nocmsghdr:
            msg.msg_control = (rand() % 5) ? NULL : (void*)evilint();
            msg.msg_controllen = (rand() % 2) ? rand() : 0;
        }

        if ((rand() % 5) == 0)
        {
            iov.iov_len = (rand() % 2) ? evilint() : 1;
            iov.iov_base = ((rand() % 5) == 0) ? (void*)evilint() : &msg;
            msg.msg_iov = ((rand() % 5) == 0) ? (void*)evilint() : &iov;
            if (rand() % 10)
            {
                msg.msg_namelen = evilint() & 4096;
                bb = malloc(msg.msg_namelen);
                if ( bb != NULL && msg.msg_namelen < 0xFFFFF)
                    fuzzer(bb, msg.msg_namelen);
                msg.msg_name = bb;
            }
            else
            {
                msg.msg_name = (caddr_t)evilint();
                msg.msg_namelen = evilint();
            }
            msg.msg_flags = evilint();
        }
#ifdef DEBUG
        fprintf(dbg, "sendmsg(%d, {nl = %x, iol = %x, ctl = %x}, 0);\n", fd, msg.msg_namelen, msg.msg_iovlen, msg.msg_controllen);
#endif
        sendmsg(fd, &msg, MSG_DONTWAIT);
        if (b) free(b);
        if (bb) free(bb);
        b = NULL;
        bb = NULL;
    }

    return;
}
#elif defined(__linux__)
void sendmsgusse(int fd)
{
	struct msghdr   msg;
	struct cmsghdr  *cmsg = NULL;
	struct iovec    iov;
    char            *b = NULL, *bb = NULL;
    int             i, flags;

    for ( i = 0 ; i < 50 ; i++ )
    {
        msg.msg_controllen = (rand() % 50) ? rand() : 0;
        if (msg.msg_controllen)
        {
            b = malloc(CMSG_SPACE(msg.msg_controllen % 5000));
            if ( b == NULL )
                continue;
            fuzzer(b, CMSG_SPACE(msg.msg_controllen % 5000));
            msg.msg_control = b;
            msg.msg_controllen = CMSG_SPACE(msg.msg_controllen % 5000);
            cmsg = CMSG_FIRSTHDR(&msg);
            cmsg->cmsg_len = CMSG_LEN(msg.msg_controllen);
            cmsg->cmsg_type = (rand() % 2) ? rand() % 255 : evilint();
            cmsg->cmsg_len = (rand() % 2) ? msg.msg_controllen : evilint();
        }
        else
        {
nocmsghdr:
            msg.msg_control = (rand() % 5) ? NULL : (void*)evilint();
            msg.msg_controllen = (rand() % 2) ? rand() : 0;
        }

        if ((rand() % 5) == 0)
        {
            iov.iov_len = (rand() % 2) ? evilint() : 1;
            iov.iov_base = ((rand() % 5) == 0) ? (void*)evilint() : &msg;
            msg.msg_iov = ((rand() % 5) == 0) ? (void*)evilint() : &iov;
            if (rand() % 10)
            {
                msg.msg_namelen = evilint() & 4096;
                bb = malloc(msg.msg_namelen);
                if ( bb != NULL && msg.msg_namelen < 0xFFFFF)
                    fuzzer(bb, msg.msg_namelen);
                msg.msg_name = bb;
            }
            else
            {
                msg.msg_name = (caddr_t)evilint();
                msg.msg_namelen = evilint();
            }
            msg.msg_flags = evilint();
        }

        sendmsg(fd, &msg, MSG_DONTWAIT);
        if (b) free(b);
        if (bb) free(bb);
        b = NULL;
        bb = NULL;
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

int randfd(void)
{
    DIR             *dip;
    struct dirent   *dit;
    static int      nbf = 1500;
    unsigned int    n = rand() % nbf, i = 0;
    int             fd;

    chdir("/dev");
    dip = opendir("/dev");
    if ( dip == NULL )
        return -1;

    while ( (dit = readdir(dip)) != NULL )
    {
        if ( i == n )
        {
            //printf("open(%s)...", dit->d_name);
            switch (rand() % 3)
            {
#if defined(__OpenBSD__)
                case 1:
                fd = opendev(dit->d_name, O_RDONLY, (rand() % 2) ? OPENDEV_BLCK : OPENDEV_PART, NULL);
                break;
                case 2:
                fd = opendev(dit->d_name, O_RDWR, (rand() % 2) ? OPENDEV_BLCK : OPENDEV_PART, NULL);
                break;
#else
                case 1:
                case 2:
#endif
                case 0:
                fd = open(dit->d_name, O_RDONLY);
                break;
            }
            //printf("%s\n", (fd > 0) ? "done" : "failed");
            closedir(dip);
            return fd;
        }
        i++;
    }
    nbf = i;

    closedir(dip);
    return -1;
}

struct foo_ip_msfilter
{
   __u32 imsf_multiaddr;
   __u32 imsf_interface;
   __u32 imsf_fmode;
   __u32 imsf_numsrc;
   __u32 imsf_slist[1];
};
 
struct foo_ip_mreq_source
{
   __u32 imr_multiaddr;
   __u32 imr_interface;
   __u32 imr_sourceaddr;
};

/* do some interesting wellknown interesting operation on sock.
 */
void valid_op(int s)
{
    struct ip_mreqn mr;
    struct foo_ip_msfilter msf;
    struct foo_ip_mreq_source ms;
    int    val=1;
    socklen_t l;
#ifdef __LINUX__
    char   *dev = "eth0";
#else
    char   *dev = "lo0";
#endif

    /* multicast stuffs from Paul Starzetz sploits.
     */
    memset (&mr, 0, sizeof (mr));
    mr.imr_multiaddr.s_addr = inet_addr ("224.0.0.199");
    setsockopt (s, SOL_IP, IP_ADD_MEMBERSHIP, &mr, sizeof (mr));
    memset (&ms, 0, sizeof (ms));
    ms.imr_multiaddr = inet_addr ("224.0.0.199");
    ms.imr_sourceaddr = inet_addr ("4.5.6.7");
    setsockopt (s, SOL_IP, IP_BLOCK_SOURCE, &ms, sizeof (ms));
    memset (&ms, 0, sizeof (ms));
    ms.imr_multiaddr = inet_addr ("224.0.0.199");
    ms.imr_sourceaddr = inet_addr ("4.5.6.7");
    setsockopt (s, SOL_IP, IP_UNBLOCK_SOURCE, &ms, sizeof (ms));
    memset (&ms, 0, sizeof (ms));
    ms.imr_multiaddr = inet_addr ("224.0.0.199");
    ms.imr_sourceaddr = inet_addr ("4.5.6.7");
    setsockopt (s, SOL_IP, IP_UNBLOCK_SOURCE, &ms, sizeof (ms));
    memset (&ms, 0, sizeof (ms));
    ms.imr_multiaddr = inet_addr ("224.0.0.199");
    ms.imr_sourceaddr = inet_addr ("4.5.6.7");
    setsockopt (s, SOL_IP, IP_UNBLOCK_SOURCE, &ms, sizeof (ms));

#if 0
    /* sctp shits.
     */
    l = sizeof(sctp_initmsg);
    getsockopt(s, SOL_SCTP, SCTP_INITMSG, &msg, &l);
    msg.sinit_num_ostreams = evilint();
    msg.sinit_max_instreams = evilint();
    setsockopt(s, SOL_SCTP, SCTP_INITMSG, &msg, sizeof(struct sctp_initmsg));
    setsockopt(s, SOL_SCTP, SCTP_NODELAY, (char*)&val, sizeof(val));
#endif

    /* f00 stuffs.
     */
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val));
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&val, sizeof(val));
    setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, dev, 4);

    return;
}

int main(int ac, char **av)
{
	char c;
	int s, i, opts, type, domain, proto, flags, isfd = 0;
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

    dbg = stdout;
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
        isfd = 0;
        switch ((snum = rand() % 50))
        {
            case 0:
                s = debug_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 1:
                s = debug_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case 2:
                s = debug_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 3:
                s = debug_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                break;
#ifdef __linux__
            case 4:
            {
                struct sockaddr_at sat;
                s = debug_socket(AF_APPLETALK, SOCK_DGRAM, 0);
                memset(&sat, 0, sizeof(sat));
                sat.sat_family = AF_APPLETALK;
                sat.sat_addr.s_net = htons(ATADDR_ANYNET);
                sat.sat_addr.s_node = ATADDR_ANYNODE;
                sat.sat_port = ATADDR_ANYPORT;
                bind(s, (struct sockaddr *) &sat, sizeof(sat));
                break;
            }
            case 5:
                s = debug_socket(AF_X25, SOCK_SEQPACKET, 0);
                break;
            case 6:
                s = debug_socket(AF_IRDA, SOCK_DGRAM, 0);
                break;
            case 7:
                s = debug_socket(AF_LLC, SOCK_DGRAM, 0);
                break;
#endif
#ifdef IPPROTO_SCTP
            case 8:
                s = debug_socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
                break;
#endif
            case 9:
                s = debug_socket(AF_UNIX, SOCK_STREAM, 0);
                break;
            case 10:
                s = debug_socket(AF_UNIX, SOCK_DGRAM, 0);
                break;
            case 11:
                s = debug_socket(AF_IPX, SOCK_DGRAM, IPPROTO_UDP);
                break;
#ifdef __linux__
            case 12:
                s = debug_socket(AF_ATMPVC, SOCK_DGRAM, 0);
                break;
            case 13:
                s = debug_socket(AF_X25, SOCK_SEQPACKET, 0);
                break;
#endif
#ifdef AF_PACKET
            case 14:
                s = debug_socket(AF_PACKET, SOCK_DGRAM, 0);
                break;
            case 15:
                s = debug_socket(AF_PACKET, SOCK_PACKET, 0);
                break;
            case 16:
                s = debug_socket(AF_PACKET, SOCK_SEQPACKET, 0);
                break;
#endif
#ifdef PF_CAN
            case 17:
            case 18:
            case 19:
                s = debug_socket(PF_CAN, SOCK_DGRAM, CAN_BCM);
                break;
#endif
            default:
                if (rand() % 20 == 0)
                {
                    s = randfd();
                    isfd = 1;
                }
                else
                {
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
                    s = debug_socket(domain, type, proto);
                }
                break;
        }
		if (s == -1) continue;

        if (rand() % 2)
            valid_op(s);

        flags = fcntl(s, F_GETFL, 0);
        fcntl(s, F_SETFL, flags | O_NONBLOCK);

        if (!isfd)
        {
            if (rand() % 4)
                connectusse(s);
            else
                bindusse(s);
            if (rand() % 4)
                connectusse(s);
            else
                bindusse(s);

            if (rand() % 5)
                listen(s, evilint());
        }
		for (i = 0; i < opts; i++)
		{
            if (isfd)
            {
                ioctlusse(s);
                continue;
            }
			ssoptusse(s);
            ioctlusse(s);
			gsoptusse(s);
            sendmsgusse(s);
            recvmsgusse(s);
            sendtousse(s);
#if defined(__linux__) || defined(__FreeBSD__)
            sendfilusse(s);
#endif
            //usleep(500);
		}
#ifdef __linux__
        splicusse(s);
#endif
        if (!isfd)
        {
            sendtousse(s);
            getsocknamusse(s);
            getpeernamusse(s);
            mmapusse(s);
        }
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
