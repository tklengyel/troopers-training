#
# Copyright (C) 2016 Tamas K Lengyel
# ACADEMIC PUBLIC LICENSE
# For details please read the LICENSE file.
#
#!/bin/bash
VG="spvg";
ARGC=$#

if [ $ARGC -ne 3 ]; then
    echo "Please specify: <guest name> <admin name> <path to cfg>"
    exit
fi

NAME=$1;
ADMIN=$2;
CFG=$3;
ADMINDOMID=$(xl domid $ADMIN);

xl destroy $NAME
lvremove -f /dev/$VG/$NAME
lvcreate -s -L20G -n $NAME /dev/$VG/troopers;
xl create -p $CFG
GUESTDOMID=$(xl domid $NAME)
xenstore-chmod -ru /local/domain/$GUESTDOMID/name r$GUESTDOMID r$ADMINDOMID
xenstore-chmod -ru /local/domain/$GUESTDOMID/domid r$GUESTDOMID r$ADMINDOMID
xl unpause $NAME
