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
#ifndef JOINER_TASK_SINGLE_H
#define JOINER_TASK_SINGLE_H
#include <iostream>
#include <processelement/task.h>
#include <fstream>

#include "generic_operator/single_key_joiner.h"
#include "generic_operator/multi_key_joiner.h"
#include "storage_config.h"
#include "storage/dredis_sa.h"
#include "joiner.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

class JoinerTask : public Task {
public:
    FILE* fp;
    typedef dstream::generic_operator::error::Code ErrorCode;
    // ÅäÖÃÎÄ¼þÃû
    static const char* kConfigFileName;
    // Importer0 joinerÃû³Æ
    static const char* kImp0JoinerName;
    // Importer1 joinerÃû³Æ
    static const char* kImp1JoinerName;
    // Íâ²¿´æ´¢ÅäÖÃÎÄ¼þÃû³Æ
    static const char* kStorageConfigFileName;
    // ¶¨Òåtuple
    typedef ProtoTuple< Joiner > JoinerTuple;
    typedef DefaultTupleTypeInfo< JoinerTuple > JoinerTupleInfo;

    typedef ProtoTuple< Joiner > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    JoinerTask();
    ~JoinerTask();
    /**
    * @brief ³õÊ¼»¯joiner¶ÔÏó¡¢¶Áconf
    */
    virtual void init();
    void onTuples(const Tuples& tuples);
    virtual void close();

protected:
    bool JoinTuples(const ImporterTuple* imp0_tuple,
                    const ImporterTuple* imp1_tuple);

    static void Imp0TimeoutCallBack(const dstream::generic_operator::KeyType& key,
                                     const dstream::generic_operator::DataType& data,
                                     void* context);
    static void Imp1TimeoutCallBack(const dstream::generic_operator::KeyType& key,
                                     const dstream::generic_operator::DataType& data,
                                     void* context);

private:
    dstream::generic_operator::SingleKeyJoiner* imp0_joiner_;
    dstream::generic_operator::SingleKeyJoiner* imp1_joiner_;
    dstream::storage::DRedisSA* imp0_joiner_sa_;
    dstream::storage::DRedisSA* imp1_joiner_sa_;
    StorageConfig storage_config_;
};
#endif // JOINER_TASK_SINGLE_H
