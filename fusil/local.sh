export PYTHONPATH="`pwd`:`pwd`/ptrace:`pwd`/../untidy"
su -c 'echo 0 > /proc/sys/kernel/randomize_va_space'
