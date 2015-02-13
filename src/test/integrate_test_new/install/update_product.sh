rm -rf product/deploy/*
cp -r ../../../../output/conf product/deploy
cp -r ../../../../output/utils product/deploy
cp -r ../../../../output/bin product/deploy
cp -r ../../../../output/lib64 product/deploy
cp -r ../../../../output/monitor product/deploy

cp -r ../../tools/hadoop-client product/deploy/utils/hadoop-client

dir=`pwd`
sh ../tools/utils/cm $dir/product/deploy/conf/dstream.cfg.xml /Cluster/PN/UseCglimit false

