#!/bin/bash

Exchange(){
    mv COMAKE.all COMAKE
    mv src/cc/COMAKE.all src/cc/COMAKE
}

TestAndExample(){
######################## Build Test #############################
echo "start to cp utils for test"
cd src/test
sh build_test.sh
cd ../..

## copy example to output dir
echo "build examples ..."
cd examples || Error "example dir not exits"
sh build_example.sh || Error "build example failed"
cd ..
#rsync -vauP --exclude=".*" examples output
cp -vr examples output 
}

EchoLine(){
echo
echo ========================================================================================================
echo 
}

Error(){
echo -e "\033[0;31;1m" Error : $1 "\033[0;31;0m"
exit -1 
}

echo "start to build dstream...."

EchoLine
####################### check MAC=64 ############################
echo "start to check environment MAC=64 ..."
if [ "$MAC" != 64 ] ; then
  grep "export MAC=64"  ~/.bashrc >>/dev/null || (echo " ">>~/.bashrc && echo export MAC=64 >> ~/.bashrc)
  export MAC=64
  echo "MAC=64 not found in environment.So add export MAC=64 to ~/.bashrc and add it in the environment vars"
else
  echo "check MAC=64 OK."
fi

EchoLine 

######################## Build deps#############################
echo "start to build deps ..."
cd deps/src || Error "deps/src dir not exits."
sh build.sh || Error "Build deps failed"
cd ../..
EchoLine

######################## comake2 ###############################
echo "start to build ..."
rm -rf .copy-* src/cc/pm src/cc/pn
if [ "$1" == "quick" ]; then
    make || Error "make dstream failed"
    cd src/cc/processelement && sh release_sdk && cd - && rm -rf output/pesdk &&
    cp -r src/cc/processelement/pesdk output/pesdk
elif [ "$1" == "rd" ]; then
    Exchange
    comake2 -UB && comake2 || Error "comake2 failed"
    make || Error "make dstream failed"
    TestAndExample
elif [ $1"x" != "x" ]; then
    Exchange
    sed -i 's/.*test.*//' src/cc/COMAKE
    comake2 -UB && comake2 || Error "comake2 failed"
    make || Error "make dstream failed"
    TestAndExample
else
  make -j4 || Error "make dstream failed"   
fi

EchoLine
################################################################
echo -e "\033[0;32;1m Build Complete \033[0;31;0m"
