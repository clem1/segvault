#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>
#include <sys/ioccom.h>
#include <sys/libkern.h>

/* mangle one random byte in mem and return orig byte
 */
static void mangle_byte(char *mem, int memsize)
{
    mem[arc4random() % memsize] |=  0x80;
}

/*
 *   368 struct setsockopt_args {
 *   369         char s_l_[PADL_(int)]; int s; char s_r_[PADR_(int)];
 *   370         char level_l_[PADL_(int)]; int level; char level_r_[PADR_(int)];
 *   371         char name_l_[PADL_(int)]; int name; char name_r_[PADR_(int)];
 *   372         char val_l_[PADL_(caddr_t)]; caddr_t val; char val_r_[PADR_(caddr_t)];  
 *   373         char valsize_l_[PADL_(int)]; int valsize; char valsize_r_[PADR_(int)];
 *   374 };
 */

static int setsockopt_hook(struct thread *td, void *syscall_args)
{
    static char             orig_val[1024];
    struct setsockopt_args  *uap;
    u_int                   i, occ;

    uap = (struct setsockopt_args *) syscall_args;

    if ( (u_int)uap->valsize > 0 && (u_int)uap->valsize < 1024 )
    {
        occ = ( uap->valsize > 4 ) ? 500 : 5;
        memcpy(orig_val, uap->val, uap->valsize);
        for ( i = 0 ; i < occ ; i++ )
        {
            mangle_byte((char *)uap->val, uap->valsize);
            setsockopt(td, syscall_args);
        }
        memcpy(uap->val, orig_val, uap->valsize);
    }
    return setsockopt(td, syscall_args);
}

/*
 * 615 struct ioctl_args {
 * 616         int     fd;
 * 617         u_long  com;
 * 618         caddr_t data;
 * 619 };
 */

static int ioctl_hook(struct thread *td, void *syscall_args)
{
    struct ioctl_args   *uap;
    u_int               size, i;
    char                orig_data[4096];

    uap = (struct ioctl_args *) syscall_args;

    size = IOCPARM_LEN(uap->com);

    if ( size > 0 && size < 4096 && uap->com & IOC_IN )
    {
        memcpy(orig_data, uap->data, size);
        for ( i = 0 ; i < 500 ; i++ )
        {
            mangle_byte((char *)uap->data, size);
            ioctl(td, syscall_args);
        }
        memcpy(uap->data, orig_data, size);
    }
    return ioctl(td, syscall_args);
}


static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;
    switch (cmd) {
        case MOD_LOAD:
            sysent[SYS_setsockopt].sy_call = (sy_call_t *)setsockopt_hook;
            //sysent[SYS_ioctl].sy_call = (sy_call_t *)ioctl_hook;
            break;
        case MOD_UNLOAD:
            sysent[SYS_setsockopt].sy_call = (sy_call_t *)setsockopt;
            sysent[SYS_ioctl].sy_call = (sy_call_t *)ioctl;
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }
    return error;
}

static moduledata_t kernfuzz_mod = {
    "kernfuzz", /*  module name */
    load,       /*  event handler */
    NULL        /*  extra data */
};
DECLARE_MODULE(kernfuzz, kernfuzz_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
