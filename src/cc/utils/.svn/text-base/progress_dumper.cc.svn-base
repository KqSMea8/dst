/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/



/**
 * @file progress_dumper2.cc
 * @author zhenpeng(zhenpeng@baidu.com)
 * @date 2013/04/22 16:41:25
 * @version $Revision$
 * @brief
 *
 **/

#include <getopt.h>
#include <unistd.h>
#include "common/application_tools.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/zk_client.h"
#include "common/id.h"
#include "common/logger.h"

#define OUTPUT_FORMAT "APP: %s\tProcessor: %s\tPipelet: %s\tMessage: %ld\tSequence: %lu\n"

static struct option kOptios[] = {
    {"conf", 1, 0, 'f'},
    {"cmd", 1, 0, 'c'},
    {"app", 1, 0, 'a'},
    {"processor", 1, 0, 'p'},
    {"pipelet", 1, 0, 'i'},
    {"point", 1, 0, 's'},
    {"file", 1, 0, 'd'},
    {"force", 0, 0, 'o'},
    {0, 0, 0, 0}
};

struct CmdOption {
    std::string conf;
    std::string cmd;
    std::string app;
    std::string processor;
    std::string pipelet;
    std::string subpoint;
    std::string file;
    bool force;
    CmdOption(): force(false) {}
};

using namespace dstream;

class ProgressDumper {
public:
    int Init(int argc, char** argv) {
        int ret = 0;
        do {
            if (ParseArgs(argc, argv) != 0) {
                ret = error::BAD_ARGUMENT;
                PrintHelp();
                break;
            }
            
            if (CheckOptions() != 0) {
                ret = error::BAD_ARGUMENT;
                PrintHelp();
                break;
            }

            config::Config config;
            if (config.ReadConfig(m_option.conf) != 0) {
                ret = error::CONFIG_NOT_EXIST;
                break;
            } 

            std::string zk_addr;
            if (config.GetValue(dstream::config_const::kZKPathName, &zk_addr) != 0) {
                ret = error::CONFIG_BAD_VALUE;
                break;
            }
            std::string app_path;
            if (config.GetValue(dstream::config_const::kZKRootPathName, &app_path) != 0) {
                ret = error::CONFIG_BAD_VALUE;
                break;
            }
            m_app_path = app_path + "/SubPoint/" + m_option.app;

            m_zk = new dstream::zk_client::ZkClient(zk_addr.c_str());
            ret = m_zk->Connect();
        } while (0);
        return ret;
    }

    int Run() {
        int ret = 0;
        if (m_option.cmd == "get") {
            ret = ReadProgress();
        } else if (m_option.cmd == "set") {
            ret = SetProgress();
        } else if (m_option.cmd == "rm") {
            ret = RmProgress();
        } else if (m_option.cmd == "rmr") {
            ret = RmrProgress();
        } else {
            printf("Unkown Command: %s\n", m_option.cmd.c_str());
            PrintHelp();
            ret = -1;
        }
        return ret;
    }

private:
    void PrintHelp() {
        printf("usage: \n");
        printf(" --conf configure file path.\n");
        printf(" --cmd [get | set | rm | rmr].\n");
        printf(" --app application name.\n");
        printf(" --processor processor ID (start from 0).\n");
        printf(" --pipelet bigpipe pipelet ID (start from 0).\n");
        printf(" --point subpoint, format: 'messageid:sequenceid'. For set command.\n");
        printf(" --force when node dose not exist, create it.\n");

        printf("example: \n\n");
        printf("progress_dumper --conf importer_sub.xml --app stream "
               "--cmd get // get stream's all subpoints\n");
        printf("progress_dumper --conf importer_sub.xml --app stream "
               "--processor importer1 --cmd get // get stream's importer1 subpoints\n");
        printf("progress_dumper --conf importer_sub.xml --app stream "
               "--processor importer1 --pipelet 1 --cmd set --point 100:10 // set stream's importer1 subpoints\n");
        printf("\n");
    }

    int ParseArgs(int argc, char** argv) {
        int ret = 0;
        while (true) {
            int index = 0;
            int c = getopt_long(argc, argv, "a:c:i:f:p:s:d:o", kOptios, &index);
            if (c == -1) {
                break;
            }
            switch (c) {
            case 'a':
                m_option.app = optarg;
                break;
            case 'c':
                m_option.cmd = optarg;
                break;
            case 'i':
                m_option.pipelet = optarg;
                break;
            case 'f':
                m_option.conf = optarg;
                break;
            case 's':
                m_option.subpoint = optarg;
                break;
            case 'p':
                m_option.processor = optarg;
                break;
            case 'd':
                m_option.file = optarg;
                break;
            case 'o':
                m_option.force = true;
                break;
            case '?':
            default:
                printf("Unknown Options.\n");
                ret = -1;
                break;
            }
        }
        return ret;
    }

    int CheckOptions() {
        int ret = 0;
        do {
            if (m_option.conf.empty() || m_option.cmd.empty() || m_option.app.empty()) {
                printf("Error: --conf or --cmd or --app should not be null.\n");
                ret = -1;
                break;
            }

            if (m_option.cmd == "set") {
                if (m_option.file.empty()) {
                    if (m_option.processor.empty() || m_option.pipelet.empty() || m_option.subpoint.empty()) {
                        printf("Error: for `set` cmd, --processor and --pipelet and --subpoint should not be null.\n");
                        ret = -2;
                        break;
                    }
                }
            }

            if (m_option.cmd == "rm") {
                if (m_option.processor.empty() || m_option.pipelet.empty()) {
                    printf("Error: for `rm` cmd, --processor and --pipelet should not be null.\n");
                    ret = -3;
                    break;
                }
            }

        } while (0);
        return ret;
    }

    int ReadProgress() {
        int ret = 0;
        // processor-id -> SubPointMap
        std::map<std::string, SubPointMap> subpoint_map;

        // if processor is null, we get all processors
        if (m_option.processor.empty()) {
            std::list<std::string> processor_list;
            ret = m_zk->GetChildList(m_app_path.c_str(), &processor_list);
            if (ret != 0) {
                DSTREAM_WARN("get app processor list failed.");
                return ret;
            }
            for (std::list<std::string>::iterator idx = processor_list.begin(); idx != processor_list.end(); ++idx) {
                subpoint_map.insert(std::make_pair(*idx, SubPointMap()));
                DSTREAM_INFO("get processor: %s", idx->c_str());
            }
        } else {
            subpoint_map.insert(std::make_pair(m_option.processor, SubPointMap()));
        }

        std::map<std::string, SubPointMap>::iterator sub_map_idx;
        zk_client::ZkNode* zk_node = zk_client::NewZkNode();
        for (sub_map_idx = subpoint_map.begin(); sub_map_idx != subpoint_map.end(); ++sub_map_idx) {
            std::string current_processor = sub_map_idx->first;
            SubPointMap& current_sub_map = sub_map_idx->second;
            // if pipelet is null, we get all piplets
            std::list<std::string> pipelet_list;
            if (m_option.pipelet.empty()) {
                std::string processor_path = m_app_path + "/" + current_processor;
                ret = m_zk->GetChildList(processor_path.c_str(), &pipelet_list);
                if (ret != 0) {
                    DSTREAM_WARN("get app pipelet list failed.");
                    DeleteZkNode(zk_node);
                    return ret;
                }

                std::list<std::string>::iterator pipelet_idx;
                for (pipelet_idx = pipelet_list.begin(); pipelet_idx != pipelet_list.end(); ++pipelet_idx) {
                    std::string subpoint_path = m_app_path + "/" + current_processor + "/" + *pipelet_idx;
                    SubPoint point;
                    DSTREAM_INFO("get pipelet: %s subpoint", subpoint_path.c_str());
                    ret = m_zk->GetNodeData(subpoint_path.c_str(), &(zk_node->data));
                    if (ret != 0) {
                        DSTREAM_WARN("get processor:%s pipelet: %s subpoint failed.",
                                current_processor.c_str(), pipelet_idx->c_str());
                        DeleteZkNode(zk_node);
                        return ret;
                    }
                    point.ParseFromArray(zk_node->data.m_buf, zk_node->data.m_len);
                    current_sub_map.insert(std::make_pair(*pipelet_idx, point));
                    DSTREAM_INFO("get processor: %s pipelet: %s subpoint: %ld:%lu",
                            current_processor.c_str(), pipelet_idx->c_str(), point.msg_id(), point.seq_id());
                }
            } else {
                std::string subpoint_path = m_app_path + "/" + current_processor + "/" + m_option.pipelet;
                SubPoint point;
                ret = m_zk->GetNodeData(subpoint_path.c_str(), &(zk_node->data));
                if (ret != 0) {
                    DSTREAM_WARN("get processor:%s pipelet: %s subpoint failed.",
                            current_processor.c_str(), m_option.pipelet.c_str());
                    DeleteZkNode(zk_node);
                    return ret;
                }
                point.ParseFromArray(zk_node->data.m_buf, zk_node->data.m_len);
                current_sub_map.insert(std::make_pair(m_option.pipelet, point));
                DSTREAM_INFO("get processor: %s pipelet: %s subpoint: %ld:%lu",
                        current_processor.c_str(), m_option.pipelet.c_str(), point.msg_id(), point.seq_id());
            }
        }
        DeleteZkNode(zk_node);

        // let's dump
        for (sub_map_idx = subpoint_map.begin(); sub_map_idx != subpoint_map.end(); ++sub_map_idx) {
            if (sub_map_idx->second.size() == 0) {
                continue;
            }
            SubPointMap::iterator sub_idx = sub_map_idx->second.begin();
            for (; sub_idx != sub_map_idx->second.end(); ++sub_idx) {
                printf(OUTPUT_FORMAT,
                        m_option.app.c_str(), sub_map_idx->first.c_str(), sub_idx->first.c_str(),
                        sub_idx->second.msg_id(), sub_idx->second.seq_id());
            }
        }
        
        return ret;
    }

    int SetProgress() {
        if (m_option.file.empty()) {
            return SetProgressFromCmd();
        }
        return SetProgressFromFile();
    }

    int RmProgress() {
        std::string dst_path = m_app_path + "/" + m_option.processor + "/" + m_option.pipelet;
        int ret = m_zk->DeletePersistentNode(dst_path.c_str());
        if (ret != 0) {
            printf("Error: delete %s failed, result: %s\n", dst_path.c_str(), error::get_text(ret));
            return -30;
        }
        printf("delete %s OK.\n", dst_path.c_str());
        return 0;
    }

    int RmrProgress() {
        std::string dst_path = m_app_path;
        if (!m_option.processor.empty()) {
            dst_path += "/" + m_option.processor;
            if (!m_option.pipelet.empty()) {
                dst_path += "/" + m_option.pipelet;
            }
        }

        int ret = m_zk->DeleteNodeTree(dst_path.c_str());
        if (ret != 0) {
            printf("Error: delete %s failed, result: %s\n", dst_path.c_str(), error::get_text(ret));
            return -40;
        }
        printf("delete %s OK.\n", dst_path.c_str());
        return 0;
    }

    // pipelet -> SubPoint
    typedef std::map<std::string, dstream::SubPoint> SubPointMap;

private:
    CmdOption m_option;
    dstream::zk_client::ZkClient* m_zk;
    std::string m_app_path;

    int32_t SetProgressFromCmd() {
        SubPoint point;
        point.set_pipelet(atoi(m_option.pipelet.c_str()));

        // parse cmd line
        size_t comma = m_option.subpoint.find(':');
        if (comma != std::string::npos) {
            point.set_msg_id(atoi(std::string(m_option.subpoint, 0, comma).c_str()));
            point.set_seq_id(atoi(std::string(m_option.subpoint, comma+1, m_option.subpoint.size()).c_str()));
        } else {
            DSTREAM_ERROR("invalid format for subpoint(msg_id:seq_id)");
            return -20;
        }
        DSTREAM_DEBUG("get subpoint: msg_id: %ld seq_id: %lu", point.msg_id(), point.seq_id());

        // create processor node
        std::string dst_path = m_app_path + "/" + m_option.processor;
        zk_client::ZkNode *zk_node = zk_client::NewZkNode();
        snprintf(zk_node->path.m_buf, zk_node->path.m_len, "%s", m_option.processor.c_str());
        zk_node->path.m_len = m_option.processor.size();
        zk_node->data.m_len = 0;
        m_zk->CreatePersistentNode(dst_path.c_str(), zk_node);

        // write to zk
        dst_path = m_app_path + "/" + m_option.processor + "/" + m_option.pipelet;
        snprintf(zk_node->path.m_buf, zk_node->path.m_len, "%s", m_option.pipelet.c_str());
        point.SerializeToArray(zk_node->data.m_buf, zk_node->data.m_len);
        zk_node->data.m_len = point.ByteSize();
        int ret = m_zk->SetNodeData(dst_path.c_str(), zk_node->data);
        if (ret != 0) {
            if (ret == error::ZK_NO_NODE && m_option.force) {
                DSTREAM_INFO("%s not exist, so we create it..", dst_path.c_str());
                ret = m_zk->CreatePersistentNode(dst_path.c_str(), zk_node);
            }
            if (ret != 0) {
                DSTREAM_ERROR("set subpoint failed, result: %s", error::get_text(ret));
                zk_client::DeleteZkNode(zk_node);
                return -21;
            }
        }
        printf("set subpoint processor: %s pipelet: %d msg_id: %ld seq_id: %lu OK.\n",
                m_option.processor.c_str(), point.pipelet(), point.msg_id(), point.seq_id());
        zk_client::DeleteZkNode(zk_node);
        return ret;
    }

    int32_t SetProgressFromFile() {
        std::ifstream infile(m_option.file.c_str());
        char line[1024], app[64], pcsr[64], pplt[64], spt[128];
        long msgid = 0;
        unsigned long seqid = 0;
        int ret = 0;
        while (infile.getline(line, sizeof(line))) {
            ret = sscanf(line, OUTPUT_FORMAT, app, pcsr, pplt, &msgid, &seqid);
            if (ret != 5) {
                DSTREAM_WARN("unknown input format for line: %s", line);
                continue;
            }

            // re-use SetProgressFromCmd
            m_option.processor = pcsr;
            m_option.pipelet = pplt;
            snprintf(spt, sizeof(spt), "%ld:%lu", msgid, seqid);
            m_option.subpoint = spt;
            ret = SetProgressFromCmd();
            if (ret != 0) {
                DSTREAM_WARN("set progress failed for line: %s", line);
                continue;
            }
        }
        return 0;
    }
};

int main(int argc, char** argv) {
    ::dstream::logger::initialize(argv[0]);

    ProgressDumper pd;
    int ret = pd.Init(argc, argv);
    if (ret != 0) {
        DSTREAM_WARN("progress dumper init failed, result: %s", error::get_text(ret));
        return ret;
    }
    return pd.Run();
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
