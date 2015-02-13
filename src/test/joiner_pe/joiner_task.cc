/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  QA test for joiner pe 
 *
 */
#include "joiner_task.h"

template <class T>
string NumToString(const T& n) {
    ostringstream out;
    out << n;
    return out.str();
}

int StringToNum(const string& str) {
    return atoi(str.c_str());
}

const char* JoinerTask::kConfigFileName = "joiner_task.cfg.xml";
const char* JoinerTask::kImp0JoinerName = "imp0_joiner";
const char* JoinerTask::kImp1JoinerName = "imp1_joiner";
const char* JoinerTask::kStorageConfigFileName = "joiner_task_storage.cfg.xml";

JoinerTask::JoinerTask(): fp(NULL), imp0_joiner_(NULL), imp1_joiner_(NULL), imp0_joiner_sa_(NULL),
    imp1_joiner_sa_(NULL) {
    registerTupleTypeInfo("importer0", new ImporterTupleInfo);
    registerTupleTypeInfo("importer1", new ImporterTupleInfo);

    registerTupleTypeInfo("joiner_task", new JoinerTupleInfo);
    fp = fopen("joiner_log", "w+");
}
JoinerTask::~JoinerTask() {
    if (imp0_joiner_) {
        delete imp0_joiner_;
        imp0_joiner_ = NULL;
    }
    if (imp1_joiner_) {
        delete imp1_joiner_;
        imp1_joiner_ = NULL;
    }
    if (imp1_joiner_sa_) {
        delete imp1_joiner_sa_;
        imp1_joiner_sa_ = NULL;
    }
    if (imp1_joiner_sa_) {
        delete imp1_joiner_sa_;
        imp1_joiner_sa_ = NULL;
    }
    fclose(fp);
}
void JoinerTask::init() {
    // new everything
    imp0_joiner_ = new (std::nothrow) dstream::generic_operator::MultiKeyJoiner();
    imp1_joiner_ = new (std::nothrow) dstream::generic_operator::SingleKeyJoiner();

    imp0_joiner_sa_ = new (std::nothrow) dstream::storage::DRedisSA();
    imp1_joiner_sa_ = new (std::nothrow) dstream::storage::DRedisSA();

    if (NULL == imp0_joiner_ || NULL == imp1_joiner_) {
        DSTREAM_ERROR("new joiner FAIL");
    }
    if (NULL == imp0_joiner_sa_ || NULL == imp1_joiner_sa_) {
        DSTREAM_ERROR("alloc joiner storage FAIL");
    }
    // init storage
    bool use_storage = false;
    dstream::config::Config s_conf;
    if (s_conf.ReadConfig(JoinerTask::kStorageConfigFileName) == dstream::error::OK) {
        use_storage = true;
    }
    int storage_ret = dstream::storage::OK;
    bool read_s_conf;
    string select_service_name;
    if (use_storage) {
        // prepare storage::StoreAccessor for imp0
        read_s_conf = storage_config_.CheckAndReadConfig(s_conf, kImp0JoinerName);
        select_service_name = storage_config_.GetMyStorageService(GetStoreID(id()));
        storage_ret = imp0_joiner_sa_->Init(storage_config_.service_loc_list,
                                            select_service_name,
                                            static_cast<int>(storage_config_.retry),
                                            static_cast<int>(storage_config_.timeout),
                                            storage_config_.service_token);
        if (!read_s_conf || select_service_name.empty() || storage_ret != dstream::storage::OK) {
            DSTREAM_ERROR("init storage fail");
            delete imp0_joiner_sa_;
            imp0_joiner_sa_ = NULL;
        }
        // prepare storage::StoreAccessor for imp1
        read_s_conf = storage_config_.CheckAndReadConfig(s_conf, kImp1JoinerName);
        select_service_name = storage_config_.GetMyStorageService(GetStoreID(id()));
        storage_ret = imp1_joiner_sa_->Init(storage_config_.service_loc_list,
                                            select_service_name,
                                            static_cast<int>(storage_config_.retry),
                                            static_cast<int>(storage_config_.timeout),
                                            storage_config_.service_token);
        if (!read_s_conf || select_service_name.empty() || storage_ret != dstream::storage::OK) {
            DSTREAM_ERROR("init storage fail");
            delete imp1_joiner_sa_;
            imp1_joiner_sa_ = NULL;
        }
    }


    dstream::config::Config conf;
    if (dstream::error::OK != conf.ReadConfig(JoinerTask::kConfigFileName)) {
        DSTREAM_ERROR("Read joiner config FAIL");
    }
    // init imp0 log joiner
    string imp0_joiner_id(kImp0JoinerName);
    imp0_joiner_id += "_" + NumToString<uint64_t>(id());
    ErrorCode err_code = dstream::generic_operator::error::OK;
    err_code = imp0_joiner_->Initialize(conf, imp0_joiner_id, kImp0JoinerName, imp0_joiner_sa_);
    if (err_code < dstream::generic_operator::error::OK) {
        DSTREAM_ERROR("fail to init imp0_joiner");
    }
    imp0_joiner_->RegisterDataTimeOutCallBack(Imp0TimeoutCallBack, this);
    // init imp1 log joiner
    string imp1_joiner_id(kImp1JoinerName);
    imp1_joiner_id += "_" + NumToString<uint64_t>(id());
    err_code = imp1_joiner_->Initialize(conf, imp1_joiner_id, kImp1JoinerName, imp1_joiner_sa_);
    if (err_code < dstream::generic_operator::error::OK) {
        DSTREAM_ERROR("fail to init imp1_joiner");
    }
    imp1_joiner_->RegisterDataTimeOutCallBack(Imp1TimeoutCallBack, this);
}
void JoinerTask::onTuples(const Tuples& tuples) {
    for (size_t i = 0; i < tuples.size(); i++) {
        ErrorCode err_code = dstream::generic_operator::error::OK;
        const Tuple* tuple = tuples[i];
        if (NULL == tuple) {
            DSTREAM_ERROR("[%s]tuple is NULL", __FUNCTION__);
            continue;
        }
        if (tuple->tag() == "importer0") {
            const ImporterTuple* imp0_tuple_ptr = static_cast<const ImporterTuple*>(tuple);
            const dstream::generic_operator::KeyType join_key_str = NumToString<uint64_t>(imp0_tuple_ptr->jk());

            DSTREAM_INFO("[%s][MARK]recv importer0", __FUNCTION__);
            dstream::generic_operator::DataType join_data;
            err_code = imp1_joiner_->Join(join_key_str, true,
                                          &join_data); // ×ö²éÕÒ£¬ÕÒµ½ºó²»É¾³ýÊý¾Ý(single_hash_table)
            if (dstream::generic_operator::error::OK == err_code) {
                DSTREAM_INFO("[%s][MARK]recv importer0 && join success", __FUNCTION__);
                ImporterTuple importer1;
                if (!importer1.ParseFromString(join_data)) {
                    DSTREAM_ERROR("[%s]fail to parse importer1", __FUNCTION__);
                } else {
                    JoinTuples(imp0_tuple_ptr, &importer1);
                }
            } else if (dstream::generic_operator::error::JOINER_KEY_NOT_FOUND == err_code) {
                DSTREAM_INFO("[%s][MARK]recv importer0 && join fail && record", __FUNCTION__);
                uint32_t tuple_time = static_cast<uint32_t>(imp0_tuple_ptr->timestamp());
                dstream::generic_operator::DataType join_data;
                if (!imp0_tuple_ptr->SerializeToString(&join_data)) {
                    DSTREAM_ERROR("[%s]imp0 fail to serialize to string", __FUNCTION__);
                }
                err_code = imp0_joiner_->Record(tuple_time, false, join_key_str,
                                                join_data); // multi_hash_table,²»Ìæ»»£¬±£Áôµ¥key¶Ô¶àvalue
                if (err_code < dstream::generic_operator::error::OK) {
                    DSTREAM_WARN("[%s]imp0 record error,time(%u),key(%s)", __FUNCTION__, tuple_time,
                                 join_key_str.c_str());
                    if (err_code == dstream::generic_operator::error::JOINER_GET_WINDOW_FAIL) {
                        DSTREAM_WARN("[%s]send too old importer0_tuple,time(%u),key(%s)", __FUNCTION__, tuple_time,
                                     join_key_str.c_str());
                    }
                }
            } else {
                DSTREAM_ERROR("imp0_tuple join error");
            }
            // end of tag==importer0
        } else if (tuple->tag() == "importer1") {
            const ImporterTuple* imp1_tuple_ptr = static_cast<const ImporterTuple*>(tuple);
            const dstream::generic_operator::KeyType join_key_str = NumToString<uint64_t>(imp1_tuple_ptr->jk());
            DSTREAM_INFO("[%s][MARK]recv importer1", __FUNCTION__);

            vector<dstream::generic_operator::DataType> join_data_list;
            err_code = imp0_joiner_->Join(join_key_str, false,
                                          &join_data_list); // ×ö²é±í£¬ÕÒµ½ºóÉ¾³ýÊý¾Ý(multi_hash_table)
            if (dstream::generic_operator::error::OK == err_code) {
                DSTREAM_INFO("[%s][MARK]recv importer1 && join success", __FUNCTION__);
                vector<dstream::generic_operator::DataType>::const_iterator iter;
                for (iter = join_data_list.begin(); iter != join_data_list.end(); ++iter) {
                    ImporterTuple importer0;
                    if (!importer0.ParseFromString(*iter)) {
                        DSTREAM_ERROR("[%s]fail to parse importer1", __FUNCTION__);
                    } else {
                        JoinTuples(&importer0, imp1_tuple_ptr);
                    }
                }
            } else if (dstream::generic_operator::error::JOINER_KEY_NOT_FOUND == err_code) {
                DSTREAM_INFO("[%s][MARK]recv importer1 && join fail && record", __FUNCTION__);
                uint32_t tuple_time = static_cast<uint32_t>(imp1_tuple_ptr->timestamp());
                dstream::generic_operator::DataType join_data;
                if (!imp1_tuple_ptr->SerializeToString(&join_data)) {
                    DSTREAM_ERROR("[%s]imp1 fail to serialize to string", __FUNCTION__);
                }
                err_code = imp1_joiner_->Record(tuple_time, true, join_key_str,
                                                join_data); // single_hash_table,±ØÐëÌæ»»£¬Ö»±£Áôµ¥key,µ«value
                if (err_code < dstream::generic_operator::error::OK) {
                    DSTREAM_WARN("[%s]imp1 record error,time(%u),key(%s)", __FUNCTION__, tuple_time,
                                 join_key_str.c_str());
                    if (err_code == dstream::generic_operator::error::JOINER_GET_WINDOW_FAIL) {
                        DSTREAM_WARN("[%s]send too old importer1_tuple,time(%u),key(%s)",
                                     __FUNCTION__, tuple_time, join_key_str.c_str());
                    }
                }
            } else {
                DSTREAM_ERROR("imp1_tuple join error");
            }
        } // end of tag==importer1
    } // end of for
}
void JoinerTask::close() {
}
bool JoinerTask::JoinTuples(const ImporterTuple* imp0_tuple, const ImporterTuple* imp1_tuple) {
    if (imp0_tuple == NULL || imp1_tuple == NULL) {
        DSTREAM_ERROR("[%s]imp0_tuple == NULL or imp1_tuple == NULL", __FUNCTION__);
    }
    JoinerTuple* joiner_tuple = static_cast< JoinerTuple* >(emit("joiner_task"));
    if (joiner_tuple == NULL) {
        DSTREAM_ERROR("[%s]allocate joiner_task error", __FUNCTION__);
    }
    joiner_tuple->set_jk(imp0_tuple->jk());
    joiner_tuple->set_timestamp(imp0_tuple->timestamp());
    string info_str = "";
    info_str = imp0_tuple->info() + " " + imp1_tuple->info();
    joiner_tuple->set_info(info_str);
    fprintf(fp, "join_key:%lu    timestamp:%lu   info:%s\n", imp0_tuple->jk(), imp0_tuple->timestamp(),
            info_str.c_str());
    fflush(fp);
}
void JoinerTask::Imp0TimeoutCallBack(const dstream::generic_operator::KeyType& key,
                                     const dstream::generic_operator::DataType& data,
                                     void* context) {
    DSTREAM_WARN("[%s] importer0 log tuple with key(%s) time out", __FUNCTION__, key.c_str());
}
void JoinerTask::Imp1TimeoutCallBack(const dstream::generic_operator::KeyType& key,
                                     const dstream::generic_operator::DataType& data,
                                     void* context) {
    DSTREAM_WARN("[%s] importer1 log tuple with key(%s) time out", __FUNCTION__, key.c_str());
}


// int main(int argc,char* argv[])
// {
//     setenv(Task::kEnvKeyProcessElementName, "joiner_task", 1);
//     JoinerTask task;
//     task.run(false);
// }
