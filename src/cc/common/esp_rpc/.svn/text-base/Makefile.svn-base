#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
CC=gcc
CXX=g++
CXXFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC
CFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC
CPPFLAGS=-D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.5.0.0\" \
  -D_FILE_OFFSET_BITS=64
INCPATH=-I.
DEP_INCPATH=-I../../com/btest/gtest \
  -I../../com/btest/gtest/include \
  -I../../com/btest/gtest/output \
  -I../../com/btest/gtest/output/include \
  -I../../inf/common/esp \
  -I../../inf/common/esp/include \
  -I../../inf/common/esp/output \
  -I../../inf/common/esp/output/include \
  -I../../inf/common/kylin \
  -I../../inf/common/kylin/include \
  -I../../inf/common/kylin/output \
  -I../../inf/common/kylin/output/include \
  -I../../inf/common/share \
  -I../../inf/common/share/include \
  -I../../inf/common/share/output \
  -I../../inf/common/share/output/include \
  -I../../quality/autotest/bmock \
  -I../../quality/autotest/bmock/include \
  -I../../quality/autotest/bmock/output \
  -I../../quality/autotest/bmock/output/include \
  -I../../quality/autotest/reportlib/cpp \
  -I../../quality/autotest/reportlib/cpp/include \
  -I../../quality/autotest/reportlib/cpp/output \
  -I../../quality/autotest/reportlib/cpp/output/include \
  -I../../third-64/boost \
  -I../../third-64/boost/include \
  -I../../third-64/boost/output \
  -I../../third-64/boost/output/include \
  -I../../third-64/protobuf \
  -I../../third-64/protobuf/include \
  -I../../third-64/protobuf/output \
  -I../../third-64/protobuf/output/include

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=e3339dd8ae4830daf4eadf4c2a0c502e  COMAKE


.PHONY:all
all:comake2_makefile_check server client 
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:comake2_makefile_check
comake2_makefile_check:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mcomake2_makefile_check[0m']"
	#in case of error, update 'Makefile' by 'comake2'
	@echo "$(COMAKE_MD5)">comake2.md5
	@md5sum -c --status comake2.md5
	@rm -f comake2.md5

.PHONY:ccpclean
ccpclean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mccpclean[0m']"
	@echo "make ccpclean done"

.PHONY:clean
clean:ccpclean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf server
	rm -rf ./output/bin/server
	rm -rf client
	rm -rf ./output/bin/client
	rm -rf example/server_my_esp_server.o
	rm -rf esp_rpc/server_esp_rpc_server.o
	rm -rf esp_rpc/esp_rpc.pb.cc
	rm -rf esp_rpc/esp_rpc.pb.h
	rm -rf esp_rpc/server_esp_rpc.pb.o
	rm -rf example/echo.pb.cc
	rm -rf example/echo.pb.h
	rm -rf example/server_echo.pb.o
	rm -rf example/client_my_esp_client.o
	rm -rf esp_rpc/client_esp_rpc_client.o
	rm -rf esp_rpc/client_esp_rpc_channel.o
	rm -rf esp_rpc/esp_rpc.pb.cc
	rm -rf esp_rpc/esp_rpc.pb.h
	rm -rf esp_rpc/client_esp_rpc.pb.o
	rm -rf example/echo.pb.cc
	rm -rf example/echo.pb.h
	rm -rf example/client_echo.pb.o

.PHONY:dist
dist:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdist[0m']"
	tar czvf output.tar.gz output
	@echo "make dist done"

.PHONY:distclean
distclean:clean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdistclean[0m']"
	rm -f output.tar.gz
	@echo "make distclean done"

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

server:example/server_my_esp_server.o \
  esp_rpc/server_esp_rpc_server.o \
  esp_rpc/server_esp_rpc.pb.o \
  example/server_echo.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mserver[0m']"
	$(CXX) example/server_my_esp_server.o \
  esp_rpc/server_esp_rpc_server.o \
  esp_rpc/server_esp_rpc.pb.o \
  example/server_echo.pb.o -Xlinker "-("  ../../com/btest/gtest/output/lib/libgtest.a \
  ../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../inf/common/esp/esp.a \
  ../../inf/common/esp/output/libesp.a \
  ../../inf/common/kylin/libkylin.a \
  ../../inf/common/share/libshare.a \
  ../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../third-64/boost/lib/libboost_date_time.a \
  ../../third-64/boost/lib/libboost_filesystem.a \
  ../../third-64/boost/lib/libboost_graph.a \
  ../../third-64/boost/lib/libboost_iostreams.a \
  ../../third-64/boost/lib/libboost_math_c99.a \
  ../../third-64/boost/lib/libboost_math_c99f.a \
  ../../third-64/boost/lib/libboost_math_c99l.a \
  ../../third-64/boost/lib/libboost_math_tr1.a \
  ../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../third-64/boost/lib/libboost_program_options.a \
  ../../third-64/boost/lib/libboost_python.a \
  ../../third-64/boost/lib/libboost_random.a \
  ../../third-64/boost/lib/libboost_regex.a \
  ../../third-64/boost/lib/libboost_serialization.a \
  ../../third-64/boost/lib/libboost_signals.a \
  ../../third-64/boost/lib/libboost_system.a \
  ../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../third-64/boost/lib/libboost_thread.a \
  ../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../third-64/boost/lib/libboost_wave.a \
  ../../third-64/boost/lib/libboost_wserialization.a \
  ../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../third-64/protobuf/lib/libprotobuf.a \
  ../../third-64/protobuf/lib/libprotoc.a -lpthread -Xlinker "-)" -o server
	mkdir -p ./output/bin
	cp -f --link server ./output/bin

client:example/client_my_esp_client.o \
  esp_rpc/client_esp_rpc_client.o \
  esp_rpc/client_esp_rpc_channel.o \
  esp_rpc/client_esp_rpc.pb.o \
  example/client_echo.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclient[0m']"
	$(CXX) example/client_my_esp_client.o \
  esp_rpc/client_esp_rpc_client.o \
  esp_rpc/client_esp_rpc_channel.o \
  esp_rpc/client_esp_rpc.pb.o \
  example/client_echo.pb.o -Xlinker "-("  ../../com/btest/gtest/output/lib/libgtest.a \
  ../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../inf/common/esp/esp.a \
  ../../inf/common/esp/output/libesp.a \
  ../../inf/common/kylin/libkylin.a \
  ../../inf/common/share/libshare.a \
  ../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../third-64/boost/lib/libboost_date_time.a \
  ../../third-64/boost/lib/libboost_filesystem.a \
  ../../third-64/boost/lib/libboost_graph.a \
  ../../third-64/boost/lib/libboost_iostreams.a \
  ../../third-64/boost/lib/libboost_math_c99.a \
  ../../third-64/boost/lib/libboost_math_c99f.a \
  ../../third-64/boost/lib/libboost_math_c99l.a \
  ../../third-64/boost/lib/libboost_math_tr1.a \
  ../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../third-64/boost/lib/libboost_program_options.a \
  ../../third-64/boost/lib/libboost_python.a \
  ../../third-64/boost/lib/libboost_random.a \
  ../../third-64/boost/lib/libboost_regex.a \
  ../../third-64/boost/lib/libboost_serialization.a \
  ../../third-64/boost/lib/libboost_signals.a \
  ../../third-64/boost/lib/libboost_system.a \
  ../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../third-64/boost/lib/libboost_thread.a \
  ../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../third-64/boost/lib/libboost_wave.a \
  ../../third-64/boost/lib/libboost_wserialization.a \
  ../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../third-64/protobuf/lib/libprotobuf.a \
  ../../third-64/protobuf/lib/libprotoc.a -lpthread -Xlinker "-)" -o client
	mkdir -p ./output/bin
	cp -f --link client ./output/bin

example/server_my_esp_server.o:example/my_esp_server.cpp \
  esp_rpc/esp_rpc_server.h \
  esp_rpc/esp_rpc_common.h \
  esp_rpc/esp_rpc.pb.h \
  example/echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/server_my_esp_server.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o example/server_my_esp_server.o example/my_esp_server.cpp

esp_rpc/server_esp_rpc_server.o:esp_rpc/esp_rpc_server.cc \
  esp_rpc/esp_rpc_server.h \
  esp_rpc/esp_rpc_common.h \
  esp_rpc/esp_rpc.pb.h \
  esp_rpc/esp_rpc.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/server_esp_rpc_server.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o esp_rpc/server_esp_rpc_server.o esp_rpc/esp_rpc_server.cc

esp_rpc/esp_rpc.pb.cc \
  esp_rpc/esp_rpc.pb.h:esp_rpc/esp_rpc.proto
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/esp_rpc.pb.cc \
  esp_rpc/esp_rpc.pb.h[0m']"
	./protoc --cpp_out=esp_rpc --proto_path=esp_rpc  esp_rpc/esp_rpc.proto

esp_rpc/esp_rpc.proto:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/esp_rpc.proto[0m']"
	@echo "ALREADY BUILT"

esp_rpc/server_esp_rpc.pb.o:esp_rpc/esp_rpc.pb.cc \
  esp_rpc/esp_rpc.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/server_esp_rpc.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o esp_rpc/server_esp_rpc.pb.o esp_rpc/esp_rpc.pb.cc

example/echo.pb.cc \
  example/echo.pb.h:example/echo.proto
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/echo.pb.cc \
  example/echo.pb.h[0m']"
	./protoc --cpp_out=example --proto_path=example  example/echo.proto

example/echo.proto:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/echo.proto[0m']"
	@echo "ALREADY BUILT"

example/server_echo.pb.o:example/echo.pb.cc \
  example/echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/server_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o example/server_echo.pb.o example/echo.pb.cc

example/client_my_esp_client.o:example/my_esp_client.cpp \
  esp_rpc/esp_rpc_channel.h \
  esp_rpc/esp_rpc_client.h \
  esp_rpc/esp_rpc_common.h \
  esp_rpc/esp_rpc.pb.h \
  example/echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/client_my_esp_client.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o example/client_my_esp_client.o example/my_esp_client.cpp

esp_rpc/client_esp_rpc_client.o:esp_rpc/esp_rpc_client.cc \
  esp_rpc/esp_rpc_client.h \
  esp_rpc/esp_rpc_common.h \
  esp_rpc/esp_rpc.pb.h \
  esp_rpc/esp_rpc.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/client_esp_rpc_client.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o esp_rpc/client_esp_rpc_client.o esp_rpc/esp_rpc_client.cc

esp_rpc/client_esp_rpc_channel.o:esp_rpc/esp_rpc_channel.cc \
  esp_rpc/esp_rpc_channel.h \
  esp_rpc/esp_rpc_client.h \
  esp_rpc/esp_rpc_common.h \
  esp_rpc/esp_rpc.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/client_esp_rpc_channel.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o esp_rpc/client_esp_rpc_channel.o esp_rpc/esp_rpc_channel.cc

esp_rpc/client_esp_rpc.pb.o:esp_rpc/esp_rpc.pb.cc \
  esp_rpc/esp_rpc.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mesp_rpc/client_esp_rpc.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o esp_rpc/client_esp_rpc.pb.o esp_rpc/esp_rpc.pb.cc

example/client_echo.pb.o:example/echo.pb.cc \
  example/echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexample/client_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o example/client_echo.pb.o example/echo.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


