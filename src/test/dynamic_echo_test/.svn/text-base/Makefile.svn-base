#COMAKE2 edit-mode: -*- Makefile -*-
####################64Bit Mode####################
ifeq ($(shell uname -m),x86_64)
.PHONY:all
all:comake2_makefile_check echo_pe_task echo_exporter echo_importer_add echo_bp_importer_add echo_tag_importer_add 
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mall[0m']"
	@echo "make all done"

.PHONY:comake2_makefile_check
comake2_makefile_check:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mcomake2_makefile_check[0m']"
	@if [ "COMAKE" -nt "Makefile" ] ; then echo "[ERROR]PLEASE UPDATE 'Makefile' BY 'comake2'" ; exit 250 ;  fi

.PHONY:clean
clean:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mclean[0m']"
	rm -rf echo_pe_task
	rm -rf ./output/bin/echo_pe_task
	rm -rf echo_exporter
	rm -rf ./output/bin/echo_exporter
	rm -rf echo_importer_add
	rm -rf ./output/bin/echo_importer_add
	rm -rf echo_bp_importer_add
	rm -rf ./output/bin/echo_bp_importer_add
	rm -rf echo_tag_importer_add
	rm -rf ./output/bin/echo_tag_importer_add
	rm -rf echo_pe_task_echo_pe_task.o
	rm -rf echo_pe_task_echo.pb.o
	rm -rf echo_exporter_echo_pe_exporter.o
	rm -rf echo_exporter_echo.pb.o
	rm -rf echo_importer_add_echo_pe_importer_add.o
	rm -rf echo_importer_add_dynamic.pb.o
	rm -rf echo_bp_importer_add_echo_bp_importer_add.o
	rm -rf echo_bp_importer_add_dynamic.pb.o
	rm -rf echo_tag_importer_add_echo_tag_importer_add.o
	rm -rf echo_tag_importer_add_dynamic_tag.pb.o

.PHONY:dist
dist:clean
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mdist[0m']"
	cd ..;tar czvf dynamic_echo_test.tar.gz dynamic_echo_test;cd dynamic_echo_test

.PHONY:love
love:
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mlove[0m']"
	@echo "make love done"

echo_pe_task:echo_pe_task_echo_pe_task.o \
  echo_pe_task_echo.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_task[0m']"
	g++ echo_pe_task_echo_pe_task.o \
  echo_pe_task_echo.pb.o -Xlinker "-("  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
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
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -lprotobuf \
  -lzookeeper_mt \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lbigpipe_api \
  -lpe -Xlinker "-)" -o echo_pe_task
	mkdir -p ./output/bin
	cp -f --link echo_pe_task ./output/bin

echo_exporter:echo_exporter_echo_pe_exporter.o \
  echo_exporter_echo.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_exporter[0m']"
	g++ echo_exporter_echo_pe_exporter.o \
  echo_exporter_echo.pb.o -Xlinker "-("  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
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
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -lprotobuf \
  -lzookeeper_mt \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lbigpipe_api \
  -lpe -Xlinker "-)" -o echo_exporter
	mkdir -p ./output/bin
	cp -f --link echo_exporter ./output/bin

echo_importer_add:echo_importer_add_echo_pe_importer_add.o \
  echo_importer_add_dynamic.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_importer_add[0m']"
	g++ echo_importer_add_echo_pe_importer_add.o \
  echo_importer_add_dynamic.pb.o -Xlinker "-("  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
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
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -lprotobuf \
  -lzookeeper_mt \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lbigpipe_api \
  -lpe -Xlinker "-)" -o echo_importer_add
	mkdir -p ./output/bin
	cp -f --link echo_importer_add ./output/bin

echo_bp_importer_add:echo_bp_importer_add_echo_bp_importer_add.o \
  echo_bp_importer_add_dynamic.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_bp_importer_add[0m']"
	g++ echo_bp_importer_add_echo_bp_importer_add.o \
  echo_bp_importer_add_dynamic.pb.o -Xlinker "-("  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
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
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -lprotobuf \
  -lzookeeper_mt \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lbigpipe_api \
  -lpe -Xlinker "-)" -o echo_bp_importer_add
	mkdir -p ./output/bin
	cp -f --link echo_bp_importer_add ./output/bin

echo_tag_importer_add:echo_tag_importer_add_echo_tag_importer_add.o \
  echo_tag_importer_add_dynamic_tag.pb.o
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_tag_importer_add[0m']"
	g++ echo_tag_importer_add_echo_tag_importer_add.o \
  echo_tag_importer_add_dynamic_tag.pb.o -Xlinker "-("  ../../../../../../com/idlcompiler/astyle/libastyle.a \
  ../../../../../../com/idlcompiler/cxx/libskeleton.a \
  ../../../../../../com/idlcompiler/parser/libparser.a \
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
  ../../../../../../lib2-64/ullib/lib/libullib.a \
  ../../../../../../public/idlcompiler/output/lib/libmcpack_idl.a \
  ../../../../../../public/mcpack/libmcpack.a -L./pesdk/lib/ \
  -lzmq \
  -ltinyxml \
  -lprotobuf \
  -lzookeeper_mt \
  -lpthread \
  -lcrypto \
  -lrt \
  -luuid \
  -lbigpipe_api \
  -lpe -Xlinker "-)" -o echo_tag_importer_add
	mkdir -p ./output/bin
	cp -f --link echo_tag_importer_add ./output/bin

echo_pe_task_echo_pe_task.o:echo_pe_task.cc \
  echo_pe_task.cc \
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
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/compiler/importer.h \
  pesdk/include/protobuf/google/protobuf/descriptor_database.h \
  pesdk/include/protobuf/google/protobuf/compiler/parser.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/io/tokenizer.h \
  pesdk/include/protobuf/google/protobuf/dynamic_message.h \
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
  pesdk/include/common/utils.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/bigpipe/bigpipe_wrapper.h \
  pesdk/include/bigpipe/common_return_type.h \
  pesdk/include/bigpipe/bigpipe_partitioner.h \
  pesdk/include/bigpipe/bigpipe_progress.h \
  pesdk/include/processelement/checkpoint.h \
  pesdk/include/processelement/subscribe.h \
  pesdk/include/processelement/status_table.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/linked_list_queue.h \
  pesdk/include/processelement/metric.h \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_task_echo_pe_task.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_pe_task_echo_pe_task.o echo_pe_task.cc

echo_pe_task_echo.pb.o:echo.pb.cc \
  echo.pb.cc \
  echo.pb.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/stubs/once.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite_inl.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/reflection_ops.h \
  pesdk/include/protobuf/google/protobuf/wire_format.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_pe_task_echo.pb.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_pe_task_echo.pb.o echo.pb.cc

echo_exporter_echo_pe_exporter.o:echo_pe_exporter.cc \
  echo_pe_exporter.cc \
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
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/compiler/importer.h \
  pesdk/include/protobuf/google/protobuf/descriptor_database.h \
  pesdk/include/protobuf/google/protobuf/compiler/parser.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/io/tokenizer.h \
  pesdk/include/protobuf/google/protobuf/dynamic_message.h \
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
  pesdk/include/common/utils.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/bigpipe/bigpipe_wrapper.h \
  pesdk/include/bigpipe/common_return_type.h \
  pesdk/include/bigpipe/bigpipe_partitioner.h \
  pesdk/include/bigpipe/bigpipe_progress.h \
  pesdk/include/processelement/checkpoint.h \
  pesdk/include/processelement/subscribe.h \
  pesdk/include/processelement/status_table.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/linked_list_queue.h \
  pesdk/include/processelement/metric.h \
  echo.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_exporter_echo_pe_exporter.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_exporter_echo_pe_exporter.o echo_pe_exporter.cc

echo_exporter_echo.pb.o:echo.pb.cc \
  echo.pb.cc \
  echo.pb.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/stubs/once.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite_inl.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/reflection_ops.h \
  pesdk/include/protobuf/google/protobuf/wire_format.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_exporter_echo.pb.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_exporter_echo.pb.o echo.pb.cc

echo_importer_add_echo_pe_importer_add.o:echo_pe_importer_add.cc \
  echo_pe_importer_add.cc \
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
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/compiler/importer.h \
  pesdk/include/protobuf/google/protobuf/descriptor_database.h \
  pesdk/include/protobuf/google/protobuf/compiler/parser.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/io/tokenizer.h \
  pesdk/include/protobuf/google/protobuf/dynamic_message.h \
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
  pesdk/include/common/utils.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/bigpipe/bigpipe_wrapper.h \
  pesdk/include/bigpipe/common_return_type.h \
  pesdk/include/bigpipe/bigpipe_partitioner.h \
  pesdk/include/bigpipe/bigpipe_progress.h \
  pesdk/include/processelement/checkpoint.h \
  pesdk/include/processelement/subscribe.h \
  pesdk/include/processelement/status_table.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/linked_list_queue.h \
  pesdk/include/processelement/metric.h \
  dynamic.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_importer_add_echo_pe_importer_add.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_importer_add_echo_pe_importer_add.o echo_pe_importer_add.cc

echo_importer_add_dynamic.pb.o:dynamic.pb.cc \
  dynamic.pb.cc \
  dynamic.pb.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/stubs/once.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite_inl.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/reflection_ops.h \
  pesdk/include/protobuf/google/protobuf/wire_format.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_importer_add_dynamic.pb.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_importer_add_dynamic.pb.o dynamic.pb.cc

echo_bp_importer_add_echo_bp_importer_add.o:echo_bp_importer_add.cc \
  echo_bp_importer_add.cc \
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
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/compiler/importer.h \
  pesdk/include/protobuf/google/protobuf/descriptor_database.h \
  pesdk/include/protobuf/google/protobuf/compiler/parser.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/io/tokenizer.h \
  pesdk/include/protobuf/google/protobuf/dynamic_message.h \
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
  pesdk/include/common/utils.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/bigpipe/bigpipe_wrapper.h \
  pesdk/include/bigpipe/common_return_type.h \
  pesdk/include/bigpipe/bigpipe_partitioner.h \
  pesdk/include/bigpipe/bigpipe_progress.h \
  pesdk/include/processelement/checkpoint.h \
  pesdk/include/processelement/subscribe.h \
  pesdk/include/processelement/status_table.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/linked_list_queue.h \
  pesdk/include/processelement/metric.h \
  dynamic.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_bp_importer_add_echo_bp_importer_add.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_bp_importer_add_echo_bp_importer_add.o echo_bp_importer_add.cc

echo_bp_importer_add_dynamic.pb.o:dynamic.pb.cc \
  dynamic.pb.cc \
  dynamic.pb.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/stubs/once.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite_inl.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/reflection_ops.h \
  pesdk/include/protobuf/google/protobuf/wire_format.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_bp_importer_add_dynamic.pb.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_bp_importer_add_dynamic.pb.o dynamic.pb.cc

echo_tag_importer_add_echo_tag_importer_add.o:echo_tag_importer_add.cc \
  echo_tag_importer_add.cc \
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
  pesdk/include/common/mutex.h \
  pesdk/include/processelement/macros.h \
  pesdk/include/processelement/tuple.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/common/serializer.h \
  pesdk/include/common/logger.h \
  pesdk/include/common/dynamic_type.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/compiler/importer.h \
  pesdk/include/protobuf/google/protobuf/descriptor_database.h \
  pesdk/include/protobuf/google/protobuf/compiler/parser.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/io/tokenizer.h \
  pesdk/include/protobuf/google/protobuf/dynamic_message.h \
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
  pesdk/include/common/utils.h \
  pesdk/include/common/mutex.h \
  pesdk/include/common/proto_serializer.h \
  pesdk/include/processelement/context.h \
  pesdk/include/common/commit_log.h \
  pesdk/include/bigpipe/bigpipe_wrapper.h \
  pesdk/include/bigpipe/common_return_type.h \
  pesdk/include/bigpipe/bigpipe_partitioner.h \
  pesdk/include/bigpipe/bigpipe_progress.h \
  pesdk/include/processelement/checkpoint.h \
  pesdk/include/processelement/subscribe.h \
  pesdk/include/processelement/status_table.h \
  pesdk/include/common/utils.h \
  pesdk/include/common/byte_array.h \
  pesdk/include/common/linked_list_queue.h \
  pesdk/include/processelement/metric.h \
  dynamic_tag.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_tag_importer_add_echo_tag_importer_add.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_tag_importer_add_echo_tag_importer_add.o echo_tag_importer_add.cc

echo_tag_importer_add_dynamic_tag.pb.o:dynamic_tag.pb.cc \
  dynamic_tag.pb.cc \
  dynamic_tag.pb.h \
  pesdk/include/protobuf/google/protobuf/stubs/common.h \
  pesdk/include/protobuf/google/protobuf/generated_message_util.h \
  pesdk/include/protobuf/google/protobuf/repeated_field.h \
  pesdk/include/protobuf/google/protobuf/message_lite.h \
  pesdk/include/protobuf/google/protobuf/io/coded_stream.h \
  pesdk/include/protobuf/google/protobuf/extension_set.h \
  pesdk/include/protobuf/google/protobuf/generated_message_reflection.h \
  pesdk/include/protobuf/google/protobuf/message.h \
  pesdk/include/protobuf/google/protobuf/unknown_field_set.h \
  pesdk/include/protobuf/google/protobuf/stubs/once.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite_inl.h \
  pesdk/include/protobuf/google/protobuf/wire_format_lite.h \
  pesdk/include/protobuf/google/protobuf/descriptor.h \
  pesdk/include/protobuf/google/protobuf/reflection_ops.h \
  pesdk/include/protobuf/google/protobuf/wire_format.h \
  pesdk/include/protobuf/google/protobuf/descriptor.pb.h
	@echo "[[1;32;40mCOMAKE:BUILD[0m][Target:'[1;32;40mecho_tag_importer_add_dynamic_tag.pb.o[0m']"
	g++ -c -I. \
  -I./pesdk/include/ \
  -I./pesdk/include/protobuf \
  -I./pesdk/include/zeromq \
  -I./pesdk/include/bigpipe -I../../../../../../com/idlcompiler \
  -I../../../../../../com/idlcompiler/include \
  -I../../../../../../com/idlcompiler/output \
  -I../../../../../../com/idlcompiler/output/include \
  -I../../../../../../lib2-64/bsl \
  -I../../../../../../lib2-64/bsl/include \
  -I../../../../../../lib2-64/bsl/output \
  -I../../../../../../lib2-64/bsl/output/include \
  -I../../../../../../lib2-64/ullib \
  -I../../../../../../lib2-64/ullib/include \
  -I../../../../../../lib2-64/ullib/output \
  -I../../../../../../lib2-64/ullib/output/include \
  -I../../../../../../public/idlcompiler \
  -I../../../../../../public/idlcompiler/include \
  -I../../../../../../public/idlcompiler/output \
  -I../../../../../../public/idlcompiler/output/include \
  -I../../../../../../public/mcpack \
  -I../../../../../../public/mcpack/include \
  -I../../../../../../public/mcpack/output \
  -I../../../../../../public/mcpack/output/include -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
  -DVERSION=\"1.0.1.0\" \
  -D_FILE_OFFSET_BITS=64 -g \
  -pipe \
  -W \
  -Wall \
  -fPIC  -o echo_tag_importer_add_dynamic_tag.pb.o dynamic_tag.pb.cc

endif #ifeq ($(shell uname -m),x86_64)


