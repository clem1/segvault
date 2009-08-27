/*
 * netusse.c - fucking kernel net stacks fuzzer
 *
 * Copyright (c) Clément Lecigne, 2006
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <sys/klog.h>
#include <net/if_arp.h>
#include <linux/atalk.h>

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
        return strstr(buf, "ecx: ") != NULL;
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
            case 0:
#ifdef __LINUX__
                o = randcalls[rand() % sizeof(randcalls)/sizeof(randcalls[0])];
#else
                o = rand() % 64;
#endif
                ret = syscall(o);
                break;
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

void soopt(int s)
{
	unsigned int optval;
	int optlen, optname, level, ret, on = rand() % 2;
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
    while(ret == -1);
}

void gsn(int s)
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

void gpn(int s)
{
    char    buf[2048], pbuf[2048];
    int     len = 0, ret;

    memset(&buf, 'A', 2048);
    memset(&pbuf, 'A', 2048);

    for ( len = 0 ; len < 50 ; len++ )
    {
        ret = getpeername(s, (struct sockaddr *)&buf, &len);
        if ( ret >= 0 )
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

void goof(int s)
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

void usage(char *prog)
{
	printf(" - usage: %s [-r seed][-n occ][-o occ]\n", prog);
	exit(EXIT_FAILURE);
}

int main(int ac, char **av)
{
	char c;
	int s, i, opts;
	unsigned int seed, occ;

	seed = getpid();
	occ = 5000000;
	opts = 10;
	
    signal(SIGPIPE, SIG_IGN);

	while ((c = getopt(ac, av, "r:o:n:")) != EOF)
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
		case 'h':
			usage(av[0]);
			break;
		default:
			usage(av[0]);
			break;
		}
	}

	printf(" + using seed: %u\n", seed);
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
            default:
                s = socket(evilint(), evilint(), evilint());
                break;
        }
		if (s == -1) continue;

		for (i = 0; i < opts; i++)
		{
			soopt(s);
			goof(s);
            usleep(500);
		}
        gsn(s);
        gpn(s);
        printf(".");fflush(stdout);
		close(s);

#ifdef __LINUX__
        if ( linux_hasoopsed() )
        {
            printf("dude, check your kernel! It has oopsed\n");
            getchar();
        }
#endif

		occ--;
	}

	printf(" + finished.\n");
	exit(EXIT_SUCCESS);
}
