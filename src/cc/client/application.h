/**
* @file   application.h
* @brief    dstream application operator.
* @author   konghui, konghui@baidu.com
* @version
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_CLIENT_APPLICATION_H__ //NOLINT
#define __DSTREAM_CC_CLIENT_APPLICATION_H__ //NOLINT

#include <common/config.h>
#include <common/proto/application.pb.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace dstream {
namespace client {

struct Dag {
    struct PVertex {
        std::string     name;
        ProcessorRole   role;
        int32_t         input_num;

        PVertex(): input_num(0) {}
    };
    class VerCompare {
    public:
        bool operator() (const PVertex& p1, const PVertex& p2) {
            return p1.name < p2.name;
        }
    };
    typedef std::set<std::string>                 PInput;
    typedef std::map<PVertex, PInput, VerCompare> AdjacencMatrix;

    AdjacencMatrix m_vec;
};

enum {
    ADD_IMPORTER = 0,
    DEL_IMPORTER,
};

enum UPDATE_FLAG {
    UPDATE = 0,
    ROLLBACK,
};

typedef std::vector<std::string> VectorString;
typedef std::vector<std::string>::iterator VectorStringIterator;

int32_t InitCfg(const std::string& app_dir, bool sub_tree, bool dyn_importer,
                bool update_processor, config::Config* cfg);
int32_t GetAppName(const std::string& app_dir, bool sub_tree, bool dyn_importer,
                   bool update_processor, std::string* app_name);
int32_t InitApp(const std::string& app_dir,
                bool check_processor_dir,
                bool sub_tree,
                bool dyn_importer,
                bool update_processor,
                Application *app);
int32_t MergeSubTreeIntoApp(const Application& sub_tree, Application* old_tree);
int32_t DelSubTreeFromApp(const Application& sub_tree, Application* new_tree);
int32_t MergeImporterIntoApp(const Application& importer, Application* old_tree);
int32_t DelImporterFromApp(const Application& importer, Application* new_tree);
void ClearOldTag(const uint32_t old_processor_num,
                 const std::set<std::string>& importer_processors,
                 const std::set<std::string>& subscribe_importer_processors,
                 Topology* old_processor_set);
int32_t UpdateParallelismFromApp(const Application& update, Application* new_tree);
int32_t CheckTopology(const Application& app,
                      bool check_upstream_existence = true);
int32_t CheckImporter(const Application* old_tree, const Application& importer, bool add_or_delete);
int32_t BuildApp(const std::string app_dir, Application* app);
int64_t ConstructProcessorID(const Topology* topo);
int32_t GenerateUploadFile(const std::string path, VectorString* to_be_upload);


} // namespace client
} // namespace dstream
#endif // __DSTREAM_CC_CLIENT_APPLICATION_H__ NOLINT
