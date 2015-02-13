#!/bin/bash
if [ ! -d pesdk ]
then
    echo "ERROR:UPDATE PESDK FIRST"
    exit -1
fi
comake2 -UB
comake2
make
