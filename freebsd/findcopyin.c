/*
 *
 * TODO:
 *  - kmalloc() through mkdir
 *  - x = mkdir(128)
 *  - save 8 first byte of copyin
 *  - call x in copyin
 *  - create fuzzer by taking x copy into account
 *  - memcpy fuzzer in x
 *  - hack
 */

#include <stdio.h>
#include <fcntl.h>
#include <kvm.h>
#include <nlist.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/sysent.h>
#include <sys/syscall.h>

int main(int argc, char **argv)
{
    kvm_t           *kd;
	struct nlist    nl[] = { { NULL }, { NULL } };
    unsigned char   basm[16];
    ssize_t         rasm, i;

	nl[0].n_name = "copyin";

	kd = kvm_openfiles(NULL, NULL, NULL, O_RDONLY, NULL);
	if( kd == NULL )
        exit(1337);

	if( kvm_nlist(kd, nl) < 0 )
        exit(1337);

	if( nl[0].n_value == 0 )
    {
		fprintf(stderr, "ERROR: copyin() not found!\n");
		exit(1337);
	}

    printf("%s is 0x%x at 0x%x!\n", nl[0].n_name, nl[0].n_type, nl[0].n_value);

	if( (rasm = kvm_read(kd, nl[0].n_value, basm, sizeof(basm))) > 0 )
    {
        for ( i = 0 ; i < rasm ; i++ )
            printf("\\x%02x", basm[i]);
        printf("\n");
	}

    kvm_close(kd);
	exit(0);
}
