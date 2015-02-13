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
  -I../../../../../../inf/bigpipe/api/c-api \
  -I../../../../../../inf/bigpipe/api/c-api/include \
  -I../../../../../../inf/bigpipe/api/c-api/output \
  -I../../../../../../inf/bigpipe/api/c-api/output/include \
  -I../../../../../../inf/bigpipe/common/include \
  -I../../../../../../inf/bigpipe/common/include/protocol \
  -I../../../../../../inf/bigpipe/meta \
  -I../../../../../../inf/bigpipe/meta/include \
  -I../../../../../../inf/bigpipe/meta/output \
  -I../../../../../../inf/bigpipe/meta/output/include \
  -I../../../../../../inf/common/esp \
  -I../../../../../../inf/common/esp/include \
  -I../../../../../../inf/common/esp/output \
  -I../../../../../../inf/common/esp/output/include \
  -I../../../../../../inf/common/kylin \
  -I../../../../../../inf/common/kylin/include \
  -I../../../../../../inf/common/kylin/output \
  -I../../../../../../inf/common/kylin/output/include \
  -I../../../../../../inf/common/share \
  -I../../../../../../inf/common/share/include \
  -I../../../../../../inf/common/share/output \
  -I../../../../../../inf/common/share/output/include \
  -I../../../../../../inf/doris/zkc \
  -I../../../../../../inf/doris/zkc/include \
  -I../../../../../../inf/doris/zkc/output \
  -I../../../../../../inf/doris/zkc/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ccode \
  -I../../../../../../lib2-64/ccode/include \
  -I../../../../../../lib2-64/ccode/output \
  -I../../../../../../lib2-64/ccode/output/include \
  -I../../../../../../lib2-64/dict \
  -I../../../../../../lib2-64/dict/include \
  -I../../../../../../lib2-64/dict/output \
  -I../../../../../../lib2-64/dict/output/include \
  -I../../../../../../lib2-64/string \
  -I../../../../../../lib2-64/string/include \
  -I../../../../../../lib2-64/string/output \
  -I../../../../../../lib2-64/string/output/include \
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
  -I../../../../../../public/ubmonitor \
  -I../../../../../../public/ubmonitor/include \
  -I../../../../../../public/ubmonitor/output \
  -I../../../../../../public/ubmonitor/output/include \
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
  -I../../../../../../third-64/tcmalloc \
  -I../../../../../../third-64/tcmalloc/include \
  -I../../../../../../third-64/tcmalloc/output \
  -I../../../../../../third-64/tcmalloc/output/include \
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
COMAKE_MD5=aee798fbdc7225225e28723469bfa2bf  COMAKE


.PHONY:all
all:comake2_makefile_check echo_pe 
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
	rm -rf echo_pe
	rm -rf ./output/bin/echo_pe
	rm -rf echo_pe_echo_pe.o
	rm -rf echo_pe_echo.pb.o

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

echo_pe:echo_pe_echo_pe.o \
  echo_pe_echo.pb.o \
  ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a \
  ./pesdk/lib/libtransporter.a \
  ./pesdk/lib/libmetamanager.a
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe[0m']"
	$(CXX) echo_pe_echo_pe.o \
  echo_pe_echo.pb.o -Xlinker "-(" ./pesdk/lib/libcommon.a \
  ./pesdk/lib/libproto.a \
  ./pesdk/lib/libpe.a \
  ./pesdk/lib/libtransporter.a \
  ./pesdk/lib/libmetamanager.a ../../../../../../com/btest/gtest/output/lib/libgtest.a \
  ../../../../../../com/btest/gtest/output/lib/libgtest_main.a \
  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/java/libjava_skeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
  ../../../../../../com/idlcompiler/php/libphp_skeleton.a \
  ../../../../../../inf/bigpipe/api/c-api/libbigpipe_api.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common.a \
  ../../../../../../inf/bigpipe/common/libbigpipe_common_for_test.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta.a \
  ../../../../../../inf/bigpipe/meta/libbigpipe_meta_for_test.a \
  ../../../../../../inf/common/esp/esp.a \
  ../../../../../../inf/common/esp/output/libesp.a \
  ../../../../../../inf/common/kylin/libkylin.a \
  ../../../../../../inf/common/share/libshare.a \
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
  ../../../../../../lib2-64/ccode/lib/libulccode.a \
  ../../../../../../lib2-64/dict/lib/libuldict.a \
  ../../../../../../lib2-64/string/lib/libstring.a \
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
  ../../../../../../public/ubmonitor/libub_monitor.a \
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
  ../../../../../../third-64/tcmalloc/lib/libprofiler.a \
  ../../../../../../third-64/tcmalloc/lib/libtcmalloc.a \
  ../../../../../../third-64/tcmalloc/lib/libtcmalloc_and_profiler.a \
  ../../../../../../third-64/tcmalloc/lib/libtcmalloc_debug.a \
  ../../../../../../third-64/tcmalloc/lib/libtcmalloc_minimal.a \
  ../../../../../../third-64/tcmalloc/lib/libtcmalloc_minimal_debug.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_mt.a \
  ../../../../../../thirdsrc/zookeeper/output/lib/libzookeeper_st.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -llog4cplus \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lcrypt -Xlinker "-)" -o echo_pe
	mkdir -p ./output/bin
	cp -f --link echo_pe ./output/bin

echo_pe_echo_pe.o:echo_pe.cc \
  pesdk/include/common/logger.h \
  pesdk/include/common/logger.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/config.hxx \
  pesdk/include/log4cplus/config/defines.hxx \
  pesdk/include/log4cplus/helpers/thread-config.h \
  pesdk/include/log4cplus/layout.h \
  pesdk/include/log4cplus/loglevel.h \
  pesdk/include/log4cplus/tstring.h \
  pesdk/include/log4cplus/tchar.h \
  pesdk/include/log4cplus/streams.h \
  pesdk/include/log4cplus/helpers/pointer.h \
  pesdk/include/log4cplus/thread/syncprims.h \
  pesdk/include/log4cplus/spi/filter.h \
  pesdk/include/log4cplus/configurator.h \
  pesdk/include/log4cplus/appender.h \
  pesdk/include/log4cplus/logger.h \
  pesdk/include/log4cplus/spi/appenderattachable.h \
  pesdk/include/log4cplus/spi/loggerfactory.h \
  pesdk/include/log4cplus/helpers/property.h \
  pesdk/include/log4cplus/consoleappender.h \
  pesdk/include/log4cplus/fileappender.h \
  pesdk/include/log4cplus/fstreams.h \
  pesdk/include/log4cplus/helpers/timehelper.h \
  pesdk/include/log4cplus/helpers/lockfile.h \
  pesdk/include/log4cplus/helpers/loglog.h \
  pesdk/include/log4cplus/ndc.h \
  pesdk/include/log4cplus/syslogappender.h \
  pesdk/include/processelement/task.h \
  pesdk/include/common/counters.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/condition.h \
  pesdk/include/common/cglimit_utils.h \
  pesdk/include/common/dstream.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/type.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/serializer.h \
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
  pesdk/include/common/proto/pm_client_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pm.pb.h \
  pesdk/include/common/proto/pm_pn_rpc.pb.h \
  pesdk/include/common/proto/pn.pb.h \
  pesdk/include/common/proto/pub_sub.pb.h \
  pesdk/include/common/proto_rpc.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/common/rpc.h \
  pesdk/include/zeromq/zmq.h \
  pesdk/include/transporter/transporter_common.h \
  pesdk/include/common/dstream.h \
  pesdk/include/common/error.h \
  pesdk/include/transporter/transporter_stubmgr.h \
  pesdk/include/common/mutex.h \
  pesdk/include/transporter/transporter_protocol.h \
  pesdk/include/transporter/transporter_common.h \
  pesdk/include/transporter/transporter.h \
  pesdk/include/common/id.h \
  pesdk/include/transporter/transporter_counters.h \
  pesdk/include/transporter/transporter_tag.h \
  pesdk/include/common/config_const.h \
  pesdk/include/transporter/transporter_inflow.h \
  pesdk/include/transporter/transporter_queue.h \
  pesdk/include/common/mem_mgr.h \
  pesdk/include/common/error.h \
  pesdk/include/common/rwlock.h \
  pesdk/include/transporter/queue_buffer.h \
  pesdk/include/common/logger.h \
  pesdk/include/transporter/ring_buffer.h \
  pesdk/include/transporter/transporter_outflow.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/common/proto/application.pb.h \
  pesdk/include/common/bounded_blocking_queue.h \
  pesdk/include/common/condition.h \
  pesdk/include/transporter/transporter_protocol.h \
  pesdk/include/processelement/bp_progress.h \
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/metric.h \
  pesdk/include/processelement/pe_transporter.h \
  pesdk/include/transporter/transporter_cli.h \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_echo_pe.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o echo_pe_echo_pe.o echo_pe.cc

echo_pe_echo.pb.o:echo.pb.cc \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_echo.pb.o[0m']"
	$(CXX) -c $(INCPATH) $(DEP_INCPATH) $(CPPFLAGS) $(CXXFLAGS)  -o echo_pe_echo.pb.o echo.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


