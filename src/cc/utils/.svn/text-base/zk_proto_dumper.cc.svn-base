/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 /** 
 * @file    zk_proto_dumper.cc
 * @brief   
 * @author  liuguodong,liuguodong01@baidu.com
 * @version     1.0
 * @date    2013-08-14
 * Copyright (c) 2011, Baidu, Inc. All rights reserved.
 */
#include <list>
#include "boost/algorithm/string/trim.hpp"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pn.pb.h"
#include "common/zk_client.h"
#include "common/application_tools.h"
#include "common/utils.h"
#include "metamanager/zk_meta_manager.h"
#include "json_to_pb.h"
#include "pb_to_json.h"

const char* kPMString        = "pm";
const char* kPMBackupString  = "pmbackup";
const char* kPNString        = "pn";
const char* kAppString       = "app";
const char* kPEString        = "pe";
const char* kConfigString    = "config";
const char* kPNGroupString   = "pngroup";
const char* kPMMachineString = "pmmachines";
const char* kPNMachineString = "pnmachines";

class ZKMetaDumper {
public:
    ZKMetaDumper(): m_zk(NULL), m_json_str("{") {}

    int32_t Init(const std::string& zk, const std::string& root) {
        m_root = root;
        m_zk = new(std::nothrow) dstream::zk_client::ZkClient(zk.c_str());
        if (m_zk != NULL) {
            return m_zk->Connect();
        } else {
            return dstream::error::BAD_MEMORY_ALLOCATION;
        }
    }

    const std::string& GetFormatJsonStr() {
        size_t len = m_json_str.length();
        if(len > 0 && m_json_str[len -1] == ',') {
            m_json_str[len - 1] = '}';
        } else {
            m_json_str.append("}");
        }
        return m_json_str;
    }

    int32_t DumpPN() {
        std::list<dstream::PN> pn_list;
        std::string zk_string = m_root + "/PN";
        int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, zk_string, &pn_list);
        if (ret) {
            DSTREAM_ERROR("get pn list failed, zk_root:%s errcode: %s",
                           zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }
        std::string pn_buf;
        std::string pn_str(kPNString);
        ProtoListToJson<dstream::PN>(AddQuotes(&pn_str), pn_list, &pn_buf);
        m_json_str.append(pn_buf).append(",");
        return dstream::error::OK;
    }

    int32_t DumpPM() {
        dstream::PM pm;
        std::string zk_string = m_root + "/PM";
        int ret = dstream::zk_meta_manager::GetZKData(m_zk, zk_string, &pm);
        if (ret) {
            DSTREAM_ERROR("get pm node failed, zk_root: %s errcode: %s",
                           zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }
        std::string pm_buf;
        std::string pm_str(kPMString);
        ProtoToJson<dstream::PM>(AddQuotes(&pm_str), pm, &pm_buf);
        m_json_str.append(pm_buf).append(",");
        return dstream::error::OK;
    }

    int32_t DumpApp(const std::string& appid, bool showapp, bool showpe) {
        std::string zk_string = m_root + "/App";
        std::string app_buf;
        if (appid.empty()) {
            std::list<dstream::Application> app_list;
            int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, zk_string, &app_list);
            if (ret) {
                DSTREAM_ERROR("get app failed, zk_root: %s, errcode: %s",
                        zk_string.c_str(), dstream::error::ErrStr(ret));
                return ret;
            }
            if (showapp) {
                std::string app_str(kAppString);
                app_buf.clear();
                ProtoListToJson<dstream::Application>(AddQuotes(&app_str), app_list, &app_buf);
                m_json_str.append(app_buf).append(",");
            }
            if (showpe) {
                std::list<dstream::Application>::iterator app_it = app_list.begin();
                list<dstream::ProcessorElement> all_pe_list;
                for (;app_it != app_list.end(); ++app_it) {
                    std::string pe_zk_string(zk_string);
                    std::list<dstream::ProcessorElement> pe_list;
                    pe_zk_string += "/";
                    dstream::StringAppendNumber(&pe_zk_string, app_it->id().id());
                    int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, pe_zk_string, &pe_list);
                    if (ret) {
                        DSTREAM_ERROR("get pe list failed, zk_root: %s, errcode: %s",
                                       pe_zk_string.c_str(), dstream::error::ErrStr(ret));
                    }
                    all_pe_list.insert(all_pe_list.end(), pe_list.begin(), pe_list.end());
                }
                app_buf.clear();
                std::string pe_str(kPEString);
                ProtoListToJson<dstream::ProcessorElement>(AddQuotes(&pe_str), all_pe_list, &app_buf);
                m_json_str.append(app_buf).append(",");
            }
        } else {
            string pe_zk_string(zk_string);
            pe_zk_string += "/";
            pe_zk_string.append(appid);
            if (showapp) {
                dstream::Application app;
                int ret = dstream::zk_meta_manager::GetZKData(m_zk,  pe_zk_string, &app);
                if (ret) {
                    DSTREAM_ERROR("get app failed, zk_root: %s, errcode: %s",
                                   pe_zk_string.c_str(), dstream::error::ErrStr(ret));
                    return ret;
                }
                std::string app_str(kAppString);
                app_buf.clear();
                ProtoToJson<dstream::Application>(AddQuotes(&app_str), app, &app_buf);
                m_json_str.append(app_buf).append(",");
            }
            if(showpe) {
                std::list<dstream::ProcessorElement> pe_list;
                int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, pe_zk_string, &pe_list);
                if (ret) {
                    DSTREAM_ERROR("get pe list failed, zk_root: %s, errcode: %s",
                            pe_zk_string.c_str(), dstream::error::ErrStr(ret));
                }
                std::string pe_str(kPEString);
                app_buf.clear();
                ProtoListToJson<dstream::ProcessorElement>(AddQuotes(&pe_str), pe_list, &app_buf);
                m_json_str.append(app_buf).append(",");
            }
        }
        return dstream::error::OK;
    }
    int32_t DumpBackupPM() {
        std::list<dstream::PM> pm_list;
        std::string zk_string = m_root + "/BackupPM";
        int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, zk_string, &pm_list);
        if (ret) {
            DSTREAM_ERROR("get backup pm failed, zk_root: %s, errcode: %s", zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }
        std::string back_pm_buf;
        std::string back_pm_str(kPMBackupString);
        ProtoListToJson<dstream::PM>(AddQuotes(&back_pm_str), pm_list, &back_pm_buf);
        m_json_str.append(back_pm_buf).append(",");
        return dstream::error::OK;
    }

    int32_t DumpConfig() {
        dstream::ClusterConfig config;
        std::string zk_string = m_root + "/Config";
        int ret = dstream::zk_meta_manager::GetZKData(m_zk, zk_string, &config);
        if (ret) {
            DSTREAM_ERROR("get config failed, zk_root: %s, errocode: %s", zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }

        std::string conf_buf;
        std::string conf_str(kConfigString);
        ProtoToJson<dstream::ClusterConfig>(AddQuotes(&conf_str), config, &conf_buf);
        m_json_str.append(conf_buf).append(",");
        return dstream::error::OK;
    }

    int32_t DumpPNGroup() {
        std::list<dstream::GroupInfo> group_list;
        std::string zk_string = m_root + "/PNGroup";
        int ret = dstream::zk_meta_manager::GetZKChildData(m_zk, zk_string, &group_list);
        if (ret) {
            DSTREAM_ERROR("get pn group failed, zk_root: %s, errcode: %s",
                           zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }

        std::string group_buf;
        std::string group_str(kPNGroupString);
        ProtoListToJson<dstream::GroupInfo>(AddQuotes(&group_str), group_list, &group_buf);
        m_json_str.append(group_buf).append(",");
        return dstream::error::OK;
    }
    int32_t DumpPMMachines() {
        dstream::PMMachines machines;
        std::string zk_string = m_root + "/machines/pm_machines";
        int ret = dstream::zk_meta_manager::GetZKData(m_zk, zk_string, &machines);
        if (ret) {
            DSTREAM_ERROR("get pn group failed, zk_root: %s, errcode: %s",
                           zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }
        std::string machine_buf;
        std::string machine_str(kPMMachineString);
        ProtoToJson<dstream::PMMachines>(AddQuotes(&machine_str), machines, &machine_buf);
        m_json_str.append(machine_buf).append(",");
        return dstream::error::OK;
    }
    int32_t DumpPNMachines() {
        dstream::PNMachines machines;
        std::string zk_string = m_root + "/machines/pn_machines";
        int ret = dstream::zk_meta_manager::GetZKData(m_zk, zk_string, &machines);
        if (ret) {
            DSTREAM_ERROR("get pn group failed, zk_root: %s, errcode: %s",
                           zk_string.c_str(), dstream::error::ErrStr(ret));
            return ret;
        }
        std::string machine_buf;
        std::string machine_str(kPNMachineString);
        ProtoToJson<dstream::PNMachines>(AddQuotes(&machine_str), machines, &machine_buf);
        m_json_str.append(machine_buf).append(",");
        return dstream::error::OK;
    }
    int32_t DumpAll() {
       string app;
       DumpPM();
       DumpBackupPM();
       DumpPN();
       DumpApp(app, true, true);
       DumpConfig();
       DumpPNGroup();
       DumpPMMachines();
       DumpPNMachines();
       return dstream::error::OK;
    }

private:
    template <typename T>
    int32_t ProtoListToJson(const std::string& key,
                        const std::list<T>& pb_list, std::string *buf) {
        std::string back_buf = key + ":[";
        typename std::list<T>::const_iterator iter = pb_list.begin();
        for (; iter != pb_list.end(); ++iter) {
            std::string back_tmp;
            ProtoMessageToJson(*iter, &back_tmp, NULL);
            boost::algorithm::trim_if(back_tmp, boost::is_any_of("\n\t "));
            if (iter != pb_list.begin()) {
                back_buf.append(",").append(back_tmp);
            } else {
                back_buf.append(back_tmp);
            }
        }
        back_buf.append("]");
        buf->assign(back_buf);
        return dstream::error::OK; 
    }

    template <typename T>
    int32_t ProtoToJson(const std::string& key,
                    const T& proto, std::string *buf) {
        std::string back_buf;
        ProtoMessageToJson(proto, &back_buf, NULL);
        boost::algorithm::trim_if(back_buf, boost::is_any_of("\n\t "));
        buf->append(key).append(":").append(back_buf);
        return dstream::error::OK;
    }
    inline const std::string& AddQuotes(std::string* str) {
        str->assign(str->insert(0, "\""));
        str->append("\"");
        return *str;
    }

    dstream::zk_client::ZkClient* m_zk;
    std::string m_root;
    std::string m_json_str;
};

void PrintUsage(const char* self) {
    std::cout << "usage:\n"
              << self <<
              " -h zk_host -p zk path"
              " -m [all/pm,pmbackup,pn,app,pe,config,pngroup,pmmachines,pnmachines]"
              " -a app_id\n"
              "for example: [-m all] to dump all segment\n"
              "[-m pm,pn] to dump pm and pn segment\n"
              "for [-m app,pe,..], [-a appid] would dump only "
              "the corresponding appid of app or pe,"
              "othewise, all app or pe will be dump."
              << std::endl;
}

int main(int argc, char **argv)
{
    dstream::Logger::initialize("ZK_PROTO_DUMPER", Logging::WARN_LOG_LEVEL);
    std::string zk_host;
    std::string zk_root;
    std::string mod_to_dump;
    std::string app_id;
    ZKMetaDumper zk_dumper;
    int opt;
    while((opt = getopt(argc, argv, "h:p:m:a:")) != -1) {
        switch (opt) {
            case 'h': {
                if(NULL != optarg) {
                    zk_host = optarg;
                }
                break;
            }
            case 'p': {
                if(NULL != optarg) {
                    zk_root = optarg;
                }
                break;
            }
            case 'm': {
                if(NULL != optarg) {
                    mod_to_dump = optarg;
                } 
                break;
            }
            case 'a': {
                if(NULL != optarg) {
                    app_id = optarg;
                } 
                break;
            }
            default : {
                printf("unknown command: -%c %s\n", optopt, optarg);
                PrintUsage(argv[0]);
                return -1;
            }
        }
    }
    if (zk_host.empty()) {
        std::cout << "zk_host must be provided, using -h zk_host " << std::endl;
        PrintUsage(argv[0]);
        return -1;
    }
    if (zk_root.empty()) {
        std::cout << "zk_root must be provided, using -p zk_root" << std::endl;
        PrintUsage(argv[0]);
        return -1;
    }
    int32_t ret = zk_dumper.Init(zk_host, zk_root);
    if (ret) {
        DSTREAM_WARN("zk meta dumper init error: %s", dstream::error::ErrStr(ret));
        return -1;
    }
    if(mod_to_dump == "all") {
        zk_dumper.DumpAll();
    } else if(!mod_to_dump.empty()){
        std::stringstream module_to_split(mod_to_dump);
        std::string mod;
        bool dump_app = false;
        bool dump_pe  = false;
        while(std::getline(module_to_split, mod, ',')) {
            if (mod == kPMString) {
                if(zk_dumper.DumpPM() < dstream::error::OK) {
                    DSTREAM_WARN("dump pm failed!");
                }
            } else if (mod == kPMBackupString) {
                if(zk_dumper.DumpBackupPM() < dstream::error::OK) {
                    DSTREAM_WARN("dump backup pm failed!");
                }
            } else if (mod == kPNString) {
                if(zk_dumper.DumpPN() < dstream::error::OK) {
                    DSTREAM_WARN("dump pn failed!");
                }
            } else if (mod == kAppString) {
                dump_app = true;
            } else if (mod == kPEString) {
                dump_pe = true;
            } else if (mod == kConfigString) {
                if(zk_dumper.DumpConfig() < dstream::error::OK) {
                    DSTREAM_WARN("dump config failed!");
                }
            } else if (mod == kPNGroupString) {
                if(zk_dumper.DumpPNGroup() < dstream::error::OK) {
                    DSTREAM_WARN("dump pn group failed!");
                }
            } else if (mod == kPMMachineString) {
                if(zk_dumper.DumpPMMachines() < dstream::error::OK) {
                    DSTREAM_WARN("dump pm machines failed!");
                }
            } else if (mod == kPNMachineString) {
                if(zk_dumper.DumpPNMachines() < dstream::error::OK) {
                    DSTREAM_WARN("dump pn machines failed!");
                }
            } else {
                DSTREAM_WARN("unknown mod [%s], dump nothing.", mod.c_str());
            }
        }
        if(dump_app || dump_pe) {
            if(zk_dumper.DumpApp(app_id, dump_app, dump_pe) < dstream::error::OK) {
                DSTREAM_WARN("dump app/pe failed");
            }
        }
    } else  {
        std::cout << "-m [modname] must be provided" << std::endl;
        PrintUsage(argv[0]);
        return -1;
    }
    std::cout << zk_dumper.GetFormatJsonStr() <<std::endl;
    return 0;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
