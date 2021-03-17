
# =====================================
#
#    PRECONDITIONS
#    no lustre rpms installed (neither utilities nor kmod packages)
#    no lustre modules loaded (lnet service stopped, lsmod | grep libcfs shows nothing)
#    tree built with --with-zfs (so all utilities, libraries, and kernel modules built)
#    runas user exists (passwd files updated roughly hourly)
#    sparse files created for pools with 'truncate -s 512M <path>'
#      mds_HOST has file /tmp/${FSNAME}-mdt1 used to contain MDT pool
#      ost_HOST has file /tmp/${FSNAME}-ost1 used to contain OST pool
#
#    RUN TEST
#    (as root, from root of lustre tree (where autogen.sh and configure are)
#    ./lustre/tests/auster -f slag -r sanity --only 0
# =====================================

FSNAME=looze

mds_HOST=slag1
mdsfailover_HOST=slag2
mgs_HOST=slag1
ost_HOST=slag2
ostfailover_HOST=slag1
CLIENTS="slag11"
OSTCOUNT=1

FSNAME=${FSNAME:-lustre}
FSTYPE=zfs

LOAD_MODULES_REMOTE=true

PDSH="pdsh -S -w"

SHARED_DIRECTORY=/tftpboot/dumps/runas/share

. $LUSTRE/tests/cfg/ncli.sh
