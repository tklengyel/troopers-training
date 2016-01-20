#
# Copyright (C) 2016 Tamas K Lengyel
# ACADEMIC PUBLIC LICENSE
# For details please read the LICENSE file.
#
#!/bin/bash
VG="spvg";
ARGC=$#

if [ $ARGC -ne 1 ]; then
    echo "Please specify: <admin name>"
    exit
fi

NAME=$1;

xl destroy $NAME
lvremove -f /dev/$VG/$NAME
lvcreate -s -L20G -n $NAME /dev/$VG/troopers;

echo "Now copy and edit the configuration file! Don't forget to change the MAC address!"
