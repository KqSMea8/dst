#!/bin/bash

if [[  -e ../../src/cc ]];then
    echo "Build Start in Source"
    cd ../../src/cc/processelement/
    ./release_sdk || Error "release sdk failed"
    cp -rd pesdk/ ../../../examples/dstream_local_example/
    cd -
else
	echo "Build Start in Product Library"
fi

comake2 || echo "dstream_local_example: comake2 failed"
make || echo "dstream_local_example: make failed"
