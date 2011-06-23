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
#include <sys/socket.h>
#include <sys/uio.h>

#define OCC 50

#define ISROOT(td) ( td == NULL || td->td_proc == NULL || td->td_proc->p_ucred == NULL || td->td_proc->p_ucred->cr_ruid == 0 ) 

/* mangle random byte in mem
 */
static void mangle_byte(char *mem, int memsize)
{
    int i, count;

    if ( memsize <= 0 )
        return;

    count = arc4random() % memsize;

    for ( i = 0 ; i < count ; i++ )
    {
        unsigned int idx = arc4random() % memsize;
        switch ( arc4random() % 15 )
        {
            case 0:
                mem[idx] |= 0x80;
                break;
            case 1:
                mem[idx] = (char)(arc4random() & 0xFF);
                break;
            case 2:
                mem[idx]++;
                break;
            case 3:
                mem[idx]--;
                break;
            case 4:
                mem[idx] = 0;
                break;
            case 5:
                mem[idx] = 0xFF;
                break;
            default:
                break;
        }
    }
}

static int bind_hook(struct thread *td, void *syscall_args)
{
    int             i;
    static char     o[4096];
    struct bind_args *uap = (struct bind_args *)syscall_args;

    if ( ISROOT(td) || uap->namelen > 4096 )
        return bind(td, uap);

    memcpy(o, uap->name, uap->namelen);
    for ( i = 0 ; i < OCC ; i++ )
    {
        mangle_byte((char *)uap->name, uap->namelen);
        bind(td, uap);
    }
    memcpy(uap->name, o, uap->namelen);
	return bind(td, uap);
}

static int accept_hook(struct thread *td, void *syscall_args)
{
    int             i;
    static char     o[4096];
    struct accept_args *uap = (struct accept_args *)syscall_args;

    if ( ISROOT(td) || *uap->anamelen > 4096 )
        return accept(td, uap);

    memcpy(o, uap->name, *uap->anamelen);
    for ( i = 0 ; i < OCC ; i++ )
    {
        mangle_byte((char *)uap->name, *uap->anamelen);
        accept(td, uap);
    }
    memcpy(uap->name, o, *uap->anamelen);
	return accept(td, uap);
}

static int connect_hook(struct thread *td, void *syscall_args)
{
    int             i;
    static char     o[4096];
    struct connect_args *uap = (struct connect_args *)syscall_args;

    if ( ISROOT(td) || uap->namelen > 4096 )
        return connect(td, uap);

    memcpy(o, uap->name, uap->namelen);
    for ( i = 0 ; i < OCC ; i++ )
    {
        mangle_byte((char *)uap->name, uap->namelen);
        connect(td, uap);
    }
    memcpy(uap->name, o, uap->namelen);
	return connect(td, uap);
}

static int sendto_hook(struct thread *td, void *syscall_args)
{
    int             i, oflags;
    static char     obuf[4096], oto[4096];
    struct sendto_args *uap = (struct sendto_args *)syscall_args;

    if ( ISROOT(td) || uap->tolen > 4096 || uap->len > 4096 )
        return sendto(td, uap);

    memcpy(obuf, uap->buf, uap->len);
    memcpy(oto, uap->to, uap->tolen);
    oflags = uap->flags;
    for ( i = 0 ; i < OCC ; i++ )
    {
        if ( arc4random() % 2 == 1 )
            mangle_byte((char *)uap->buf, uap->len);
        if ( arc4random() % 2 == 1 )
            mangle_byte((char *)uap->to, uap->tolen);
        if ( arc4random() % 2 == 1 )
            uap->flags = arc4random();
        sendto(td, uap);
    }
    memcpy(uap->buf, obuf, uap->len);
    memcpy(uap->to, oto, uap->tolen);
    uap->flags = oflags;
	return sendto(td, uap);
}

static int sendmsg_hook(struct thread *td, void *syscall_args)
{
    int             i, oflags, error;
    struct sendmsg_args *uap = (struct sendmsg_args *)syscall_args;
    struct msghdr   msg, *omsg;
    struct iovec    *iov;

    if ( ISROOT(td) )
        return sendmsg(td, uap);

	error = copyin(uap->msg, &msg, sizeof (msg));
	if (error)
		return (error);
	error = copyiniov(msg.msg_iov, msg.msg_iovlen, &iov, EMSGSIZE);
	if (error)
		return (error);
	msg.msg_iov = iov;

    omsg = uap->msg;
    uap->msg = &msg;
    oflags = uap->flags;
    for ( i = 0 ; i < OCC ; i++ )
    {
        if ( arc4random() % 5 == 1 )
            mangle_byte((char *)msg.msg_name, msg.msg_namelen);
        if ( arc4random() % 5 == 1 )
            mangle_byte((char *)msg.msg_control, msg.msg_controllen);
        if ( arc4random() % 5 == 1 )
            mangle_byte((char *)iov->iov_base, iov->iov_len);
        if ( arc4random() % 5 == 1 )
            uap->flags = arc4random();
        sendmsg(td, uap);
    }

    uap->msg = omsg;
    uap->flags = oflags;
	return sendmsg(td, uap);
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
    u_int                   i;

    uap = (struct setsockopt_args *) syscall_args;

    if ( ISROOT(td) || (u_int)uap->valsize < 4096 )
        return setsockopt(td, syscall_args);

    memcpy(orig_val, uap->val, uap->valsize);
    for ( i = 0 ; i < OCC ; i++ )
    {
        mangle_byte((char *)uap->val, uap->valsize);
        setsockopt(td, syscall_args);
    }
    memcpy(uap->val, orig_val, uap->valsize);
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
        for ( i = 0 ; i < OCC ; i++ )
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
            sysent[SYS_ioctl].sy_call = (sy_call_t *)ioctl_hook;
            sysent[SYS_bind].sy_call = (sy_call_t *)bind_hook;
            sysent[SYS_accept].sy_call = (sy_call_t *)accept_hook;
            sysent[SYS_connect].sy_call = (sy_call_t *)connect_hook;
            sysent[SYS_sendto].sy_call = (sy_call_t *)sendto_hook;
            sysent[SYS_sendmsg].sy_call = (sy_call_t *)sendmsg_hook;
            break;
        case MOD_UNLOAD:
            sysent[SYS_setsockopt].sy_call = (sy_call_t *)setsockopt;
            sysent[SYS_ioctl].sy_call = (sy_call_t *)ioctl;
            sysent[SYS_bind].sy_call = (sy_call_t *)bind;
            sysent[SYS_accept].sy_call = (sy_call_t *)accept;
            sysent[SYS_connect].sy_call = (sy_call_t *)connect;
            sysent[SYS_sendto].sy_call = (sy_call_t *)sendto;
            sysent[SYS_sendmsg].sy_call = (sy_call_t *)sendmsg;
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
