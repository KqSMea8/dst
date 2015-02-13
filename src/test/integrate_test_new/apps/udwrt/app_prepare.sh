#!/bin/bash
rm -rf rt_s*
echo "===========Download app from icafe=================="
./sshexec/sshpass -p getprod scp -r getprod@product.scm.baidu.com:/data/prod-unit/prod-64/inf/budw/rt/rt_1-0-0-0_PD_BL/product/* ./output
cd output
mv rt_importer/ runse_importer/
mv rt_pe/ rtpe
cd ..
cp -rf udwrt_conf/* output/
mv output/rt_select ./
echo "===========Update configs form udwrt_conf/=================="
mv output/ rt_submit/
echo "===========Add source================"
cd rt_submit/rtpe/
./dostartup
cd -
cd rt_select
./rt_select nstream "select time, wiseuserid, product_type, source_type, word into UFS4Mobile from WiseLogSe"
cd -
