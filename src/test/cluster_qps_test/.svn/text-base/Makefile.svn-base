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
  -I../../../../../../public/comlog-plugin \
  -I../../../../../../public/comlog-plugin/include \
  -I../../../../../../public/comlog-plugin/output \
  -I../../../../../../public/comlog-plugin/output/include \
  -I../../../../../../public/configure \
  -I../../../../../../public/configure/include \
  -I../../../../../../public/configure/output \
  -I../../../../../../public/configure/output/include \
  -I../../../../../../public/connectpool \
  -I../../../../../../public/connectpool/include \
  -I../../../../../../public/connectpool/output \
  -I../../../../../../public/connectpool/output/include \
  -I../../../../../../public/gm/galileo \
  -I../../../../../../public/gm/galileo/include \
  -I../../../../../../public/gm/galileo/output \
  -I../../../../../../public/gm/galileo/output/include \
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
  -I../../../../../../public/ub \
  -I../../../../../../public/ub/include \
  -I../../../../../../public/ub/output \
  -I../../../../../../public/ub/output/include \
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
COMAKE_MD5=9c6fe952e24355fde06a3f57978e9213  COMAKE


.PHONY:all
all:comake2_makefile_check importer task exporter 
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
	rm -rf importer
	rm -rf ./output/bin/importer
	rm -rf task
	rm -rf ./output/bin/task
	rm -rf exporter
	rm -rf ./output/bin/exporter
	rm -rf importer_importer.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./importer_echo.pb.o
	rm -rf task_task.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./task_echo.pb.o
	rm -rf exporter_exporter.o
	rm -rf ./echo.pb.cc
	rm -rf ./echo.pb.h
	rm -rf ./exporter_echo.pb.o

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

importer:importer_importer.o \
  importer_echo.pb.o \
  ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mimporter[0m']"
	$(CXX) importer_importer.o \
  importer_echo.pb.o -Xlinker "-(" ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
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
  ../../../../../../public/comlog-plugin/libcomlog.a \
  ../../../../../../public/comlog-plugin/output/lib/libdfsappender.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/connectpool/libconnectpool.a \
  ../../../../../../public/gm/galileo/libgalileo.a \
  ../../../../../../public/gm/galileo/output/lib/libzookeeper_mt.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/ub/output/lib/libub.a \
  ../../../../../../public/ub/output/lib/libub_aserver.a \
  ../../../../../../public/ub/output/lib/libub_client.a \
  ../../../../../../public/ub/output/lib/libub_conf.a \
  ../../../../../../public/ub/output/lib/libub_galileo.a \
  ../../../../../../public/ub/output/lib/libub_log.a \
  ../../../../../../public/ub/output/lib/libub_misc.a \
  ../../../../../../public/ub/output/lib/libub_monitor.a \
  ../../../../../../public/ub/output/lib/libub_server.a \
  ../../../../../../public/ub/output/lib/libubex.a \
  ../../../../../../public/ub/output/lib/libubfw.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
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
  -luuid -Xlinker "-)" -o importer
	mkdir -p ./output/bin
	cp -f --link importer ./output/bin

task:task_task.o \
  task_echo.pb.o \
  ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtask[0m']"
	$(CXX) task_task.o \
  task_echo.pb.o -Xlinker "-(" ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
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
  ../../../../../../public/comlog-plugin/libcomlog.a \
  ../../../../../../public/comlog-plugin/output/lib/libdfsappender.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/connectpool/libconnectpool.a \
  ../../../../../../public/gm/galileo/libgalileo.a \
  ../../../../../../public/gm/galileo/output/lib/libzookeeper_mt.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/ub/output/lib/libub.a \
  ../../../../../../public/ub/output/lib/libub_aserver.a \
  ../../../../../../public/ub/output/lib/libub_client.a \
  ../../../../../../public/ub/output/lib/libub_conf.a \
  ../../../../../../public/ub/output/lib/libub_galileo.a \
  ../../../../../../public/ub/output/lib/libub_log.a \
  ../../../../../../public/ub/output/lib/libub_misc.a \
  ../../../../../../public/ub/output/lib/libub_monitor.a \
  ../../../../../../public/ub/output/lib/libub_server.a \
  ../../../../../../public/ub/output/lib/libubex.a \
  ../../../../../../public/ub/output/lib/libubfw.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
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
  -luuid -Xlinker "-)" -o task
	mkdir -p ./output/bin
	cp -f --link task ./output/bin

exporter:exporter_exporter.o \
  exporter_echo.pb.o \
  ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexporter[0m']"
	$(CXX) exporter_exporter.o \
  exporter_echo.pb.o -Xlinker "-(" ./pesdk//lib/libcommon.a \
  ./pesdk//lib/libproto.a \
  ./pesdk//lib/libpe.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../inf/bigpipe/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
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
  ../../../../../../public/comlog-plugin/libcomlog.a \
  ../../../../../../public/comlog-plugin/output/lib/libdfsappender.a \
  ../../../../../../public/configure/libconfig.a \
  ../../../../../../public/connectpool/libconnectpool.a \
  ../../../../../../public/gm/galileo/libgalileo.a \
  ../../../../../../public/gm/galileo/output/lib/libzookeeper_mt.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a \
  ../../../../../../public/nshead/libnshead.a \
  ../../../../../../public/odict/libodict.a \
  ../../../../../../public/spreg/libspreg.a \
  ../../../../../../public/ub/output/lib/libub.a \
  ../../../../../../public/ub/output/lib/libub_aserver.a \
  ../../../../../../public/ub/output/lib/libub_client.a \
  ../../../../../../public/ub/output/lib/libub_conf.a \
  ../../../../../../public/ub/output/lib/libub_galileo.a \
  ../../../../../../public/ub/output/lib/libub_log.a \
  ../../../../../../public/ub/output/lib/libub_misc.a \
  ../../../../../../public/ub/output/lib/libub_monitor.a \
  ../../../../../../public/ub/output/lib/libub_server.a \
  ../../../../../../public/ub/output/lib/libubex.a \
  ../../../../../../public/ub/output/lib/libubfw.a \
  ../../../../../../public/uconv/libuconv.a \
  ../../../../../../quality/autotest/reportlib/cpp/libautotest.a \
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
  -luuid -Xlinker "-)" -o exporter
	mkdir -p ./output/bin
	cp -f --link exporter ./output/bin

importer_importer.o:importer.cc \
  pesdk/include/common/logger.h \
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
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
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
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mimporter_importer.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o importer_importer.o importer.cc

./echo.pb.cc \
  ./echo.pb.h:echo.proto
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./echo.pb.cc \
  ./echo.pb.h[0m']"
	../../../../../..//third-64/protobuf/bin/protoc --cpp_out=./ --proto_path=./  echo.proto

echo.proto:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho.proto[0m']"
	@echo "ALREADY BUILT"

./importer_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./importer_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./importer_echo.pb.o ./echo.pb.cc

task_task.o:task.cc \
  pesdk/include/common/logger.h \
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
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
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
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mtask_task.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o task_task.o task.cc

./task_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./task_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./task_echo.pb.o ./echo.pb.cc

exporter_exporter.o:exporter.cc \
  pesdk/include/common/logger.h \
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
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/futex.h \
  pesdk/include/common/utils.h \
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
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mexporter_exporter.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o exporter_exporter.o exporter.cc

./exporter_echo.pb.o:./echo.pb.cc \
  echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40m./exporter_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o ./exporter_echo.pb.o ./echo.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


