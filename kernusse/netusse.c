/*
 * netusse.c - fucking kernel net stacks fuzzer
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
#include <sys/klog.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <net/if_arp.h>
#include <linux/atalk.h>
#include <linux/in.h>

#define SEED_FILE "/tmp/netusse.seed"

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

		ret = setsockopt(s, level, optname, (void *)optval, optlen);
	}
    while(ret == -1 && tout--);
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
            if ( memcmp(&buf, &pbuf, (len > 2048) ? 2048 : len) != 0 )
            {
                printf("\nPOSSIBLE LEAK WITH :\n");
                printf("\tgetsockname(sock (%d), buf, &%d)\n", snum, len);
                len = (len > 2048) ? 2048 : len;
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
        if ( ret >= 0 && memcmp(&buf, &pbuf, (len > 2048) ? 2048 : len) != 0 )
        {
            kernop(s);
            getpeername(s, (struct sockaddr *)&pbuf, &len);
            if ( memcmp(&buf, &pbuf, (len > 2048) ? 2048 : len) != 0 )
            {
                printf("\nPOSSIBLE LEAK WITH :\n");
                printf("\tgetpeername(sock (%d), buf, &%d)\n", snum, len);
                len = (len > 2048) ? 2048 : len;
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

    if ( memcmp(&buf, &pbuf, (len > 2048) ? 2048 : len) != 0 && memcmp(&pbuf, &rbuf, (len > 2048) ? 2048 : len) != 0 )
    {
        printf("\nPOSSIBLE LEAK WITH :\n");
		printf("\tgetsockopt(sock (%d), %d, %u, buf, &%d)\n", snum, level, optname, len);
        len = (len > 2048) ? 2048 : len;
        printf("FIRST CALL:\n");
        dump(buf, len);
        printf("SECOND CALL:\n");
        dump(pbuf, len);
        getchar();
    }

	return;
}

static void fuzzer(char *mm, size_t mm_size)
{
    size_t i;

    for ( i = 0 ; i < mm_size ; i++ )
        mm[i] = rand() & 255;

    return;
}

/**
 * fucking bindusse #@!
 */
void bindusse(int fd)
{
    int     ret, tout = 5, len;
    char    *b;

    do
    {
        len = evilint();
        b = malloc(len);
        if ( b != NULL && len < 0xFFFFF)
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
    int     ret, tout = 5, len;
    char    *b;

    do
    {
        len = evilint();
        b = malloc(len);
        if ( b != NULL && len < 0xFFFFF)
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
    if ( addr != NULL && alen < 0xFFFFF)
        fuzzer(addr, alen);

    mlen = evilint();
    msg = malloc(mlen);
    if ( msg != NULL && mlen < 0xFFFFF)
        fuzzer(msg, mlen);

    sendto(fd, msg, mlen, flags, (struct sockaddr *)addr, (socklen_t)alen);

    if ( addr ) free(addr);
    if ( msg ) free(msg);
}

void sendmsgusse(int fd)
{
	struct msghdr   msg;
	struct cmsghdr  *cmsg = NULL;
	struct iovec    iov;
    char            *b;
    int             i, flags;

    for ( i = 0 ; i < 50 ; i++ )
    {
        msg.msg_controllen = (rand() % 2) ? evilint() : 0;
        if (msg.msg_controllen)
        {
            if (msg.msg_controllen < sizeof (struct cmsghdr))
                cmsg = (struct cmsghdr *)malloc(sizeof (struct cmsghdr));
            else
                cmsg = (struct cmsghdr *)malloc(msg.msg_controllen);
            if (cmsg == NULL) goto nocmsghdr;
            msg.msg_control = cmsg;
            cmsg->cmsg_level = (rand() % 2) ? IPPROTO_IP : evilint();
            cmsg->cmsg_type = (rand() % 2) ? rand() % 255 : evilint();
            cmsg->cmsg_len = (rand() % 2) ? msg.msg_controllen : evilint();
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
            msg.msg_namelen = evilint() & 50;
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
        SOCK_PACKET,
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
        IPPROTO_DCCP,
        IPPROTO_RSVP,
        IPPROTO_GRE,
        IPPROTO_IPV6,
        IPPROTO_ESP,
        IPPROTO_AH,
        IPPROTO_BEETPH,
        IPPROTO_PIM,
        IPPROTO_COMP,
        IPPROTO_SCTP,
        IPPROTO_UDPLITE,
    };
    const int domains[] = {
        AF_UNSPEC,
        AF_UNIX,
        AF_LOCAL,
        AF_INET,
        AF_AX25,
        AF_IPX,
        AF_APPLETALK,
        AF_NETROM,
        AF_BRIDGE,
        AF_ATMPVC,
        AF_X25,
        AF_INET6,
        AF_ROSE,
        AF_DECnet,
        AF_NETBEUI,
        AF_SECURITY,
        AF_KEY,
        AF_NETLINK,
        AF_ROUTE,
        AF_PACKET,
        AF_ASH,
        AF_ECONET,
        AF_ATMSVC,
        AF_RDS,
        AF_SNA,
        AF_IRDA,
        AF_PPPOX,
        AF_WANPIPE,
        AF_LLC,
        AF_CAN,
        AF_TIPC,
        AF_BLUETOOTH,
        AF_IUCV,
        AF_RXRPC,
        AF_ISDN,
        AF_PHONET
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
        switch ((snum = rand() % 20))
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
            case 8:
                s = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
                break;
            case 9:
                s = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
                break;
            case 10:
                s = socket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case 11:
                s = socket(AF_IPX, SOCK_DGRAM, IPPROTO_UDP);
                break;
            case 12:
                s = socket(AF_ATMPVC, SOCK_DGRAM, 0);
                break;
            case 13:
                s = socket(AF_X25, SOCK_SEQPACKET, 0);
                break;
            case 14:
                s = socket(AF_PACKET, SOCK_DGRAM, 0);
                break;
            case 15:
                s = socket(AF_PACKET, SOCK_PACKET, 0);
                break;
            case 16:
                s = socket(AF_PACKET, SOCK_SEQPACKET, 0);
                break;
            default:
                domain = domains[rand() % (sizeof(domains)/sizeof(domains[0]))];
                proto = protos[rand() % (sizeof(protos)/sizeof(protos[0]))];
                type = types[rand() % (sizeof(types)/sizeof(types[0]))];
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
            sendmsgusse(s);
            sendtousse(s);
            usleep(500);
		}
        splicusse(s);
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
