/* fstest.c -- 
 * Copyright 2006-07 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *     Steve Grubb <sgrubb@redhat.com>
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#if defined(__linux__)
#include <sys/vfs.h>
#include <sys/sendfile.h>
#elif defined(__FreeBSD__)
#include <sys/cdefs.h>
#include <sys/limits.h>
#include <sys/mount.h>
#elif defined(__NetBSD__)
#include <sys/cdefs.h>
#include <sys/mount.h>
#endif
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define LOGGIT 1
#if defined(__linux__)
#define CHECK_XATTR 1
#endif

#ifdef CHECK_XATTR
#include <attr/xattr.h>
#endif
#define MAXLISTBUF 65536

char origin[PATH_MAX];
const char *orig_dpath = NULL;
char *lbuf = NULL;
#if !defined(__NetBSD__)
struct statfs sfbuf;
#endif
struct stat sbuf;
int dev_null = -1, sfd = -1;
struct addrinfo *res=NULL, hints;

static void mlog(const char *msg)
{
#ifdef LOGGIT
	puts(msg);
#else
	msg = NULL;
#endif
}

#if !defined(__NetBSD__)
static void check_statfs(void)
{
	mlog("+statfs");
	statfs(orig_dpath, &sfbuf);
}
#endif

static void setup_socket(void)
{	// setup connection to xinetd discard
	if ((sfd < 0) && res) {
		struct addrinfo *ptr;

		for (ptr = res; ptr; ptr = ptr->ai_next) {
			if (ptr->ai_socktype != hints.ai_socktype)
				continue;

			sfd = socket(ptr->ai_family, 
				ptr->ai_socktype, ptr->ai_protocol);
			if (sfd < 0)
				continue;

			if (connect(sfd, ptr->ai_addr, ptr->ai_addrlen) < 0) {
				close(sfd);
				sfd = -1;
				continue;
			} else
				break;
		}
	}
}

static void file_read_checks(const char *path)
{
	char buf1[16],buf2[8],buf3[32];
	struct iovec vec[3];
	int rc, f, cnt;
	off_t i;

	mlog("+open");
	f = open(path, O_RDONLY);
	if (f < 0)
		return;

	// first look at the file's attributes
	mlog("+fstat");
	rc = fstat(f, &sbuf);
#ifdef CHECK_XATTR
	mlog("+flistxattr");
	flistxattr(f, lbuf, MAXLISTBUF);
	mlog("+fgetxattr");
	fgetxattr(f, "selinux", lbuf, MAXLISTBUF);
#endif
//	readahead(f, NULL

	// Check reading
	mlog("+read");
	cnt = 0;
	while((read(f, buf3, sizeof(buf3)) > 0) && cnt < 1000)
		cnt++;

	// lseek around
	mlog("+lseek");
	if (rc == 0) 
		lseek(f, sbuf.st_size, SEEK_SET);
	lseek(f, 100000, SEEK_SET);
	lseek(f, 0, SEEK_SET);

	// vectored reads
	vec[0].iov_base = (void*)&buf1;
	vec[0].iov_len = sizeof(buf1);
	vec[1].iov_base = (void*)&buf2;
	vec[1].iov_len = sizeof(buf2);
	vec[2].iov_base = (void*)&buf3;
	vec[2].iov_len = sizeof(buf3);
	mlog("+readv");
	cnt = 0;
	while((readv(f, vec, 3) > 0) && cnt < 1000)
		cnt++;

	// check out pread syscall
	i = 0;
	mlog("+pread");
	cnt = 0;
	while ((pread(f, buf1, sizeof(buf1), i) > 0) && cnt < 1000) {
		i += sizeof(buf1)*2;
		cnt++;
	}

	// flock
	mlog("+flock");
	flock(f, LOCK_SH|LOCK_NB);
	flock(f, LOCK_UN);

	// fcntl ?

	// sendfile to localhost:discard
#if defined(__linux__)
	setup_socket();
	if (sfd >= 0) {
		mlog("+sendfile");
		lseek(f, 0, SEEK_SET);
		sendfile(sfd, f, NULL, rc ? 100000 : sbuf.st_size);
		close(sfd);
		sfd = -1;
	}
#endif

	// mmap each file
	if (rc == 0) {
		char *src;
		mlog("+mmap");
		src = mmap(0, sbuf.st_size, PROT_READ, MAP_FILE | MAP_SHARED,
			f, 0);
		if (src != (char *)-1) {
			// Don't touch memory...or Mr. SIGBUS will visit you
			mlog("+munmap");
			munmap(src, sbuf.st_size);
		}
	}

	close(f);
}

static void check_dir(const char *dpath, int level)
{
	DIR *d;
	struct dirent *e;
	char path[PATH_MAX];
	int count = 0, dfd;

	if (level >= 4) {
		puts("excessive recursion detected");
		return;
	}

	// get directory listing
	mlog("+opendir");
	d = opendir(dpath);
	if (d == NULL) {
		printf("opendir failed: %s\n", strerror(errno));
		return;
	}

#if !defined(__NetBSD__)
	// fstatfs
	mlog("+fstatfs");
	dfd = dirfd(d);
	fstatfs(dfd, &sfbuf);
	while ((e = readdir(d))) {
		int rc = -1;

		if (strcmp(e->d_name, ".") == 0)
			continue;
		if (strcmp(e->d_name, "..") == 0)
			continue;

		count++;
		if (count > 64) {
			puts("excessive entries detected");
			break;
		}

		snprintf(path, sizeof(path), "%s/%s", dpath, e->d_name);

		// stat each
		mlog("+stat");
		rc = stat(path, &sbuf);
		if (rc == 0 && S_ISDIR(sbuf.st_mode)) {
			mlog("+chdir");
			if (chdir(path) == 0) {
				mlog("+getcwd");
				getcwd(path, sizeof(path));
				mlog("+fchdir");
				fchdir(dirfd(d));
				chdir(origin);
				check_dir(path, level+1);
			}
		} 
		mlog("+fstatat");
		rc = fstatat(dfd, path, &sbuf, AT_SYMLINK_NOFOLLOW);

		// access each
		mlog("+access");
		access(path, R_OK|W_OK|X_OK);
		mlog("+faccessat");
		faccessat(dfd, path, R_OK|W_OK|X_OK,
			AT_EACCESS|AT_SYMLINK_NOFOLLOW);

		// lstat each
		mlog("+lstat");
		lstat(path, &sbuf);

		// Check xattrs
#ifdef CHECK_XATTR
		mlog("+listxattr");
		listxattr(path, lbuf, MAXLISTBUF);
		mlog("+llistxattr");
		llistxattr(path, lbuf, MAXLISTBUF);
		
		mlog("+getxattr");
		getxattr(path, "selinux", lbuf, MAXLISTBUF);
		mlog("+lgetxattr");
		lgetxattr(path, "selinux", lbuf, MAXLISTBUF);
#endif
		if ( !(S_ISFIFO(sbuf.st_mode)||S_ISSOCK(sbuf.st_mode)) ) {
			int fd;

			mlog("+openat");
			fd = openat(dfd, path, O_RDONLY);
			close(fd);			
			file_read_checks(path);
			mlog("+readlink");
			readlink(path, lbuf, MAXLISTBUF);
			mlog("+readlinkat");
			readlinkat(dfd, path, lbuf, MAXLISTBUF);
		}
	}
#endif
	closedir(d);
}

static void dir_tests(const char *dpath)
{
	dpath = dpath; // shut up compiler 
	// new file tests
	// mkdir

	// mknod
	// rename
}


// Consider attr, read, write, full in config file
/* We should be passed the directory to be tested */
int main(int argc, char *argv[])
{
	int rc;

	if (argc != 2)
		exit(-1);

	// OK start the tests
	orig_dpath = argv[1];
	getcwd(origin, sizeof(origin));
#ifdef CHECK_XATTR
	lbuf = malloc(MAXLISTBUF+1);
	if (!lbuf)
		exit(-1);
#endif
	dev_null = open("/dev/null", O_RDWR);
	if (dev_null < 0)
		exit(-1);
	signal(SIGPIPE, SIG_IGN);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	// Setup address for later use	
	rc = getaddrinfo("localhost", "discard", &hints, &res);

	// fs tests
#if !defined(__NetBSD__)
	check_statfs();
#endif

	// directory tests
	check_dir(orig_dpath, 0);

	dir_tests(orig_dpath);
/*

file write tests
15) creat
truncate
ftruncate
write
16) writev
17) pwrite
16) fsync
17) fdatasync

file attr tests
chown
lchown
chmod
lchmod
quotactl
setxattr
fsetxattr
lsetxattr
removexattr
fremovexattr
lremovexattr
*/

	if (rc == 0)
		freeaddrinfo(res);
	if (sfd >= 0)
		close(sfd);
	if (dev_null >= 0)
		close(dev_null);
	free(lbuf);

	puts("++++ Tests finished");
	return 0;
}

