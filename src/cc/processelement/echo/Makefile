SDK_ROOT=../pesdk

INCPATHS=-I. -I$(SDK_ROOT)/include \
 -I$(SDK_ROOT)/include/zeromq \
 -I$(SDK_ROOT)/include/protobuf \
 -I$(SDK_ROOT)/include/bigpipe 

LIBS=-L$(SDK_ROOT)/lib -lpe\
 -lzmq -lprotobuf -llog4cpp \
 -lpthread -lcrypto -lrt -luuid

CXXFLAGS=-fPIC -g -W -Wall

worker:echo.pb.o worker.o
	$(CC) -o $@ $(LIBS) $^
clean:
	rm -rf *.o worker dstream.pe.loopback
%.o:%.cc
	$(CC) -o $@ $^ $(CXXFLAGS) $(INCPATHS) -c
echo.pb.cc:echo.proto
	$(SDK_ROOT)/bin/protoc --cpp_out=. $^
