#!/bin/bash

source ./lib/fun.sh

hosts=`cat ./machines/test_machines/*`
uniq_string $hosts
echo $uniq_val
