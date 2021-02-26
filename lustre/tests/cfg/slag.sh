
# =====================================
#
#    PRECONDITIONS
#
#    runas user exists (passwd files updated roughly hourly)
#    pools imported
#    letting auster create the datasets should eliminate these
#      lustre datasets already exist
#      file system root is read/write by user
#    utilities built (e.g. runas from runas.c)
#    no kernel module (.ko) files in the tree that do not match installed
#
# =====================================

MGSNID="172.19.1.24@o2ib100"
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

MGSZFSDEV="mgs/mgs"
MDSZFSDEV1="mgs/mdt1"
OSTZFSDEV1="ost1/ost1"

PDSH="pdsh -S -w"

SHARED_DIRECTORY=/tftpboot/dumps/runas/share

. $LUSTRE/tests/cfg/ncli.sh
