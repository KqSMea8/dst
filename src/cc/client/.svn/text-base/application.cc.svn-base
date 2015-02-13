/**
* @file   application.cc
* @brief    dstream application class.
* @author   konghui, konghui@baidu.com
* @version
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <boost/algorithm/string.hpp>
#include <deque>
#include <string>

#include "client/application.h"
#include "client/dstream_client.h"
#include "common/config_const.h"
#include "common/dstream.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/utils.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace  client {

static int32_t GetStrCfgIterm(const config::Config& cfg,
                              const char* str_name,
                              bool required,
                              std::string* str) {
    assert(NULL != str);
    *str = cfg.GetValue(str_name);
    if (str->empty()) {
        if (required) {
            DSTREAM_DEBUG("fail to get required iterm:[%s]", str_name);
        } else {
            DSTREAM_DEBUG("optional iterm:[%s] not configured", str_name);
        }
        return error::CONFIG_BAD_ARGUMENT;
    }
    return error::OK;
}

static int32_t GetStrCfgIterm(const config::ConfigNode& node,
                              const char* str_name,
                              bool required ,
                              std::string* str) {
    assert(NULL != str);
    *str = node.GetValue(str_name);
    if (str->empty()) {
        if (required) {
            DSTREAM_DEBUG("fail to get required iterm:[%s]", str_name);
        } else {
            DSTREAM_DEBUG("optional iterm:[%s] not configured", str_name);
        }
        return error::CONFIG_BAD_ARGUMENT;
    }
    return error::OK;
}

/* for debug */
static void DumpProcessor(const Topology& tp) {
    printf("Get [%d] processors:\n", tp.processors_size());
    for (int32_t i = 0; i < tp.processors_size(); i++) {
        printf("---[%s]\n", tp.processors(i).name().c_str());
        printf("   ---ID:[%lu]\n   ---Role:[%d]\n",
               tp.processors(i).id().id(),
               tp.processors(i).role());
        printf("   ---Cmd: [%s]\n", tp.processors(i).exec_cmd().c_str());
        printf("   ---Parallelism: [%u]\n", tp.processors(i).parallism());
        printf("   ---Resource: CPU:[%f], MEM:[%lu]\n",
               tp.processors(i).resource_require().cpu(),
               tp.processors(i).resource_require().memory());
        for (int32_t j = 0; j < tp.processors(i).subscribe().bigpipe_size(); j++) {
            printf("   ---subscribe: bigpipe:[%s]\n",
                   tp.processors(i).subscribe().bigpipe(j).name().c_str());
        }
        /*for (int32_t j = 0; j < tp.processors(i).subscribe().hdfs_size(); j++) {
          printf("   ---subscribe: hdfs:[%s]\n",
                  tp.processors(i).subscribe().hdfs(j).path().c_str());
        }*/
        for (int32_t j = 0; j < tp.processors(i).subscribe().processor_size(); j++) {
            printf("   ---subscribe: processor:[%s], \ttag:[%s]\n",
                   tp.processors(i).subscribe().processor(j).name().c_str(),
                   tp.processors(i).subscribe().processor(j).tags().c_str());
        }
        if (tp.processors(i).has_persist_progress()) {
            printf("   ---PersistProgress: [%s]\n", tp.processors(i).persist_progress().c_str());
        }

        if (tp.processors(i).has_publish()) {
            Publish pub = tp.processors(i).publish();

            // dump bigpipe exporter info
            if (pub.type() == config_const::kExporterTypeBigpipe) {
                printf("   ---exporter publish: pipe name:[%s],"
                       " \tpub token:[%s], \tpipelete num:[%u]\n",
                       pub.bigpipe().pipe_name().c_str(),
                       pub.bigpipe().pub_token().c_str(),
                       pub.bigpipe().pipelet_num());

                printf("   ---exporter publish: retry time:[%u],"
                       "\tretry interval:[%u]\n",
                       pub.bigpipe().retry_time(),
                       pub.bigpipe().retry_interval());
            }
        }
        printf("    ---[%s]\n", tp.processors(i).pn_group().c_str());
    }
}

int32_t InitCfg(const std::string& app_dir, bool sub_tree, bool dyn_importer,
                bool update_processor, config::Config* cfg) {
    assert(cfg != NULL);
    std::string file;
    file = app_dir + "/";
    /*  file += sub_tree ? config_const::kSubTreeCfgFileName :
                         config_const::kAppCfgFileName;*/
    if (sub_tree) {
        file += config_const::kSubTreeCfgFileName;
    } else if (dyn_importer) {
        file += config_const::kDynamicImporterCfgFileName;
    } else if (update_processor) {
        file += config_const::kUpdateProcessorCfgFileName;
    } else {
        file += config_const::kAppCfgFileName;
    }
    int32_t ret = cfg->ReadConfig(file);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to read config file[%s]", file.c_str());
        return ret;
    }
    return error::OK;
}

int32_t GetAppName(const std::string& app_dir, bool sub_tree/*=false*/,
                   bool dyn_importer, bool update_processor,
                   std::string* app_name) {
    assert(app_name != NULL);
    std::string iterm;
    config::Config cfg;
    int32_t ret = InitCfg(app_dir, sub_tree, dyn_importer, update_processor, &cfg);
    if (ret < error::OK) {
        return ret;
    }
    return cfg.GetValue(config_const::kAppNameName, app_name);
}

static int32_t ReadExporterPubs(const std::string& cfg_file, Processor* pp) {
    std::string item;
    config::Config cfg;

    int32_t ret = cfg.ReadConfig(cfg_file);
    if (ret < error::OK) {
        DSTREAM_DEBUG("fail to read publish config file[%s] for exporter[%s]",
                      cfg_file.c_str(), pp->name().c_str());
        return ret;
    }

    if (pp->publish().type() == "bigpipe") {
        config::ConfigNode bigpipe_node = cfg.GetNode("BigPipe");
        if (!bigpipe_node.IsErrorNode() && !bigpipe_node.HasNoChildren()) {
            Publish* pub = pp->mutable_publish();

            BigpipePub* bps = pub->mutable_bigpipe();
            if (bps == NULL) {
                DSTREAM_WARN("fail to add bigpipe into exporter publish");
                return error::BAD_MEMORY_ALLOCATION;
            }

            ret = GetStrCfgIterm(bigpipe_node, "PipeName", false, &item);
            if (ret != error::OK) {
                DSTREAM_WARN("config item[PipeName] required");
                return error::CONFIG_BAD_ARGUMENT;
            }
            bps->set_pipe_name(item);

            ret = GetStrCfgIterm(bigpipe_node, "PubAuthentication", false, &item);
            if (ret != error::OK) {
                DSTREAM_WARN("config item[PubAuthentication] required");
                return error::CONFIG_BAD_ARGUMENT;
            }
            bps->set_pub_user(item);

            ret = GetStrCfgIterm(bigpipe_node, "PubToken", false, &item);
            if (ret != error::OK) {
                DSTREAM_WARN("config item[PubToken] required");
                return error::CONFIG_BAD_ARGUMENT;
            }
            bps->set_pub_token(item);

            int32_t pipelet_num = 0;
            item = bigpipe_node.GetValue("PipeletNum");
            if (item.empty() || (pipelet_num = atoi(item.c_str())) <= 0) {
                DSTREAM_WARN("config item[PipeletNum] required");
                return error::CONFIG_BAD_ARGUMENT;
            }
            bps->set_pipelet_num((unsigned)pipelet_num);

            // set default fail over config
            bps->set_retry_time(3);
            bps->set_retry_interval(50); // micro seconds

            // get failover config
            config::ConfigNode failover_node = cfg.GetNode("Failover");
            if (!failover_node.IsErrorNode() && !failover_node.HasNoChildren()) {
                ret = GetStrCfgIterm(failover_node, "RetryTime", false, &item);
                if (ret == error::OK) {
                    bps->set_retry_time(atoi(item.c_str()));
                }

                ret = GetStrCfgIterm(failover_node, "RetryInterval", false, &item);
                if (ret == error::OK) {
                    bps->set_retry_interval(atoi(item.c_str()));
                }
            }
        }
    }
    return error::OK;
}

static int32_t ReadImporterSubs(const std::string& cfg_file, Processor* pp) {
    std::string iterm;
    config::Config cfg;

    int32_t ret = cfg.ReadConfig(cfg_file);
    if (ret < error::OK) {
        DSTREAM_DEBUG("fail to read subscribe cfg file[%s] for importer[%s]",
                      cfg_file.c_str(), pp->name().c_str());
        return ret;
    }

    Subscribe* sub = pp->mutable_subscribe();
    config::ConfigNode::NodeList child_list;
    config::ConfigNode::NodeList::iterator it;

    /* read fake */
    config::ConfigNode fake_node = cfg.GetNode("Fake");
    if (!fake_node.IsErrorNode()) {
        FakeSub* s = sub->add_fake();
        if (NULL == s) {
            DSTREAM_WARN("fail to add fake into subscribes for importer[%s]",
                         pp->name().c_str());
            return error::BAD_MEMORY_ALLOCATION;
        }
    }

    /* read local file */
    config::ConfigNode localfile_node = cfg.GetNode("LocalFile");
    if (!localfile_node.IsErrorNode() && !localfile_node.HasNoChildren()) {
        LocalSub* s = sub->add_localfile();
        if (NULL == s) {
            DSTREAM_WARN("fail to add localfile into subscribes for importer[%s]",
                         pp->name().c_str());
            return error::BAD_MEMORY_ALLOCATION;
        }
        ret = GetStrCfgIterm(localfile_node, "FileName", false, &iterm);
        if (ret == error::OK && !iterm.empty()) {
            s->set_path(iterm);
        } else {
            DSTREAM_WARN("fail to add localfile path for importer[%s]",
                         pp->name().c_str());
            return error::CONFIG_BAD_ARGUMENT;
        }

        ret = GetStrCfgIterm(localfile_node, "SubPoint", false, &iterm);
        if (ret == error::OK && !iterm.empty()) {
            DSTREAM_DEBUG("set SubPoint to (%s) for importer[%s]",
                          iterm.c_str(), pp->name().c_str());
            s->set_sub_point(strtoull(iterm.c_str(), NULL, 10));
        }
    }

    /* read scribe */
    config::ConfigNode scribe_node = cfg.GetNode("Scribe");
    if (!scribe_node.IsErrorNode() && !scribe_node.HasNoChildren()) {
        ScribeSub* s = sub->add_scribe();
        if (NULL == s) {
            DSTREAM_WARN("fail to add scribe into subscribes for importer[%s]",
                         pp->name().c_str());
            return error::BAD_MEMORY_ALLOCATION;
        }
        ret = GetStrCfgIterm(scribe_node, "BufferSize", false, &iterm);
        if (ret == error::OK && !iterm.empty()) {
            DSTREAM_DEBUG("set BufferSize to (%s) for importer[%s]",
                          iterm.c_str(), pp->name().c_str());
            s->set_buffersize(strtoull(iterm.c_str(), NULL, 10));
        }
    }

    /* read bigpipe */
    config::ConfigNode bigpipe_node = cfg.GetNode("BigPipe");
    if (!bigpipe_node.IsErrorNode() && !bigpipe_node.HasNoChildren()) {
        child_list = bigpipe_node.GetChildrenList();
        for (it = child_list.begin(); it != child_list.end(); it++) {
            BigpipeSub* s = sub->add_bigpipe();
            if (NULL == s) {
                DSTREAM_WARN("fail to add bigpipe into subscribes for importer[%s]",
                             pp->name().c_str());
                return error::BAD_MEMORY_ALLOCATION;
            }
            s->set_name(it->GetName());
            if (s->name().empty()) {
                DSTREAM_WARN("fail to add bigpipe name for importer[%s]",
                             pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }

            // read username
            ret = GetStrCfgIterm(*it, "Authentication", false, &iterm);
            if (ret < error::OK || iterm.empty()) {
                DSTREAM_ERROR("bigpipe username not configured for importer[%s]",
                              pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
            s->set_username(iterm);

            // read password
            ret = GetStrCfgIterm(*it, "Token", false, &iterm);
            if (ret < error::OK || iterm.empty()) {
                DSTREAM_WARN("bigpipe token not configured for importer[%s],"
                             "will set to '_default_token_'",
                             pp->name().c_str());
                iterm = "_default_token_";
            }
            s->set_password(iterm);
            // modify by lanbijia@baidu.com
            // set unused pipelet item
            s->set_pipelet("0");
            // set pipelet id list
            ret = GetStrCfgIterm(*it, "Pipelet", false, &iterm);
            if (ret < error::OK || iterm.empty()) {
                DSTREAM_WARN("pipelet not configured for importer[%s], will set 0",
                             pp->name().c_str());
                iterm = "0";
            }
            // parse pipelet id, & add to pipelet id list
            std::vector<int> num_list;
            ret = application_tools::ParseStringToNumberList(iterm, &num_list);
            if (ret < error::OK || num_list.size() <= 0) {
                DSTREAM_ERROR("Pipelet id list parse failed for importer[%s]",
                              pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
            for (std::vector<int>::iterator num_it = num_list.begin();
                 num_it != num_list.end(); num_it++) {
                s->add_pipelet_id(*num_it);
            }
            // set total pipelet number
            ret = GetStrCfgIterm(*it, "PipeletNum", false, &iterm);
            if (ret == error::OK && !iterm.empty()) {
                s->set_pipelet_num(strtol(iterm.c_str(), NULL, 10));
            } else {
                DSTREAM_INFO("PipeletNum not configured for importer[%s], will set 1",
                             pp->name().c_str());
                iterm = "1";
                s->set_pipelet_num(strtol(iterm.c_str(), NULL, 10));
            }

            ret = GetStrCfgIterm(*it, "SubPoint", false, &iterm);
            if (ret == error::OK && !iterm.empty()) {
                DSTREAM_DEBUG("set SubPoint to (%s) for importer[%s]",
                              iterm.c_str(), pp->name().c_str());
                s->set_sub_point(strtoull(iterm.c_str(), NULL, 10));
            }
        }
    }

    /* read hdfs */
    config::ConfigNode hdfs_node = cfg.GetNode("Hdfs");
    if (!hdfs_node.IsErrorNode() && !hdfs_node.HasNoChildren()) {
        child_list = hdfs_node.GetChildrenList();
        for (it = child_list.begin(); it != child_list.end(); it++) {
            HdfsSub* h = sub->add_hdfs();
            if (NULL == h) {
                DSTREAM_WARN("fail to add hdfs into subscribes for importer[%s]",
                             pp->name().c_str());
                return error::CLIENT_INIT_TP_FAIL;
            }
            ret = GetStrCfgIterm(*it, "HdfsUri", false, &iterm);
            h->set_path(iterm);
            if (h->path().empty()) {
                DSTREAM_WARN("fail to get hdfs path for importer[%s]",
                             pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
        }
    }

    /* read processor */
    config::ConfigNode pro_node = cfg.GetNode("Processor");
    if (!pro_node.IsErrorNode() && !pro_node.HasNoChildren()) {
        child_list = pro_node.GetChildrenList();
        for (it = child_list.begin(); it != child_list.end(); it++) {
            ProcessorSub* p = sub->add_processor();
            if (NULL == p) {
                DSTREAM_WARN("fail to add processor into subscribes for importer[%s]",
                             pp->name().c_str());
                return error::CLIENT_INIT_TP_FAIL;
            }
            p->set_name(it->GetName());
            if (p->name().empty()) {
                DSTREAM_WARN("fail to get processor path for importer[%s]",
                             pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
            ret = GetStrCfgIterm(*it, "tag", false, &iterm);
            p->set_tags(iterm);
        }
    }

    if (sub->bigpipe_size() == 0 &&
        sub->hdfs_size() == 0 &&
        sub->processor_size() == 0 &&
        sub->localfile_size() == 0 &&
        sub->scribe_size() == 0 &&
        sub->fake_size() == 0) {
        DSTREAM_WARN("no subscribe configured for importer:[%s]", pp->name().c_str());
        return error::CONFIG_BAD_ARGUMENT;
    }

    return error::OK;
}

static int32_t ReadProcessorSubs(const config::ConfigNode& node, Processor* pp) {
    int32_t ret = error::OK;
    std::string iterm;
    config::ConfigNode::NodeList child_list;
    config::ConfigNode::NodeList::iterator it;
    config::ConfigNode sub_node = node.GetNode("Subscribe");
    if (sub_node.IsErrorNode() || sub_node.HasNoChildren()) {
        DSTREAM_WARN("fail to get processor[%s]' subscribe: not node or bad config",
                     pp->name().c_str());
        return error::CONFIG_BAD_ARGUMENT;
    }
    Subscribe* sub = pp->mutable_subscribe();
    child_list = sub_node.GetChildrenList();
    for (it = child_list.begin(); it != child_list.end(); it++) {
        ProcessorSub* p = sub->add_processor();
        if (NULL == p) {
            DSTREAM_WARN("fail to add processor[%s]' subscribe: bad memory "
                         "allocation", pp->name().c_str());
            return error::CLIENT_INIT_TP_FAIL;
        }
        ret = GetStrCfgIterm(*it, "processor", true , &iterm);
        if (ret < error::OK || iterm.empty()) {
            DSTREAM_WARN("fail to get processor[%s]' subscribe: no upstream "
                         "processors", pp->name().c_str());
            return error::CONFIG_BAD_ARGUMENT;
        }
        p->set_name(iterm);
        GetStrCfgIterm(*it, "tag", false, &iterm);
        p->set_tags(iterm);
    }
    return error::OK;
}

// add by lanbijia@baidu.com
// for read publish tag from application config
static int32_t ReadProcessorPubs(const config::ConfigNode& node, Processor* pp) {
    int32_t ret = error::OK;
    std::string iterm;
    config::ConfigNode::NodeList child_list;
    config::ConfigNode::NodeList::iterator it;

    config::ConfigNode pub_node = node.GetNode("Publish");
    if (pub_node.IsErrorNode() || pub_node.HasNoChildren()) {
        DSTREAM_DEBUG("optional iterm:[%s] not configured", "Publish");
        return error::CONFIG_BAD_ARGUMENT;
    }

    child_list = pub_node.GetChildrenList();
    Publish* pub = pp->mutable_publish();
    for (it = child_list.begin(); it != child_list.end(); it++) {
        GetStrCfgIterm(*it, "tag", false, &iterm);
        if (!iterm.empty()) {
            pub->add_tags(iterm);
        }
    }

    return ret;
}


static int32_t ReadProcessors(const std::string& app_dir,
                              config::ConfigNode& node,
                              Topology* tp,
                              ProcessorRole role,
                              bool check_processor_dir/*=true*/) {
    int32_t ret = error::OK;
    std::string iterm;
    config::ConfigNode::NodeList child_list;
    child_list = node.GetChildrenList();
    config::ConfigNode::NodeList::iterator it;
    static int64_t pid = 0;
    for (it = child_list.begin(); it != child_list.end(); it++, pid++) {
        Processor* pp = tp->add_processors();
        if (NULL == pp) {
            DSTREAM_WARN("fail to add processor to topology: bad memory allocation");
            return error::CLIENT_INIT_TP_FAIL;
        }
        pp->set_role(role);
        pp->mutable_id()->set_id(pid);

        ret = GetStrCfgIterm(*it, "ProcessorName", true, &iterm);
        if (ret < error::OK || iterm.empty()) {
            return error::CONFIG_BAD_ARGUMENT;
        }
        pp->set_name(iterm);
        if (check_processor_dir) {
            /* check processor's dir */
            iterm = app_dir;
            iterm.append("/").append(pp->name());
            if (!IsDir(iterm.c_str())) {
                DSTREAM_WARN("processor dir:[%s] does not exist", iterm.c_str());
                return error::FILE_OPERATION_ERROR;
            }
            // check the bins and other files
            if (!HasExecutedFiles(iterm.c_str())) {
                DSTREAM_WARN("processor dir:[%s] does not have executed files", iterm.c_str());
                return error::FILE_OPERATION_ERROR;
            }
        }

        ret = GetStrCfgIterm(*it, "Description", false, &iterm);
        pp->set_descr(iterm);
        ret = GetStrCfgIterm(*it, "RunCmd", true, &iterm);
        if (ret < error::OK || iterm.empty()) {
            return error::CONFIG_BAD_ARGUMENT;
        }
        pp->set_exec_cmd(iterm);

        // add by lanbijia@baidu.com
        // read publish info from config
        if (role != EXPORTER) {
            ret = ReadProcessorPubs(*it, pp);
        }

        if (role == IMPORTER) {
            // read persist progress option
            std::string persist_progress("false");
            ret = GetStrCfgIterm(*it, "PersistProgress", false, &iterm);
            if (ret == error::OK && iterm == "true") {
                persist_progress = "true";
            }
            pp->set_persist_progress(persist_progress);

            if (check_processor_dir) {
                std::string cfg_file = app_dir + "/" + pp->name() + "/" + pp->name() + "_sub.xml";
                ret = GetStrCfgIterm(*it, "Subscribe", false, &iterm);
                if (ret == error::OK && !iterm.empty()) {
                    if ('/' == iterm[0]) { // abs path
                        cfg_file = iterm;
                    } else {
                        cfg_file = app_dir + "/" + pp->name() + "/" + iterm;
                    }
                }
                ret = ReadImporterSubs(cfg_file, pp);
            } else {
                ret = error::OK;
            }

        } else if (role == EXPORTER) { // read down stream publish config
            ret = GetStrCfgIterm(*it,  "ExporterType", false, &iterm);
            if (ret == error::OK && iterm == "bigpipe") {
                pp->mutable_publish()->set_type("bigpipe");

                std::string cfg_file = app_dir + "/" + pp->name() + "/" + pp->name() + "_pub.xml";
                ret = ReadExporterPubs(cfg_file, pp);
                if (ret != error::OK) {
                    return ret;
                }
            }
            config::ConfigNode::NodeList sub_list;
            sub_list = it->GetChildrenList("Subscribe");
            if (sub_list.size() > 1) {
                DSTREAM_WARN("processor [%s] has multiple subscribe iterms", pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
            ret = ReadProcessorSubs(*it, pp);
        } else {
            config::ConfigNode::NodeList sub_list;
            sub_list = it->GetChildrenList("Subscribe");
            if (sub_list.size() > 1) {
                DSTREAM_WARN("processor [%s] has multiple subscribe iterms", pp->name().c_str());
                return error::CONFIG_BAD_ARGUMENT;
            }
            ret = ReadProcessorSubs(*it, pp);
        }
        if (ret < error::OK) {
            return ret;
        }

        ret = GetStrCfgIterm(*it, "Parallelism", true, &iterm);
        if (ret < error::OK || iterm.empty()) {
            return error::CONFIG_BAD_ARGUMENT;
        }
        uint32_t tmp_parallism = static_cast<uint32_t>(strtol(iterm.c_str(), NULL, 10));
        pp->set_parallism(tmp_parallism);
        // remove backup pe num from PM
        pp->set_backup_num(1);
        ret = GetStrCfgIterm(*it, "Resource.CPU", true, &iterm);
        if (ret < error::OK || iterm.empty()) {
            return error::CONFIG_BAD_ARGUMENT;
        }
        pp->mutable_resource_require()->set_cpu(strtof(iterm.c_str(), NULL));
        ret = GetStrCfgIterm(*it, "Resource.Memory", true, &iterm);
        if (ret < error::OK || iterm.empty()) {
            return error::CONFIG_BAD_ARGUMENT;
        }
        pp->mutable_resource_require()->set_memory(GetConfigMemory(&iterm));

        if (role == IMPORTER) {
            (tp->add_importer_id())->set_id(pid);
        } else if (role == EXPORTER) {
            (tp->add_exporter_id())->set_id(pid);
        }

        ret = GetStrCfgIterm(*it, "DynamicProtoDir", false, &iterm);
        if (ret < error::OK) {
            iterm = ".";
        }
        pp->set_dynamic_proto_dir(iterm);

        // add by fangjun02@baidu.com
        // for transporter config
        ret = GetStrCfgIterm(*it, config_const::kProcessorBatchCountName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_tuple_batch_count(strtol(iterm.c_str(), NULL, 10));
        }

        ret = GetStrCfgIterm(*it, config_const::kProcessorBatchSizeName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_tuple_batch_size(strtol(iterm.c_str(), NULL, 10));
        }

        ret = GetStrCfgIterm(*it, config_const::kProcessorForceForwardMsName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_force_forward_tuples_period_ms(strtol(iterm.c_str(), NULL, 10));
        }

        pp->set_send_queue_size(config_const::kPNSendQueueSize);
        ret = GetStrCfgIterm(*it, config_const::kProcessorSendQSizeName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_send_queue_size(strtol(iterm.c_str(), NULL, 10));
        }

        pp->set_recv_queue_size(config_const::kPNRecvQueueSize);
        ret = GetStrCfgIterm(*it, config_const::kProcessorRecvQSizeName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_recv_queue_size(strtol(iterm.c_str(), NULL, 10));
        }

        pp->set_send_watermark(config_const::kPNSendWaterMark);
        ret = GetStrCfgIterm(*it, config_const::kProcessorSendQWaterMarkName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_send_watermark(strtof(iterm.c_str(), NULL));
        }

        pp->set_recv_watermark(config_const::kPNRecvWaterMark);
        ret = GetStrCfgIterm(*it, config_const::kProcessorRecvQWaterMarkName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_recv_watermark(strtof(iterm.c_str(), NULL));
        }

        ret = GetStrCfgIterm(*it, config_const::kProcessorFetchTimeoutMsName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_fetch_timeout_ms(strtol(iterm.c_str(), NULL, 10));
        }

        ret = GetStrCfgIterm(*it, config_const::kProcessorSendTimeoutMsName, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->set_send_timeout_ms(strtol(iterm.c_str(), NULL, 10));
        }

        // test queue size, add by konghui, 2013.03.21
        int lap_size = static_cast<int>((pp->recv_queue_size() * (1 - pp->recv_watermark())));
        if (lap_size < transporter::kMaxMsgSize) {
            ret = error::BAD_ARGUMENT;
            DSTREAM_WARN("%s: processor:%s: %s * %s should be great "
                         "than %dMB, which is %dMB", error::ErrStr(ret),
                         pp->name().c_str(),
                         config_const::kProcessorRecvQSizeName,
                         config_const::kProcessorRecvQWaterMarkName,
                         transporter::kMaxMsgSize / (1024 * 1024),
                         lap_size / (1024 * 1024));
            return ret;
        }
        lap_size = static_cast<int>((pp->send_queue_size() * (1 - pp->send_watermark())));
        if (lap_size < transporter::kMaxMsgSize) {
            ret = error::BAD_ARGUMENT;
            DSTREAM_WARN("%s: processor:%s: %s * %s should be great "
                         "than %dMB, which is %dMB", error::ErrStr(ret),
                         pp->name().c_str(),
                         config_const::kProcessorSendQSizeName,
                         config_const::kProcessorSendQWaterMarkName,
                         transporter::kMaxMsgSize / (1024 * 1024),
                         lap_size / (1024 * 1024));
            return ret;
        }

        // add kylin configs
        ret = GetStrCfgIterm(*it, "PNGroup", false, &iterm);
        if (ret < error::OK) {
            iterm = "default";
        }
        pp->set_pn_group(iterm);

        // set default revision
        pp->set_cur_revision(0);
        pp->set_max_revision(0);
        
        // add flow control config, default is -1, not limit
        ret = GetStrCfgIterm(*it, config_const::kQPSLimit, false, &iterm);
        if (ret >= error::OK && !iterm.empty()) {
            pp->mutable_flow_control()->set_qps_limit(strtol(iterm.c_str(), NULL, 10));
        }

    } // end of for

    return error::OK;
}

int32_t InitApp(const std::string& app_dir,
                bool check_processor_dir,
                bool sub_tree/*=false*/,
                bool dyn_importer/*=false*/,
                bool update_processor/*=false*/,
                Application *app) {
    assert(app != NULL);
    std::string iterm;
    config::Config cfg;

    if (!IsDir(app_dir.c_str())) {
        return error::FILE_OPERATION_ERROR;
        DSTREAM_WARN("app dir:[%s] does not exist", app_dir.c_str());
    }

    int32_t ret = error::OK;
    ret = InitCfg(app_dir, sub_tree, dyn_importer, update_processor, &cfg);
    if (ret < error::OK) {
        return ret;
    }

    ret = GetStrCfgIterm(cfg, config_const::kAppNameName, true, &iterm);
    if (ret < error::OK || iterm.empty()) {
        return error::CONFIG_BAD_ARGUMENT;
    }
    if (!is_valid_appname(iterm)) {
        DSTREAM_DEBUG("app name must be made of \"a-zA-Z0-9.@_-\"");
        return error::INVALID_APP_NAME;
    }
    app->set_name(iterm);
    ret = GetStrCfgIterm(cfg, "Description", false, &iterm);
    app->set_descr(iterm);

    Topology* tp = app->mutable_topology();

    ret = GetStrCfgIterm(cfg, config_const::kAppDataStrategyName, false, &iterm);
    if (ret < error::OK) {
        app->set_data_strategy(config_const::kDefaultAppDataStrategy);
    } else {
        if (config_const::kDataStrategyDropAgelong == iterm) {
            app->set_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG);
        } else if (config_const::kDataStrategyHoldData == iterm) {
            app->set_data_strategy(DST_WHEN_QUEUE_FULL_HOLD);
        } else {
            app->set_data_strategy(config_const::kDefaultAppDataStrategy);
        }
    }
    DSTREAM_INFO("Set DataStrategy : %s", DataStrategyTypeToStr(app->data_strategy()));

    ret = GetStrCfgIterm(cfg, "Topology.ProcessorNum", true, &iterm);
    if (ret < error::OK || iterm.empty()) {
        return error::CONFIG_BAD_ARGUMENT;
    }
    tp->set_processor_num(strtol(iterm.c_str(), NULL, 10));

    /* get importer */
    if (!sub_tree) { /* sub tree should not have importer */
        config::ConfigNode node = cfg.GetNode("Topology.Importer");
        if (!node.IsErrorNode() && !node.HasNoChildren()) {
            ret = ReadProcessors(app_dir, node, tp, IMPORTER, check_processor_dir);
            if (ret < error::OK) {
                DSTREAM_WARN("fail to init importers from config file");
                return ret;
            }
        }
    }

    /* get exporter */
    config::ConfigNode node = cfg.GetNode("Topology.Exporter");
    if (!node.IsErrorNode() && !node.HasNoChildren()) {
        if (dyn_importer) {
            ret = ReadProcessors(app_dir, node, tp, EXPORTER, false);
        } else {
            ret = ReadProcessors(app_dir, node, tp, EXPORTER, check_processor_dir);
        }
        if (ret < error::OK) {
            DSTREAM_WARN("fail to init exporters from config file");
            return ret;
        }
    }
    /* get processor */
    node = cfg.GetNode("Topology.Processor");
    if (!node.IsErrorNode() && !node.HasNoChildren()) {
        if (dyn_importer) {
            ret = ReadProcessors(app_dir, node, tp, WORK_PROCESSOR, false);
        } else {
            ret = ReadProcessors(app_dir, node, tp, WORK_PROCESSOR, check_processor_dir);
        }
        if (ret < error::OK) {
            DSTREAM_WARN("fail to init processor from config file");
            return ret;
        }
    }

    /* debug */
    DumpProcessor(*tp);
    return error::OK;
}

/**
* @brief  merge the sub_tree into the old_tree
*
* @param[in]       sub_tree   the sub_tree to be merged into the old_tree
* @param[in][out]  old_tree   the target tree to merge the sub_tree
*
* @return   successed: error::OK;
*           failed:    other error code;
* @author   konghui, konghui@baidu.com
* @date   2012-04-06
*/
int32_t MergeSubTreeIntoApp(const Application& sub_tree, Application* old_tree) {
    DSTREAM_INFO("merger sub tree into app ...");
    assert(NULL != old_tree);

    /* check validity for addiing sub tree into the app topology */
    if (old_tree->name() != sub_tree.name()) {
        DSTREAM_WARN("bad app merge: app name[%s] and sub tree name[%s] are "
                     "surposed to be equal", old_tree->name().c_str(),
                     sub_tree.name().c_str());
        return error::CLIENT_MERGER_TOPO_FAIL;
    }

    /* merge sub tree int app */
    Topology* new_tp = old_tree->mutable_topology();
    if (NULL == new_tp) {
        DSTREAM_WARN("fail to get topology from app[%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }
    // size_t processor_num = new_tp->processor_num();
    const Topology& sub_tree_tp = sub_tree.topology();
    for (uint32_t i = 0; i < sub_tree_tp.processor_num(); i++) {
        Processor* pp = new_tp->add_processors();
        if (NULL == pp) {
            DSTREAM_WARN("fail to allocate processor from app:"
                         "[bad memory allocation]");
            /* may leading memory leaks, but we don't care */
            return error::BAD_MEMORY_ALLOCATION;
        }
        *pp = sub_tree_tp.processors(i);
        /* add exporter id
         * (will not add importer id cause sub tree should not have one) */
        if (EXPORTER == pp->role()) {
            ProcessorID* ppid = new_tp->add_exporter_id();
            if (NULL == ppid) {
                DSTREAM_WARN("fail to add processor id to app:"
                             "[bad memory allocation]");
                /* may leading memory leaks, but we don't care */
                return error::BAD_MEMORY_ALLOCATION;
            }
            *ppid = pp->id();
        }
        int64_t processor_id = ConstructProcessorID(dynamic_cast<Topology*>(new_tp)); //NOLINT
        assert(processor_id != -1);
        DSTREAM_DEBUG("processor id is %lu", processor_id);
        // pp->mutable_id()->set_id(processor_num++);
        pp->mutable_id()->set_id(processor_id);
    }
    /* set processor_num */
    if (new_tp->processors_size() !=
        static_cast<int>((new_tp->processor_num() + sub_tree_tp.processor_num()))) {
        DSTREAM_WARN("fail to check topology correction after merger "
                     "sub tree into app, processor_num goes in-consistent: "
                     "supposed[%d], actually[%d]",
                     new_tp->processor_num() + sub_tree_tp.processor_num(),
                     new_tp->processors_size());
        return error::CLIENT_CHECK_TP_FAIL;
    }
    new_tp->set_processor_num(new_tp->processors_size());

    /* check topology again */
    int32_t ret = CheckTopology(*old_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check topology correction after merger "
                     "sub tree into app, errstr:[%s]", error::get_text(ret));
        return error::CLIENT_CHECK_TP_FAIL;
    }

    DSTREAM_INFO("merger sub tree into app ...OK");
    return error::OK;
}

/**
* @brief  delete the sub_tree into the old_tree
*
* @param[in]       sub_tree   the sub_tree going to be remove out of the old_tree
* @param[in][out]  old_tree   the target tree
*
* @return   successed: error::OK;
*           failed:    other error code;
* @author   konghui, konghui@baidu.com
* @date   2012-04-06
*/
int32_t DelSubTreeFromApp(const Application& sub_tree, Application* old_tree) {
    DSTREAM_INFO("delete sub tree from app ...");
    assert(NULL != old_tree);
    /* check validity for addiing sub tree into the app topology */
    if (old_tree->name() != sub_tree.name()) {
        DSTREAM_WARN("bad sub tree delete: app name[%s] and sub_tree name[%s] are "
                     "surposed to be equal", old_tree->name().c_str(),
                     sub_tree.name().c_str());
        return error::CLIENT_MERGER_TOPO_FAIL;
    }
    /* delete sub-tree processors from app */
    Topology* new_tp = old_tree->mutable_topology();
    if (NULL == new_tp) {
        DSTREAM_WARN("fail to get topology from app[%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }
    size_t old_processor_num = new_tp->processor_num();
    const Topology& sub_tree_tp = sub_tree.topology();
    std::set<std::string> sub_tree_processors;
    DSTREAM_INFO("sub_tree_tp pro num is %d, old tree pro num is %zu",
                 sub_tree_tp.processor_num(), old_processor_num);
    if (sub_tree_tp.processor_num() >= old_processor_num) {
        DSTREAM_WARN("bad sub tree delete: sub_tree[%s] is bigger than app[%s], "
                     "perhaps you got the wrong sub_tree or app",
                     sub_tree.name().c_str(), old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }
    for (uint32_t i = 0; i < sub_tree_tp.processor_num(); i++) {
        sub_tree_processors.insert(sub_tree_tp.processors(i).name());
    }
    /* get the different set */
    Topology*       different_set     = new Topology();
    const Topology& old_processor_set = old_tree->topology();
    std::set<std::string>::const_iterator it;
    for (uint32_t i = 0; i < old_processor_num; i++) {
        it = sub_tree_processors.find(old_processor_set.processors(i).name());
        if (it == sub_tree_processors.end()) {
            /* add into the different set */
            Processor* pp = different_set->add_processors();
            if (NULL == pp) {
                DSTREAM_WARN("fail to allocate processor from app:"
                             "[bad memory allocation]");
                /* may leading memory leaks, but we don't care */
                delete different_set;
                return error::BAD_MEMORY_ALLOCATION;
            }
            *pp = old_processor_set.processors(i);
            /* add importer and exporter ids */
            if (IMPORTER == pp->role()) {
                ProcessorID* ppid = different_set->add_importer_id();
                if (NULL == ppid) {
                    DSTREAM_WARN("fail to add processor id to app:"
                                 "[bad memory allocation]");
                    /* may leading memory leaks, but we don't care */
                    delete different_set;
                    return error::BAD_MEMORY_ALLOCATION;
                }
                *ppid = pp->id();
            } else if (EXPORTER == pp->role()) {
                ProcessorID* ppid = different_set->add_exporter_id();
                if (NULL == ppid) {
                    DSTREAM_WARN("fail to add processor id to app:"
                                 "[bad memory allocation]");
                    /* may leading memory leaks, but we don't care */
                    delete different_set;
                    return error::BAD_MEMORY_ALLOCATION;
                }
                *ppid = pp->id();
            }
        }
    } // end of for
    /* copy topology */
    /* upstream_bigpipe_num has be dropped */
    different_set->set_processor_num(different_set->processors_size());
    /* swap topology */
    old_tree->mutable_topology()->Swap(different_set);
    delete different_set;

    /* check topology again */
    int32_t ret = CheckTopology(*old_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check topology correction after delete "
                     "sub tree from app, errstr:[%s]", error::get_text(ret));
        return error::CLIENT_CHECK_TP_FAIL;
    }

    DSTREAM_INFO("delete sub tree from app ...OK");
    return error::OK;
}

/**
* @brief  merge the importer into the old_tree
*
* @param[in]       importer   the importer going to be merged into the old_tree
* @param[in][out]  old_tree   the target tree
*
* @return   successed: error::OK;
*           failed:    other error code;
* @author   fangjun, fangjun02@baidu.com
* @date   2012-07-20
*/
int32_t MergeImporterIntoApp(const Application& importer, Application* old_tree) {
    DSTREAM_INFO("merge importer into app ...");
    assert(NULL != old_tree);
    int32_t ret;

    /* check validity for adding importer into the app topology */
    if (old_tree->name() != importer.name()) {
        DSTREAM_WARN("bad app merge: app name[%s] and improter name[%s] are "
                     "suppose to be equal", old_tree->name().c_str(), importer.name().c_str());
        return error::CLIENT_MERGER_TOPO_FAIL;
    }

    ret = CheckImporter(old_tree, importer, ADD_IMPORTER);
    if (ret < error::OK) {
        DSTREAM_WARN("invalid topology when add importer from app[%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }

    /* merge importer into app */
    Topology* new_tp = old_tree->mutable_topology();
    if (NULL == new_tp) {
        DSTREAM_WARN("fail to get topology from app[%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }

    // size_t processor_num = new_tp->processor_num();
    const Topology& importer_tp = importer.topology();
    for (uint32_t i = 0; i < importer_tp.processor_num(); i++) {
        Processor pro = importer_tp.processors(i);
        /*if (NULL == pro) {
          DSTREAM_WARN("fail tp get processor from app");
          return error::CLIENT_MERGER_TOPO_FAIL;
        }*/
        /* if is importer, add into app */
        if (IMPORTER == pro.role()) {
            Processor* pp = new_tp->add_processors();
            if (NULL == pp) {
                DSTREAM_WARN("fail to allocate processor from app:"
                             "[bad memory allocation]");
                return error::BAD_MEMORY_ALLOCATION;
            }
            *pp = pro;
            ProcessorID* ppid = new_tp->add_importer_id();
            if (NULL == ppid) {
                DSTREAM_WARN("fail to allocate processor from app:"
                             "[bad memory allocation]");
                return error::BAD_MEMORY_ALLOCATION;
            }
            *ppid = pro.id();
            // pp->mutable_id()->set_id(processor_num++);
            int64_t processor_id = ConstructProcessorID(dynamic_cast<Topology*>(new_tp)); //NOLINT
            assert(processor_id != -1);
            pp->mutable_id()->set_id(processor_id);
        } else {
            /* if is pe or exporter, change subscribe */
            for (uint32_t j = 0; j < new_tp->processor_num(); j++) {
                if (strcmp(new_tp->processors(j).name().c_str(), pro.name().c_str()) == 0) {
                    Subscribe* sub = new_tp->mutable_processors(j)->mutable_subscribe();
                    for (int32_t k = 0; k < pro.subscribe().bigpipe_size(); k++) {
                        BigpipeSub* bigsub = sub->add_bigpipe();
                        if (NULL == bigsub) {
                            DSTREAM_WARN("fail to allocate bigpipesub from subscribe:"
                                         "[bad memory allocation]");
                            return error::BAD_MEMORY_ALLOCATION;
                        }
                        *bigsub = pro.subscribe().bigpipe(k);
                    }
                    for (int32_t k = 0; k < pro.subscribe().hdfs_size(); k++) {
                        HdfsSub* hdfssub = sub->add_hdfs();
                        if (NULL == hdfssub) {
                            DSTREAM_WARN("fail to allocate hdfssub from subscribe:"
                                         "[bad memory allocation]");
                            return error::BAD_MEMORY_ALLOCATION;
                        }

                        *hdfssub = pro.subscribe().hdfs(k);
                    }
                    for (int32_t k = 0; k < pro.subscribe().processor_size(); k++) {
                        ProcessorSub* prosub = sub->add_processor();
                        if (NULL == prosub) {
                            DSTREAM_WARN("fail to allocate prosub from subscribe:"
                                         "[bad memory allocation]");
                            return error::BAD_MEMORY_ALLOCATION;
                        }
                        *prosub = pro.subscribe().processor(k);
                    }
                    for (int32_t k = 0; k < pro.subscribe().localfile_size(); k++) {
                        LocalSub* locsub = sub->add_localfile();
                        if (NULL == locsub) {
                            DSTREAM_WARN("fail to allocate locsub from subscribe:"
                                         "[bad memory allocation]");
                            return error::BAD_MEMORY_ALLOCATION;
                        }
                        *locsub = pro.subscribe().localfile(k);
                    }
                    break;
                } // end of if
            } // end of for
        } // end of if
    } // end of for
    new_tp->set_processor_num(new_tp->processors_size());
    DumpProcessor(*new_tp);

    /* check topology again */
    ret = CheckTopology(*old_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check topology correction after merge "
                     "importer into app, errstr:[%s]", error::get_text(ret));
        return error::CLIENT_CHECK_TP_FAIL;
    }

    DSTREAM_INFO("merger improter into app ...OK");
    DumpProcessor(*new_tp);
    return error::OK;
}
/**
* @brief  delete the importer from the old_tree
*
* @param[in]       importer   the importer going to be deleted from the old_tree
* @param[in][out]  old_tree   the target tree
*
* @return   successed: error::OK;
*           failed:    other error code;
* @author   fangjun, fangjun02@baidu.com
* @date   2012-07-20
*/
int32_t DelImporterFromApp(const Application& importer, Application* old_tree) {
    DSTREAM_INFO("del importer from app ...");
    assert(NULL != old_tree);
    int32_t ret = error::OK;

    /* check validity for deleting importer from the app topology */
    if (old_tree->name() != importer.name()) {
        DSTREAM_WARN("bad importer delete: app name [%s] and importer name [%s] are"
                     "supposed to be equal", old_tree->name().c_str(),
                     importer.name().c_str());
        return error::CLIENT_MERGER_TOPO_FAIL;
    }

    ret = CheckImporter(old_tree, importer, DEL_IMPORTER);
    if (ret < error::OK) {
        DSTREAM_WARN("invalid topology when del importer from app [%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }

    /* delete importer processors from app */
    Topology* new_tp = old_tree->mutable_topology();
    if (NULL == new_tp) {
        DSTREAM_WARN("fail to get topology from app[%s]",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }

    size_t old_processor_num = new_tp->processor_num();
    const Topology& importer_tp = importer.topology();
    std::set<std::string> importer_processors;
    std::set<std::string> subscribe_importer_processors;
    std::set<std::string>::iterator it;
    for (uint32_t i = 0; i < importer_tp.processor_num(); i++) {
        if (IMPORTER == importer_tp.processors(i).role()) {
            importer_processors.insert(importer_tp.processors(i).name());
        } else {
            subscribe_importer_processors.insert(importer_tp.processors(i).name());
        }
    }
    Topology* different_set = new Topology();
    Topology* old_processor_set = old_tree->mutable_topology();

    ClearOldTag(old_processor_num, importer_processors,
                subscribe_importer_processors, old_processor_set);

    /* get the different set */
    for (uint32_t i = 0; i < old_processor_num; i++) {
        it = importer_processors.find(old_processor_set->processors(i).name());
        if (it == importer_processors.end()) {
            /* add into the different set */
            Processor* pp = different_set->add_processors();
            if (NULL == pp) {
                DSTREAM_WARN("fail to allocate processor from app:"
                             "[bad memory allocation]");
                delete different_set;
                return error::BAD_MEMORY_ALLOCATION;
            }
            *pp = old_processor_set->processors(i);

            /* add importer and exporter ids */
            if (IMPORTER == pp->role()) {
                ProcessorID* ppid = different_set->add_importer_id();
                if (NULL == ppid) {
                    DSTREAM_WARN("fail to add processor id to app:"
                                 "[bad memory allication]");
                    delete different_set;
                    return error::BAD_MEMORY_ALLOCATION;
                }
                *ppid = pp->id();
            } else if (EXPORTER == pp->role()) {
                ProcessorID* ppid = different_set->add_exporter_id();
                if (NULL == ppid) {
                    DSTREAM_WARN("fail to add processor id to app:"
                                 "[bad memory allication]");
                    delete different_set;
                    return error::BAD_MEMORY_ALLOCATION;
                }
                *ppid = pp->id();
            } // end of if importer or exporter
        } // end of if find
    } // end of for

    /* check topology again */
    ret = CheckTopology(*old_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check topology correction after del"
                     "improter from app, errstr: [%s]", error::get_text(ret));

        delete different_set;
        return error::CLIENT_CHECK_TP_FAIL;
    }

    /* copy topology */
    different_set->set_processor_num(different_set->processors_size());
    /* swap topology */
    old_tree->mutable_topology()->Swap(different_set);
    delete different_set;

    DumpProcessor(*new_tp);
    DSTREAM_INFO("del importer from app ...OK");
    return ret;
}

/**
* @brief  Remove the tag of the importer to be deleted
*
* @param[in]       old_processor_num
* @param[in]       importer_processors
* @param[in]       subscribe_importer_processors
* @param[in][out]  old_processor_set
* @author   fangjun, fangjun02@baidu.com
* @date   2012-12-03
*/
void ClearOldTag(const uint32_t old_processor_num,
                 const std::set<std::string>& importer_processors,
                 const std::set<std::string>& subscribe_importer_processors,
                 Topology* old_processor_set) {
    assert(NULL != old_processor_set);
    std::set<std::string>::iterator it, subit;
    for (it = importer_processors.begin(); it != importer_processors.end(); it++) {
        for (uint32_t j = 0; j < old_processor_num; j++) {
            subit = subscribe_importer_processors.find(old_processor_set->processors(j).name());
            if (subit != subscribe_importer_processors.end()) {
                Subscribe* orig_sub = old_processor_set->mutable_processors(j)->mutable_subscribe();
                for (int32_t k = orig_sub->processor().size() - 1; k >= 0; k--) {
                    if (strcmp(orig_sub->processor(k).name().c_str(), it->c_str()) == 0) {
                        orig_sub->mutable_processor()->SwapElements(k,
                                            orig_sub->processor().size() - 1);
                        orig_sub->mutable_processor()->ReleaseLast();
                    }
                }
            }
        }
    }
    DumpProcessor(*old_processor_set);
}
/**
* @brief  update the parallelism from the old_tree
*
* @param[in][out]  old_tree   the target tree
* @param[in]       update     the app going to be updated from the old_tree
*
* @return   successed: error::OK;
*           failed:    other error code;
* @author   fangjun, fangjun02@baidu.com
* @date   2012-08-24
*/
int32_t UpdateParallelismFromApp(const Application& update, Application* old_tree) {
    DSTREAM_INFO("update parallelism from app ...");
    assert(NULL != old_tree);
    int32_t ret = error::OK;
    uint32_t i, j;

    /* check validity for update parallelism from the app */
    if (old_tree->name() != update.name()) {
        DSTREAM_WARN("bad update parallelism: app name [%s] and update name [%s] are"
                     "supposed to be equal", old_tree->name().c_str(), update.name().c_str());
        return error::CLIENT_MERGER_TOPO_FAIL;
    }

    /* check and update parallelism from orignal app */
    Topology* new_tp = old_tree->mutable_topology();
    if (NULL == new_tp) {
        DSTREAM_WARN("fail to get topology from app [%s]:",
                     old_tree->name().c_str());
        return error::BAD_ARGUMENT;
    }

    const Topology& update_tp = update.topology();
    if (update_tp.processor_num() != static_cast<uint32_t>(update_tp.processors_size())) {
        DSTREAM_WARN("bad update parallelism: processor number does not match");
        return error::CLIENT_MERGER_TOPO_FAIL;
    }
    for ( i = 0; i < update_tp.processor_num(); i++ ) {
        for (j = 0; j < new_tp->processor_num(); j++) {
            if (update_tp.processors(i).name() == new_tp->processors(j).name()) {
                new_tp->mutable_processors(j)->set_parallism(update_tp.processors(i).parallism());
                break;
            }
        }
        if (j == new_tp->processor_num()) {
            DSTREAM_WARN("bad update parallelism: processor[%s] does not exist",
                         update_tp.processors(i).name().c_str());
            return error::CLIENT_MERGER_TOPO_FAIL;
        }
    }
    DumpProcessor(*new_tp);
    DSTREAM_INFO("update parallelism from app ...OK");
    return ret;
}


static int32_t InitAppToDag(const Application& app, Dag* dag);
static bool CheckDag(const Dag& dag, bool not_allow_mis_node);

int32_t CheckTopology(const Application& app,
                      bool check_upstream_existence/*=true*/) {
    DSTREAM_INFO("check app topology validity...");
    int32_t ret = error::OK;

    /* check processors' names those should be unique */
    std::set<std::string> processor_names;
    const Topology& tp = app.topology();
    for (int32_t i = 0 ; i < tp.processors_size(); i++) {
        processor_names.insert(tp.processors(i).name());
    }
    if (static_cast<int32_t>(processor_names.size()) != tp.processors_size()) {
        DSTREAM_WARN("bad topology Description: processor name should be unique");
        return error::CLIENT_CHECK_TP_FAIL;
    }

    /* init dag */
    Dag dag;
    DSTREAM_DEBUG("init app topology into a dag...");
    ret = InitAppToDag(app, &dag);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init app into dag[%s], please check your app's "
                     "topology again!", error::get_text(ret));
        return ret;
    }
    DSTREAM_DEBUG("init app topology into a dag...OK");

    /* check integrity
     * 1.check import, exporter, processor
     *   1) exporter must have 0 out-degree
     *   2) importer and processor must have more in-degree and out-degree than 0(TODO)
     *   3) the dag should be connective and every upstream should exist;
     */
    DSTREAM_DEBUG("check dag validity...");
    /* check in-degree */
    bool failed = false;
    Dag::AdjacencMatrix::const_iterator it;
    for (it = dag.m_vec.begin(); it != dag.m_vec.end(); it++) {
        if (0 == it->first.input_num && it->first.role != IMPORTER) {
            DSTREAM_WARN("invalid topology: processor[%s] has no subscribes, "
                         "please check your app's topology again!",
                         it->first.name.c_str());
            failed = true;
        }
    }
    if (failed) {
        return error::CLIENT_CHECK_TP_FAIL;
    }

    if (!CheckDag(dag, check_upstream_existence)) {
        DSTREAM_WARN("fail to check app's topology, invalid DAG topology,"
                     " please check your app's topology again!");
        return error::CLIENT_CHECK_TP_FAIL;
    }
    DSTREAM_DEBUG("check dag validity...OK");

    /* check connectivity */
    if (check_upstream_existence) {
        DSTREAM_DEBUG("check dag connectivity...");
        Dag::AdjacencMatrix::const_iterator dag_it;
        failed = false;
        for (dag_it = dag.m_vec.begin(); dag_it != dag.m_vec.end(); dag_it++) {
            Dag::PInput::const_iterator up_it;
            for (up_it = dag_it->second.begin(); up_it != dag_it->second.end(); up_it++) {
                if (processor_names.find(*up_it) == processor_names.end()) {
                    DSTREAM_WARN("invalid topology: processor[%s] subscribes nonexistent "
                                 "upstream[%s], please check your app's topology again!",
                                 dag_it->first.name.c_str(), up_it->c_str());
                    failed = true;
                }
            }
        }
        if (failed) {
            DSTREAM_WARN("check dag connectivity...FAIL");
            return error::CLIENT_CHECK_TP_FAIL;
        }
        DSTREAM_DEBUG("check dag connectivity...OK");
    }

    DSTREAM_INFO("check app topology validity...OK");
    return error::OK;
}

int32_t CheckImporter(const Application* old_tree, const Application& importer,
                      bool add_or_delete) {
    DSTREAM_INFO("check importer topology validity...");
    int32_t ret = error::OK;

    /* check processors' names those should be unique*/
    std::set<std::string> importer_names;
    std::set<std::string> normal_pe_names;
    std::set<std::string> subscribe_importer_names;
    uint32_t importer_num = 0;
    const Topology& importer_tp = importer.topology();
    for (int32_t i = 0; i < importer_tp.processors_size(); i++) {
        if (IMPORTER == importer_tp.processors(i).role()) {
            importer_num++;
            importer_names.insert(importer_tp.processors(i).name());
        } else {
            normal_pe_names.insert(importer_tp.processors(i).name());
            for (int32_t j = 0; j < importer_tp.processors(i).subscribe().processor_size(); j++) {
                subscribe_importer_names.insert(
                            importer_tp.processors(i).subscribe().processor(j).name());
            }
        }
    }

    /*if (add_or_delete == DEL_IMPORTER) {
      if (normal_pe_names.size() != 0) {
        DSTREAM_WARN("bad topology Description: delete importer topology should not include normal pe");
        return error::CLIENT_CHECK_TP_FAIL;
      }
    }*/
    if (static_cast<uint32_t>(importer_names.size()) != importer_num) {
        DSTREAM_WARN("bad topology Description: processor name should be unique");
        return error::CLIENT_CHECK_TP_FAIL;
    }

    /* check if importer already exists
     * if exists, return fail
     */
    std::set<std::string> old_tp_names;
    const Topology& old_tp = old_tree->topology();
    for (int32_t i = 0; i < old_tp.processors_size(); i++) {
        old_tp_names.insert(old_tp.processors(i).name());
    }
    std::set<std::string>::iterator find_it;
    std::set<std::string>::iterator it;
    for (find_it = importer_names.begin(); find_it != importer_names.end(); find_it++) {
        for (it = old_tp_names.begin(); it != old_tp_names.end(); it++) {
            if ((*it) == (*find_it)) {
                break;
            }
        }
        if (it != old_tp_names.end() && add_or_delete == ADD_IMPORTER) {
            DSTREAM_WARN("bad topology Description: importer already exists");
            return error::CLIENT_CHECK_TP_FAIL;
        } else if (it == old_tp_names.end() && add_or_delete == DEL_IMPORTER) {
            DSTREAM_WARN("bad topology Description: importer does not exist");
            return error::CLIENT_CHECK_TP_FAIL;
        }
    }

    if (add_or_delete == ADD_IMPORTER) {
        /* check if pe already exists
         * if not exists, return fail
         */
        for (find_it = normal_pe_names.begin();
             find_it != normal_pe_names.end();
             find_it++) {
            for (it = old_tp_names.begin();
                 it != old_tp_names.end();
                 it++) {
                if ((*it) == (*find_it)) {
                    break;
                }
            }
            if (it == old_tp_names.end()) {
                DSTREAM_WARN("bad topology Description: subscribe pe does not exists");
                return error::CLIENT_CHECK_TP_FAIL;
            }
        }
        /* check if all new importer been subscribed
         * if not, return fail
         */
        for (find_it = importer_names.begin();
             find_it != importer_names.end();
             find_it++) {
            for (it = subscribe_importer_names.begin();
                 it != subscribe_importer_names.end();
                 it++) {
                if ((*it) == (*find_it)) {
                    break;
                }
            }
            if (it == subscribe_importer_names.end()) {
                DSTREAM_WARN("bad topology Description: new importer has not been subscribed");
                return error::CLIENT_CHECK_TP_FAIL;
            }
        }
        /* check if all new pe subscribes new importer
         * if not, return fail
         */
        if (subscribe_importer_names.size() != importer_names.size()) {
            DSTREAM_WARN("bad topology Description: subscribe old importer");
            return error::CLIENT_CHECK_TP_FAIL;
        }
    } // end of if
    return ret;
}

int32_t BuildApp(const std::string app_dir, Application* app) {
    int32_t ret = error::OK;

    ret = InitApp(app_dir, true, false, false, false, app);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init topology from app dir:[%s]", app_dir.c_str());
        return ret;
    }

    /* check topology */
    ret = CheckTopology(*app);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check topology");
        return ret;
    }
    DSTREAM_DEBUG("topology check ...OK");

    /* build topology, and check */
    /*  ret = BuildTopology(app);
      if (ret < error::OK) {
        DSTREAM_WARN("fail to build topology");
        return ret;
      }
    */
    return error::OK;
}

int64_t ConstructProcessorID(const Topology* topo) {
    std::set<int64_t> processorid_set;
    std::set<int64_t>::iterator iter;
    for (int32_t i = 0; i < topo->processors().size(); i++) {
        processorid_set.insert(topo->processors(i).id().id());
        DSTREAM_DEBUG("processor id set has %lu", topo->processors(i).id().id());
    }
    for (int64_t i = 0; i < 256; i++) {
        if ((iter = processorid_set.find(i)) == processorid_set.end()) {
            return i;
        }
    }
    return -1;
}

int32_t GenerateUploadFile(const std::string path, VectorString* to_be_upload) {
    struct dirent dir_entry;
    struct dirent* pdir_result = NULL;
    DIR* dirp;
    if ((dirp = opendir(path.c_str())) == NULL) {
        return error::EMPTY_DIRECTORY;
    }
    while (!readdir_r(dirp, &dir_entry, &pdir_result) && pdir_result != NULL) {
        if (strcmp(dir_entry.d_name, ".") == 0 ||
            strcmp(dir_entry.d_name, "..") == 0 ||
            strcmp(dir_entry.d_name, ".svn") == 0) {
            continue;
        }
        std::string file(dir_entry.d_name);
        to_be_upload->push_back(file);
    }
    closedir(dirp);
    return error::OK;
}

static int32_t InitAppToDag(const Application& app, Dag* dag) {
    if (NULL == dag) {
        return error::BAD_ARGUMENT;
    }

    const Topology& tp = app.topology();
    int32_t pnum = tp.processor_num();
    if ( pnum != tp.processors_size() ) {
        DSTREAM_WARN("bad topology Description: processor num is not consistent"
                     "set:[%d], actually:[%d]",
                     tp.processor_num(), tp.processors_size());
        return error::CLIENT_CHECK_TP_FAIL;
    }

    /* format processors into dag */
    for (int32_t i = 0; i < pnum; i++) {
        const Processor& p = tp.processors(i);
        Dag::PVertex ver;
        Dag::PInput  pinput;
        ver.name = p.name();
        ver.role = p.role();
        /* construct edgs in this dag */
        /* get the subscribe list and construct the edgs */
        if (p.has_subscribe()) {
            const Subscribe& sub = p.subscribe();
            int32_t nsub = sub.processor_size();
            for (int sub_index = 0; sub_index < nsub; sub_index++) {
                if (pinput.insert(sub.processor(sub_index).name()).second) {
                    /* processor not exists */
                    ver.input_num++;
                }
            }
        }
        dag->m_vec[ver] = pinput;
    }

    return error::OK;
}

static bool CheckDag(const Dag& dag, bool not_allow_mis_node = true) {
    Dag sort_dag = dag;
    std::deque<std::string> zero_degree_ver;

    /* remove those not belong the sub tree */
    if (!not_allow_mis_node) {
        std::set<std::string> nodes;
        Dag::AdjacencMatrix::iterator dag_it;
        for (dag_it = sort_dag.m_vec.begin(); dag_it != sort_dag.m_vec.end(); dag_it++) {
            nodes.insert(dag_it->first.name);
        }
        for (dag_it = sort_dag.m_vec.begin(); dag_it != sort_dag.m_vec.end();) {
            Dag::PInput::iterator up_it;
            bool has_breakup = false;
            for (up_it = dag_it->second.begin(); up_it != dag_it->second.end(); ) {
                if (nodes.find(*up_it) == nodes.end()) {
                    Dag::PVertex v   = dag_it->first;
                    Dag::PInput  pin = dag_it->second;
                    v.input_num--;
                    pin.erase(*up_it);
                    sort_dag.m_vec.erase(dag_it);
                    sort_dag.m_vec[v] = pin;
                    has_breakup = true;
                    break;
                } else {
                    ++up_it;
                }
            }
            if (has_breakup) {
                dag_it = sort_dag.m_vec.begin();
                has_breakup = false;
            } else {
                ++dag_it;
            }
        } // end of for
    }

    /* find vertexs with zero input degree */
    Dag::AdjacencMatrix::iterator it;
    for (it = sort_dag.m_vec.begin(); it != sort_dag.m_vec.end();) {
        if (0 == it->first.input_num) {
            zero_degree_ver.push_front(it->first.name);
            sort_dag.m_vec.erase(it++);
        } else {
            ++it;
        }
    }
    size_t count = 0;
    while (!zero_degree_ver.empty()) {
        std::string name = zero_degree_ver.front();
        zero_degree_ver.pop_front();
        count++;
        /* remove adjacence edges */
        Dag::AdjacencMatrix::iterator dag_it;
        for (dag_it = sort_dag.m_vec.begin(); dag_it != sort_dag.m_vec.end();) {
            Dag::PInput& input_set = dag_it->second;
            Dag::PInput::iterator input_it;
            input_it = input_set.find(name);
            if (input_it != input_set.end()) {
                Dag::PVertex v   = dag_it->first;
                Dag::PInput  pin = dag_it->second;
                v.input_num--;
                pin.erase(name);
                sort_dag.m_vec.erase(dag_it++);
                if (v.input_num <= 0) {
                    zero_degree_ver.push_front(v.name);
                } else {
                    sort_dag.m_vec[v] = pin;
                }
                dag_it = sort_dag.m_vec.begin();
            } else {
                ++dag_it;
            }
        } // end of for
    } // end of while
    if (count != dag.m_vec.size()) {
        std::set<std::string> nodes;
        Dag::AdjacencMatrix::const_iterator dag_it;
        /* print debug */
        std::string cir_sub_nodes;
        for (dag_it = sort_dag.m_vec.begin(); dag_it != sort_dag.m_vec.end(); dag_it++) {
            if (!cir_sub_nodes.empty()) {
                cir_sub_nodes += "\t";
            }
            cir_sub_nodes += dag_it->first.name;
        }
        DSTREAM_INFO("may a circle subscribe exists or nodes does not exist "
                     "in these nodes seq:[%s]", cir_sub_nodes.c_str());
        return false;
    }
    return true;
}

} // namespace client
} // namespace  dstream
