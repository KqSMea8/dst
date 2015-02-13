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
  -DVERSION=\"1.5.5.0\" \
  -D_FILE_OFFSET_BITS=64
INCPATH=-I. \
  -I../../../../..//inf/computing/dstream/output/pesdk/include/ \
  -I../../../../..//inf/computing/dstream/output/pesdk/include/log4cplus \
  -I../../../../..//inf/computing/dstream/output/pesdk/include/zeromq
DEP_INCPATH=-I../../../../../com/btest/gtest \
  -I../../../../../com/btest/gtest/include \
  -I../../../../../com/btest/gtest/output \
  -I../../../../../com/btest/gtest/output/include \
  -I../../../../../com/idlcompiler \
  -I../../../../../com/idlcompiler/include \
  -I../../../../../com/idlcompiler/output \
  -I../../../../../com/idlcompiler/output/include \
  -I../../../../../inf/bigpipe/api/c-api \
  -I../../../../../inf/bigpipe/api/c-api/include \
  -I../../../../../inf/bigpipe/api/c-api/output \
  -I../../../../../inf/bigpipe/api/c-api/output/include \
  -I../../../../../inf/bigpipe/common \
  -I../../../../../inf/bigpipe/common/include \
  -I../../../../../inf/bigpipe/common/output \
  -I../../../../../inf/bigpipe/common/output/include \
  -I../../../../../inf/bigpipe/meta \
  -I../../../../../inf/bigpipe/meta/include \
  -I../../../../../inf/bigpipe/meta/output \
  -I../../../../../inf/bigpipe/meta/output/include \
  -I../../../../../inf/common/esp \
  -I../../../../../inf/common/esp/include \
  -I../../../../../inf/common/esp/output \
  -I../../../../../inf/common/esp/output/include \
  -I../../../../../inf/common/kylin \
  -I../../../../../inf/common/kylin/include \
  -I../../../../../inf/common/kylin/output \
  -I../../../../../inf/common/kylin/output/include \
  -I../../../../../inf/common/share \
  -I../../../../../inf/common/share/include \
  -I../../../../../inf/common/share/output \
  -I../../../../../inf/common/share/output/include \
  -I../../../../../inf/computing/ant \
  -I../../../../../inf/computing/ant/include \
  -I../../../../../inf/computing/ant/output \
  -I../../../../../inf/computing/ant/output/include \
  -I../../../../../inf/computing/dredis \
  -I../../../../../inf/computing/dredis/include \
  -I../../../../../inf/computing/dredis/output \
  -I../../../../../inf/computing/dredis/output/include \
  -I../../../../../inf/computing/dstream \
  -I../../../../../inf/computing/dstream/include \
  -I../../../../../inf/computing/dstream/output \
  -I../../../../../inf/computing/dstream/output/include \
  -I../../../../../inf/computing/java6 \
  -I../../../../../inf/computing/java6/include \
  -I../../../../../inf/computing/java6/output \
  -I../../../../../inf/computing/java6/output/include \
  -I../../../../../inf/computing/zookeeper \
  -I../../../../../inf/computing/zookeeper/include \
  -I../../../../../inf/computing/zookeeper/output \
  -I../../../../../inf/computing/zookeeper/output/include \
  -I../../../../../inf/doris/zkc \
  -I../../../../../inf/doris/zkc/include \
  -I../../../../../inf/doris/zkc/output \
  -I../../../../../inf/doris/zkc/output/include \
  -I../../../../../lib2-64/bsl \
  -I../../../../../lib2-64/bsl/include \
  -I../../../../../lib2-64/bsl/output \
  -I../../../../../lib2-64/bsl/output/include \
  -I../../../../../lib2-64/dict \
  -I../../../../../lib2-64/dict/include \
  -I../../../../../lib2-64/dict/output \
  -I../../../../../lib2-64/dict/output/include \
  -I../../../../../lib2-64/ullib \
  -I../../../../../lib2-64/ullib/include \
  -I../../../../../lib2-64/ullib/output \
  -I../../../../../lib2-64/ullib/output/include \
  -I../../../../../public/bslext \
  -I../../../../../public/bslext/include \
  -I../../../../../public/bslext/output \
  -I../../../../../public/bslext/output/include \
  -I../../../../../public/configure \
  -I../../../../../public/configure/include \
  -I../../../../../public/configure/output \
  -I../../../../../public/configure/output/include \
  -I../../../../../public/idlcompiler \
  -I../../../../../public/idlcompiler/include \
  -I../../../../../public/idlcompiler/output \
  -I../../../../../public/idlcompiler/output/include \
  -I../../../../../public/mcpack \
  -I../../../../../public/mcpack/include \
  -I../../../../../public/mcpack/output \
  -I../../../../../public/mcpack/output/include \
  -I../../../../../public/nshead \
  -I../../../../../public/nshead/include \
  -I../../../../../public/nshead/output \
  -I../../../../../public/nshead/output/include \
  -I../../../../../public/odict \
  -I../../../../../public/odict/include \
  -I../../../../../public/odict/output \
  -I../../../../../public/odict/output/include \
  -I../../../../../public/protobuf-json \
  -I../../../../../public/protobuf-json/include \
  -I../../../../../public/protobuf-json/output \
  -I../../../../../public/protobuf-json/output/include \
  -I../../../../../public/spreg \
  -I../../../../../public/spreg/include \
  -I../../../../../public/spreg/output \
  -I../../../../../public/spreg/output/include \
  -I../../../../../public/uconv \
  -I../../../../../public/uconv/include \
  -I../../../../../public/uconv/output \
  -I../../../../../public/uconv/output/include \
  -I../../../../../quality/autotest/bmock \
  -I../../../../../quality/autotest/bmock/include \
  -I../../../../../quality/autotest/bmock/output \
  -I../../../../../quality/autotest/bmock/output/include \
  -I../../../../../quality/autotest/reportlib/cpp \
  -I../../../../../quality/autotest/reportlib/cpp/include \
  -I../../../../../quality/autotest/reportlib/cpp/output \
  -I../../../../../quality/autotest/reportlib/cpp/output/include \
  -I../../../../../third-64/boost/include \
  -I../../../../../third-64/boost/include/boost \
  -I../../../../../third-64/boost/include/boost/tr1 \
  -I../../../../../third-64/libevent \
  -I../../../../../third-64/libevent/include \
  -I../../../../../third-64/libevent/output \
  -I../../../../../third-64/libevent/output/include \
  -I../../../../../third-64/pcre \
  -I../../../../../third-64/pcre/include \
  -I../../../../../third-64/pcre/output \
  -I../../../../../third-64/pcre/output/include \
  -I../../../../../third-64/protobuf \
  -I../../../../../third-64/protobuf/include \
  -I../../../../../third-64/protobuf/output \
  -I../../../../../third-64/protobuf/output/include \
  -I../../../../../third-64/stlport \
  -I../../../../../third-64/stlport/include \
  -I../../../../../third-64/stlport/output \
  -I../../../../../third-64/stlport/output/include \
  -I../../../../../third-64/thrift \
  -I../../../../../third-64/thrift/include \
  -I../../../../../third-64/thrift/output \
  -I../../../../../third-64/thrift/output/include \
  -I../../../../../thirdsrc/tcmalloc \
  -I../../../../../thirdsrc/tcmalloc/include \
  -I../../../../../thirdsrc/tcmalloc/output \
  -I../../../../../thirdsrc/tcmalloc/output/include

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=4060156a368cefac48225bd5ff6e9624  COMAKE


.PHONY:all
all:comake2_makefile_check echo_importer echo_pe_task echo_exporter tag_importer_add 
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
	rm -rf echo_importer
	rm -rf ./output/bin/echo_importer
	rm -rf echo_pe_task
	rm -rf ./output/bin/echo_pe_task
	rm -rf echo_exporter
	rm -rf ./output/bin/echo_exporter
	rm -rf tag_importer_add
	rm -rf ./output/bin/tag_importer_add
	rm -rf echo_importer_echo_pe_importer.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./echo_importer_echo.pb.o
	rm -rf echo_pe_task_echo_pe_task.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./echo_pe_task_echo.pb.o
	rm -rf echo_exporter_echo_pe_exporter.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./echo_exporter_echo.pb.o
	rm -rf tag_importer_add_echo_tag_importer_add.o
	rm -rf ./dynamic.pb.cc
	rm -rf ./dynamic.pb.h
	rm -rf ./tag_importer_add_dynamic.pb.o

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

echo_importer:echo_importer_echo_pe_importer.o \
  echo_importer_echo.pb.o \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_importer[0m']"
	$(CXX) echo_importer_echo_pe_importer.o \
  echo_importer_echo.pb.o -Xlinker "-(" ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a ../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../inf/bigpipe/api/c-api/libbigpipe_api.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../inf/common/esp/esp.a \
  ../../../../../inf/common/esp/output/libesp.a \
  ../../../../../inf/common/kylin/libkylin.a \
  ../../../../../inf/common/share/libshare.a \
  ../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../inf/computing/dstream/output/lib/libgeneric_operator.a \
  ../../../../../inf/computing/dstream/output/lib/libproto.a \
  ../../../../../inf/computing/dstream/output/lib/libstorage.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmetamanager.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmongoose.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libzmq.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_mt.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_st.a \
  ../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../public/configure/libconfig.a \
  ../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../public/mcpack/libmcpack.a \
  ../../../../../public/nshead/libnshead.a \
  ../../../../../public/odict/libodict.a \
  ../../../../../public/protobuf-json/output/lib/libjson-pb.a \
  ../../../../../public/spreg/libspreg.a \
  ../../../../../public/uconv/libuconv.a \
  ../../../../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../../../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../third-64/libevent/lib/libevent.a \
  ../../../../../third-64/libevent/lib/libevent_core.a \
  ../../../../../third-64/libevent/lib/libevent_extra.a \
  ../../../../../third-64/libevent/lib/libevent_openssl.a \
  ../../../../../third-64/libevent/lib/libevent_pthreads.a \
  ../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../third-64/thrift/lib/libthrift.a \
  ../../../../../third-64/thrift/lib/libthriftnb.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libprofiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_and_profiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_debug.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal_debug.a -L../../../../..//inf/computing/dstream/output/pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lcrypt \
  -lrt \
  -luuid -Xlinker "-)" -o echo_importer
	mkdir -p ./output/bin
	cp -f --link echo_importer ./output/bin

echo_pe_task:echo_pe_task_echo_pe_task.o \
  echo_pe_task_echo.pb.o \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_task[0m']"
	$(CXX) echo_pe_task_echo_pe_task.o \
  echo_pe_task_echo.pb.o -Xlinker "-(" ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a ../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../inf/bigpipe/api/c-api/libbigpipe_api.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../inf/common/esp/esp.a \
  ../../../../../inf/common/esp/output/libesp.a \
  ../../../../../inf/common/kylin/libkylin.a \
  ../../../../../inf/common/share/libshare.a \
  ../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../inf/computing/dstream/output/lib/libgeneric_operator.a \
  ../../../../../inf/computing/dstream/output/lib/libproto.a \
  ../../../../../inf/computing/dstream/output/lib/libstorage.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmetamanager.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmongoose.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libzmq.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_mt.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_st.a \
  ../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../public/configure/libconfig.a \
  ../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../public/mcpack/libmcpack.a \
  ../../../../../public/nshead/libnshead.a \
  ../../../../../public/odict/libodict.a \
  ../../../../../public/protobuf-json/output/lib/libjson-pb.a \
  ../../../../../public/spreg/libspreg.a \
  ../../../../../public/uconv/libuconv.a \
  ../../../../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../../../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../third-64/libevent/lib/libevent.a \
  ../../../../../third-64/libevent/lib/libevent_core.a \
  ../../../../../third-64/libevent/lib/libevent_extra.a \
  ../../../../../third-64/libevent/lib/libevent_openssl.a \
  ../../../../../third-64/libevent/lib/libevent_pthreads.a \
  ../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../third-64/thrift/lib/libthrift.a \
  ../../../../../third-64/thrift/lib/libthriftnb.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libprofiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_and_profiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_debug.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal_debug.a -L../../../../..//inf/computing/dstream/output/pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lcrypt \
  -lrt \
  -luuid -Xlinker "-)" -o echo_pe_task
	mkdir -p ./output/bin
	cp -f --link echo_pe_task ./output/bin

echo_exporter:echo_exporter_echo_pe_exporter.o \
  echo_exporter_echo.pb.o \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_exporter[0m']"
	$(CXX) echo_exporter_echo_pe_exporter.o \
  echo_exporter_echo.pb.o -Xlinker "-(" ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a ../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../inf/bigpipe/api/c-api/libbigpipe_api.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../inf/common/esp/esp.a \
  ../../../../../inf/common/esp/output/libesp.a \
  ../../../../../inf/common/kylin/libkylin.a \
  ../../../../../inf/common/share/libshare.a \
  ../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../inf/computing/dstream/output/lib/libgeneric_operator.a \
  ../../../../../inf/computing/dstream/output/lib/libproto.a \
  ../../../../../inf/computing/dstream/output/lib/libstorage.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmetamanager.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmongoose.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libzmq.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_mt.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_st.a \
  ../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../public/configure/libconfig.a \
  ../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../public/mcpack/libmcpack.a \
  ../../../../../public/nshead/libnshead.a \
  ../../../../../public/odict/libodict.a \
  ../../../../../public/protobuf-json/output/lib/libjson-pb.a \
  ../../../../../public/spreg/libspreg.a \
  ../../../../../public/uconv/libuconv.a \
  ../../../../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../../../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../third-64/libevent/lib/libevent.a \
  ../../../../../third-64/libevent/lib/libevent_core.a \
  ../../../../../third-64/libevent/lib/libevent_extra.a \
  ../../../../../third-64/libevent/lib/libevent_openssl.a \
  ../../../../../third-64/libevent/lib/libevent_pthreads.a \
  ../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../third-64/thrift/lib/libthrift.a \
  ../../../../../third-64/thrift/lib/libthriftnb.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libprofiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_and_profiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_debug.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal_debug.a -L../../../../..//inf/computing/dstream/output/pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lcrypt \
  -lrt \
  -luuid -Xlinker "-)" -o echo_exporter
	mkdir -p ./output/bin
	cp -f --link echo_exporter ./output/bin

tag_importer_add:tag_importer_add_echo_tag_importer_add.o \
  tag_importer_add_dynamic.pb.o \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtag_importer_add[0m']"
	$(CXX) tag_importer_add_echo_tag_importer_add.o \
  tag_importer_add_dynamic.pb.o -Xlinker "-(" ../../../../..//inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libproto.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../..//inf/computing/dstream/output/pesdk/lib/libmetamanager.a ../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../inf/bigpipe/api/c-api/libbigpipe_api.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../inf/common/esp/esp.a \
  ../../../../../inf/common/esp/output/libesp.a \
  ../../../../../inf/common/kylin/libkylin.a \
  ../../../../../inf/common/share/libshare.a \
  ../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../inf/computing/dstream/output/lib/libgeneric_operator.a \
  ../../../../../inf/computing/dstream/output/lib/libproto.a \
  ../../../../../inf/computing/dstream/output/lib/libstorage.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libcommon.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmetamanager.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libmongoose.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libpe.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libtransporter.a \
  ../../../../../inf/computing/dstream/output/pesdk/lib/libzmq.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_mt.a \
  ../../../../../inf/computing/zookeeper/output/c_client/lib/libzookeeper_st.a \
  ../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../public/configure/libconfig.a \
  ../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../public/mcpack/libmcpack.a \
  ../../../../../public/nshead/libnshead.a \
  ../../../../../public/odict/libodict.a \
  ../../../../../public/protobuf-json/output/lib/libjson-pb.a \
  ../../../../../public/spreg/libspreg.a \
  ../../../../../public/uconv/libuconv.a \
  ../../../../../quality/autotest/bmock/output/lib/libbmock.a \
  ../../../../../quality/autotest/bmock/output/lib/libgmock.a \
  ../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../third-64/libevent/lib/libevent.a \
  ../../../../../third-64/libevent/lib/libevent_core.a \
  ../../../../../third-64/libevent/lib/libevent_extra.a \
  ../../../../../third-64/libevent/lib/libevent_openssl.a \
  ../../../../../third-64/libevent/lib/libevent_pthreads.a \
  ../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../third-64/thrift/lib/libthrift.a \
  ../../../../../third-64/thrift/lib/libthriftnb.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libprofiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_and_profiler.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_debug.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal.a \
  ../../../../../thirdsrc/tcmalloc/output/lib/libtcmalloc_minimal_debug.a -L../../../../..//inf/computing/dstream/output/pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lcrypt \
  -lrt \
  -luuid -Xlinker "-)" -o tag_importer_add
	mkdir -p ./output/bin
	cp -f --link tag_importer_add ./output/bin

echo_importer_echo_pe_importer.o:echo_pe_importer.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_importer_echo_pe_importer.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o echo_importer_echo_pe_importer.o echo_pe_importer.cc

./echo.pb.cc \
  ./echo.pb.h:echo.proto
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./echo.pb.cc \
  ./echo.pb.h[0m']"
	../../../../..//third-64/protobuf/bin/protoc --cpp_out=./ --proto_path=./  echo.proto

echo.proto:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho.proto[0m']"
	@echo "ALREADY BUILT"

./echo_importer_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./echo_importer_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./echo_importer_echo.pb.o ./echo.pb.cc

echo_pe_task_echo_pe_task.o:echo_pe_task.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_task_echo_pe_task.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o echo_pe_task_echo_pe_task.o echo_pe_task.cc

./echo_pe_task_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./echo_pe_task_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./echo_pe_task_echo.pb.o ./echo.pb.cc

echo_exporter_echo_pe_exporter.o:echo_pe_exporter.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_exporter_echo_pe_exporter.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o echo_exporter_echo_pe_exporter.o echo_pe_exporter.cc

./echo_exporter_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./echo_exporter_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./echo_exporter_echo.pb.o ./echo.pb.cc

tag_importer_add_echo_tag_importer_add.o:echo_tag_importer_add.cc \
  dynamic.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtag_importer_add_echo_tag_importer_add.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o tag_importer_add_echo_tag_importer_add.o echo_tag_importer_add.cc

./dynamic.pb.cc \
  ./dynamic.pb.h:dynamic.proto
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./dynamic.pb.cc \
  ./dynamic.pb.h[0m']"
	../../../../..//third-64/protobuf/bin/protoc --cpp_out=./ --proto_path=./  dynamic.proto

dynamic.proto:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdynamic.proto[0m']"
	@echo "ALREADY BUILT"

./tag_importer_add_dynamic.pb.o:./dynamic.pb.cc \
  dynamic.pb.cc \
  dynamic.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./tag_importer_add_dynamic.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./tag_importer_add_dynamic.pb.o ./dynamic.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


