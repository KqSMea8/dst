#!/bin/bash

HADOOP_DIR=../../hadoop
JAVA_DIR=../../java5

THIS_HADOOP=./hadoop-v1-for-client/hadoop
THIS_JAVA=./hadoop-v1-for-client/java5

rm -rf ./hadoop-v1-for-client
mkdir -p ./hadoop-v1-for-client

mkdir -p ${THIS_HADOOP}
mkdir -p ${THIS_JAVA}

mkdir -p ${THIS_HADOOP}/bin
cp -r ${HADOOP_DIR}/bin/hadoop ${THIS_HADOOP}/bin/
cp -r ${HADOOP_DIR}/bin/hadoop-config.sh ${THIS_HADOOP}/bin/
cp -r ${HADOOP_DIR}/bin/rcc ${THIS_HADOOP}/bin/

mkdir -p ${THIS_HADOOP}/conf
cp -r ${HADOOP_DIR}/conf/commons-logging.properties ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/hadoop-default.xml ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/hadoop-metrics.properties ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/log4j.properties ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/configuration.xsl ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/hadoop-env.sh ${THIS_HADOOP}/conf/
cp -r ${HADOOP_DIR}/conf/hadoop-site.xml ${THIS_HADOOP}/conf/

cp -r ${HADOOP_DIR}/lib ${THIS_HADOOP}/

mkdir -p ${THIS_HADOOP}/logs
mkdir -p ${THIS_HADOOP}/pids

mkdir -p ${THIS_HADOOP}/contrib
cp -r ${HADOOP_DIR}/contrib/*-streaming.jar ${THIS_HADOOP}/contrib/

cp ${HADOOP_DIR}/*-core.jar ${THIS_HADOOP}/ 

cp -r ${JAVA_DIR}/jre/bin ${THIS_JAVA}/
cp -r ${JAVA_DIR}/jre/lib ${THIS_JAVA}/ 

