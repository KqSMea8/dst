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
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64
INCPATH=-I. \
  -I./pesdk/include/ \
  -I./pesdk/include/log4cplus \
  -I./pesdk/include/zeromq
DEP_INCPATH=-I../../../../../../com/btest/gtest \
  -I../../../../../../com/btest/gtest/include \
  -I../../../../../../com/btest/gtest/output \
  -I../../../../../../com/btest/gtest/output/include \
  -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../inf/bigpipe/c-api \
  -I../../../../../../inf/bigpipe/c-api/include \
  -I../../../../../../inf/bigpipe/c-api/output \
  -I../../../../../../inf/bigpipe/c-api/output/include \
  -I../../../../../../inf/bigpipe/common \
  -I../../../../../../inf/bigpipe/common/include \
  -I../../../../../../inf/bigpipe/common/output \
  -I../../../../../../inf/bigpipe/common/output/include \
  -I../../../../../../inf/bigpipe/meta \
  -I../../../../../../inf/bigpipe/meta/include \
  -I../../../../../../inf/bigpipe/meta/output \
  -I../../../../../../inf/bigpipe/meta/output/include \
  -I../../../../../../inf/computing/dredis \
  -I../../../../../../inf/computing/dredis/include \
  -I../../../../../../inf/computing/dredis/output \
  -I../../../../../../inf/computing/dredis/output/include \
  -I../../../../../../inf/doris/zkc \
  -I../../../../../../inf/doris/zkc/include \
  -I../../../../../../inf/doris/zkc/output \
  -I../../../../../../inf/doris/zkc/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/dict \
  -I../../../../../../lib2-64/dict/include \
  -I../../../../../../lib2-64/dict/output \
  -I../../../../../../lib2-64/dict/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/bslext \
  -I../../../../../../public/bslext/include \
  -I../../../../../../public/bslext/output \
  -I../../../../../../public/bslext/output/include \
  -I../../../../../../public/configure \
  -I../../../../../../public/configure/include \
  -I../../../../../../public/configure/output \
  -I../../../../../../public/configure/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include \
  -I../../../../../../public/nshead \
  -I../../../../../../public/nshead/include \
  -I../../../../../../public/nshead/output \
  -I../../../../../../public/nshead/output/include \
  -I../../../../../../public/odict \
  -I../../../../../../public/odict/include \
  -I../../../../../../public/odict/output \
  -I../../../../../../public/odict/output/include \
  -I../../../../../../public/spreg \
  -I../../../../../../public/spreg/include \
  -I../../../../../../public/spreg/output \
  -I../../../../../../public/spreg/output/include \
  -I../../../../../../public/uconv \
  -I../../../../../../public/uconv/include \
  -I../../../../../../public/uconv/output \
  -I../../../../../../public/uconv/output/include \
  -I../../../../../../quality/autotest/reportlib/cpp \
  -I../../../../../../quality/autotest/reportlib/cpp/include \
  -I../../../../../../quality/autotest/reportlib/cpp/output \
  -I../../../../../../quality/autotest/reportlib/cpp/output/include \
  -I../../../../../../third-64/boost \
  -I../../../../../../third-64/boost/include \
  -I../../../../../../third-64/boost/output \
  -I../../../../../../third-64/boost/output/include \
  -I../../../../../../third-64/pcre \
  -I../../../../../../third-64/pcre/include \
  -I../../../../../../third-64/pcre/output \
  -I../../../../../../third-64/pcre/output/include \
  -I../../../../../../third-64/protobuf \
  -I../../../../../../third-64/protobuf/include \
  -I../../../../../../third-64/protobuf/output \
  -I../../../../../../third-64/protobuf/output/include \
  -I../../../../../../third-64/stlport \
  -I../../../../../../third-64/stlport/include \
  -I../../../../../../third-64/stlport/output \
  -I../../../../../../third-64/stlport/output/include \
  -I../../../../../../thirdsrc/zookeeper \
  -I../../../../../../thirdsrc/zookeeper/include \
  -I../../../../../../thirdsrc/zookeeper/output \
  -I../../../../../../thirdsrc/zookeeper/output/include

#============ CCP vars ============
CCHECK=@ccheck.py
CCHECK_FLAGS=
PCLINT=@pclint
PCLINT_FLAGS=
CCP=@ccp.py
CCP_FLAGS=


#COMAKE UUID
COMAKE_MD5=0bfa0e23e65464916a5b5a4e8c6fc312  COMAKE


.PHONY:all
all:comake2_makefile_check libjoiner_task.a joiner_task joiner_importer libjoiner_task_single.a joiner_task_single libjoiner_task_multi.a joiner_task_multi 
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
	rm -rf libjoiner_task.a
	rm -rf ./output/lib/libjoiner_task.a
	rm -rf joiner_task
	rm -rf ./output/bin/joiner_task
	rm -rf joiner_importer
	rm -rf ./output/bin/joiner_importer
	rm -rf libjoiner_task_single.a
	rm -rf ./output/lib/libjoiner_task_single.a
	rm -rf joiner_task_single
	rm -rf ./output/bin/joiner_task_single
	rm -rf libjoiner_task_multi.a
	rm -rf ./output/lib/libjoiner_task_multi.a
	rm -rf joiner_task_multi
	rm -rf ./output/bin/joiner_task_multi
	rm -rf joiner_task_joiner_task.o
	rm -rf joiner_task_storage_config.o
	rm -rf joiner_task_common_utils.o
	rm -rf joiner_task_joiner_task_main.o
	rm -rf joiner_task_joiner.pb.o
	rm -rf joiner_importer_joiner_importer.o
	rm -rf joiner_importer_joiner.pb.o
	rm -rf joiner_task_single_joiner_task_single.o
	rm -rf joiner_task_single_storage_config.o
	rm -rf joiner_task_single_common_utils.o
	rm -rf joiner_task_single_joiner_task_single_main.o
	rm -rf joiner_task_single_joiner.pb.o
	rm -rf joiner_task_multi_joiner_task_multi.o
	rm -rf joiner_task_multi_storage_config.o
	rm -rf joiner_task_multi_common_utils.o
	rm -rf joiner_task_multi_joiner_task_multi_main.o
	rm -rf joiner_task_multi_joiner.pb.o

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

libjoiner_task.a:joiner_task_joiner_task.o \
  joiner_task_storage_config.o \
  joiner_task_common_utils.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlibjoiner_task.a[0m']"
	ar crs libjoiner_task.a joiner_task_joiner_task.o \
  joiner_task_storage_config.o \
  joiner_task_common_utils.o
	mkdir -p ./output/lib
	cp -f --link libjoiner_task.a ./output/lib

joiner_task:joiner_task_joiner_task_main.o \
  joiner_task_joiner.pb.o \
  pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task[0m']"
	$(CXX) joiner_task_joiner_task_main.o \
  joiner_task_joiner.pb.o -Xlinker "-(" pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/java/libjava_skeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../com/idlcompiler/php/libphp_skeleton.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_mt.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_st.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid -Xlinker "-)" -o joiner_task
	mkdir -p ./output/bin
	cp -f --link joiner_task ./output/bin

joiner_importer:joiner_importer_joiner_importer.o \
  joiner_importer_joiner.pb.o \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_importer[0m']"
	$(CXX) joiner_importer_joiner_importer.o \
  joiner_importer_joiner.pb.o -Xlinker "-(" ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/java/libjava_skeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../com/idlcompiler/php/libphp_skeleton.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_mt.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_st.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid -Xlinker "-)" -o joiner_importer
	mkdir -p ./output/bin
	cp -f --link joiner_importer ./output/bin

libjoiner_task_single.a:joiner_task_single_joiner_task_single.o \
  joiner_task_single_storage_config.o \
  joiner_task_single_common_utils.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlibjoiner_task_single.a[0m']"
	ar crs libjoiner_task_single.a joiner_task_single_joiner_task_single.o \
  joiner_task_single_storage_config.o \
  joiner_task_single_common_utils.o
	mkdir -p ./output/lib
	cp -f --link libjoiner_task_single.a ./output/lib

joiner_task_single:joiner_task_single_joiner_task_single_main.o \
  joiner_task_single_joiner.pb.o \
  pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task_single.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single[0m']"
	$(CXX) joiner_task_single_joiner_task_single_main.o \
  joiner_task_single_joiner.pb.o -Xlinker "-(" pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task_single.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/java/libjava_skeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../com/idlcompiler/php/libphp_skeleton.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_mt.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_st.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid -Xlinker "-)" -o joiner_task_single
	mkdir -p ./output/bin
	cp -f --link joiner_task_single ./output/bin

libjoiner_task_multi.a:joiner_task_multi_joiner_task_multi.o \
  joiner_task_multi_storage_config.o \
  joiner_task_multi_common_utils.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlibjoiner_task_multi.a[0m']"
	ar crs libjoiner_task_multi.a joiner_task_multi_joiner_task_multi.o \
  joiner_task_multi_storage_config.o \
  joiner_task_multi_common_utils.o
	mkdir -p ./output/lib
	cp -f --link libjoiner_task_multi.a ./output/lib

joiner_task_multi:joiner_task_multi_joiner_task_multi_main.o \
  joiner_task_multi_joiner.pb.o \
  pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task_multi.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi[0m']"
	$(CXX) joiner_task_multi_joiner_task_multi_main.o \
  joiner_task_multi_joiner.pb.o -Xlinker "-(" pesdk/lib/libgeneric_operator.a \
  pesdk/lib/libstorage.a \
  libjoiner_task_multi.a \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/java/libjava_skeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../com/idlcompiler/php/libphp_skeleton.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../../inf/computing/dredis/output/lib/libhiredis.a \
  ../../../../../../inf/computing/dredis/output/lib/librclient.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_mock_unit_test.a \
  ../../../../../../inf/doris/zkc/libdoris_zkc_unit_test.a \
  ../../../../../../lib2-64/bsl/lib/libbsl.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_ResourcePool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_archive.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_buffer.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_check_cast.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_exception.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_pool.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_utils.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_implement.a \
  ../../../../../../lib2-64/bsl/lib/libbsl_var_utils.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/bslext/output/lib/libbsl_bml.a \
  ../../../../../../public/bslext/output/lib/libbsl_containers_utils.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_scripting.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_serializer.a \
  ../../../../../../public/bslext/output/lib/libbsl_var_vscript.a \
  ../../../../../../public/bslext/output/lib/libbsl_vs.a \
  ../../../../../../public/bslext/output/lib/libbslext.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
  ../../../../../../third-64/boost/lib/libboost_date_time.a \
  ../../../../../../third-64/boost/lib/libboost_filesystem.a \
  ../../../../../../third-64/boost/lib/libboost_graph.a \
  ../../../../../../third-64/boost/lib/libboost_iostreams.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99f.a \
  ../../../../../../third-64/boost/lib/libboost_math_c99l.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1f.a \
  ../../../../../../third-64/boost/lib/libboost_math_tr1l.a \
  ../../../../../../third-64/boost/lib/libboost_prg_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_program_options.a \
  ../../../../../../third-64/boost/lib/libboost_python.a \
  ../../../../../../third-64/boost/lib/libboost_random.a \
  ../../../../../../third-64/boost/lib/libboost_regex.a \
  ../../../../../../third-64/boost/lib/libboost_serialization.a \
  ../../../../../../third-64/boost/lib/libboost_signals.a \
  ../../../../../../third-64/boost/lib/libboost_system.a \
  ../../../../../../third-64/boost/lib/libboost_test_exec_monitor.a \
  ../../../../../../third-64/boost/lib/libboost_thread.a \
  ../../../../../../third-64/boost/lib/libboost_unit_test_framework.a \
  ../../../../../../third-64/boost/lib/libboost_wave.a \
  ../../../../../../third-64/boost/lib/libboost_wserialization.a \
  ../../../../../../third-64/pcre/lib/libpcre.a \
  ../../../../../../third-64/pcre/lib/libpcrecpp.a \
  ../../../../../../third-64/pcre/lib/libpcreposix.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf-lite.a \
  ../../../../../../third-64/protobuf/lib/libprotobuf.a \
  ../../../../../../third-64/protobuf/lib/libprotoc.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_mt.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_st.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid -Xlinker "-)" -o joiner_task_multi
	mkdir -p ./output/bin
	cp -f --link joiner_task_multi ./output/bin

joiner_task_joiner_task.o:joiner_task.cc \
  joiner_task.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_joiner_task.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_joiner_task.o joiner_task.cc

joiner_task_storage_config.o:storage_config.cc \
  storage_config.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_storage_config.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_storage_config.o storage_config.cc

joiner_task_common_utils.o:common_utils.cc \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_common_utils.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_common_utils.o common_utils.cc

joiner_task_joiner_task_main.o:joiner_task_main.cc \
  joiner_task.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_joiner_task_main.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_joiner_task_main.o joiner_task_main.cc

joiner_task_joiner.pb.o:joiner.pb.cc \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_joiner.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_joiner.pb.o joiner.pb.cc

joiner_importer_joiner_importer.o:joiner_importer.cc \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/processelement/context.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_importer_joiner_importer.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_importer_joiner_importer.o joiner_importer.cc

joiner_importer_joiner.pb.o:joiner.pb.cc \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_importer_joiner.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_importer_joiner.pb.o joiner.pb.cc

joiner_task_single_joiner_task_single.o:joiner_task_single.cc \
  joiner_task_single.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single_joiner_task_single.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_single_joiner_task_single.o joiner_task_single.cc

joiner_task_single_storage_config.o:storage_config.cc \
  storage_config.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single_storage_config.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_single_storage_config.o storage_config.cc

joiner_task_single_common_utils.o:common_utils.cc \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single_common_utils.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_single_common_utils.o common_utils.cc

joiner_task_single_joiner_task_single_main.o:joiner_task_single_main.cc \
  joiner_task_single.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single_joiner_task_single_main.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_single_joiner_task_single_main.o joiner_task_single_main.cc

joiner_task_single_joiner.pb.o:joiner.pb.cc \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_single_joiner.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_single_joiner.pb.o joiner.pb.cc

joiner_task_multi_joiner_task_multi.o:joiner_task_multi.cc \
  joiner_task_multi.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi_joiner_task_multi.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_multi_joiner_task_multi.o joiner_task_multi.cc

joiner_task_multi_storage_config.o:storage_config.cc \
  storage_config.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi_storage_config.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_multi_storage_config.o storage_config.cc

joiner_task_multi_common_utils.o:common_utils.cc \
  common_utils.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi_common_utils.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_multi_common_utils.o common_utils.cc

joiner_task_multi_joiner_task_multi_main.o:joiner_task_multi_main.cc \
  joiner_task.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cpp/Appender.hh \
  pesdk/include/log4cpp/Portability.hh \
  pesdk/include/log4cpp/config.h \
  pesdk/include/log4cpp/Export.hh \
  pesdk/include/log4cpp/Priority.hh \
  pesdk/include/log4cpp/Layout.hh \
  pesdk/include/log4cpp/LoggingEvent.hh \
  pesdk/include/log4cpp/TimeStamp.hh \
  pesdk/include/log4cpp/threading/Threading.hh \
  pesdk/include/log4cpp/threading/PThreads.hh \
  pesdk/include/log4cpp/BasicLayout.hh \
  pesdk/include/log4cpp/FileAppender.hh \
  pesdk/include/log4cpp/LayoutAppender.hh \
  pesdk/include/log4cpp/AppenderSkeleton.hh \
  pesdk/include/log4cpp/Filter.hh \
  pesdk/include/log4cpp/NDC.hh \
  pesdk/include/log4cpp/OstreamAppender.hh \
  pesdk/include/log4cpp/PatternLayout.hh \
  pesdk/include/log4cpp/Configurator.hh \
  pesdk/include/log4cpp/Category.hh \
  pesdk/include/log4cpp/CategoryStream.hh \
  pesdk/include/log4cpp/Manipulator.hh \
  pesdk/include/log4cpp/convenience.h \
  pesdk/include/log4cpp/RollingFileAppender.hh \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/common/type.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/common/proto_dynamic.h \
  pesdk/include/processelement/loopback.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/common/rpc_const.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/dstream_proto_common.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/cluster_config.pb.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/generic_operator/single_key_joiner.h \
  pesdk/include/generic_operator/joiner_context.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/config.h \
  pesdk/include/tinyxml/tinyxml.h \
  pesdk/include/tinyxml/tinystr.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/error.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/generic_operator/error.h \
  pesdk/include/generic_operator/timed_sliding_windows.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/utils.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/error.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/join_table.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/generic_operator/default_serializer.h \
  pesdk/include/generic_operator/multi_key_joiner.h \
  pesdk/include/generic_operator/multi_join_table.h \
  storage_config.h \
  pesdk/include/common/config.h \
  common_utils.h \
  pesdk/include/storage/dredis_sa.h \
  pesdk/include/storage/storage_accessor.h \
  pesdk/include/storage/storage_table.h \
  pesdk/include/storage/dredis.h \
  pesdk/include/storage/dredis_list.h \
  pesdk/include/storage/dredis_hash.h \
  pesdk/include/storage/dredis_sa.h \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi_joiner_task_multi_main.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_multi_joiner_task_multi_main.o joiner_task_multi_main.cc

joiner_task_multi_joiner.pb.o:joiner.pb.cc \
  joiner.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mjoiner_task_multi_joiner.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o joiner_task_multi_joiner.pb.o joiner.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


