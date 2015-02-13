/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTER_CLI_H__
#define __DSTREAM_SRC_CC_TRANSPORTER_CLI_H__

#include <string>
#include <vector>
#include "common/config_const.h"
#include "common/id.h"
#include "common/utils.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace transporter {

/**
* @brief    基于ESPClient的客户端类,指定一个server地址，可以收发任意消息.
*           消息接口参考CESPClient.
* @author   konghui, konghui@baidu.com
* @date     2013-01-18
* @notice   - 非线程安全
            - 通过指针使用本类
*/

class TPCli: public CESPClient {
public:
    TPCli();
    int  Init(ESPADDRESS server_addr, ESPSTUB local_stub = ESP_ANYSTUB);
    void Destroy();

protected:
    virtual ~TPCli() {} // forbbiden distructor
    virtual bool OnMsg(ESPMessage* msg);

protected:
    enum TCStatus {
        TC_NOT_RUNNING = 0,
        TC_RUNNING     = 1
    };

protected:
    uint64_t        m_running;
    ESPNODE         m_node;
    ESPADDRESS      m_addr;
}; // class TPCli

/**
* @brief    特化的代理类，首先向上游query stub, 然后重定向到新的stub通信
* @author   konghui, konghui@baidu.com
* @date     2013-01-18
* @notice   非线程安全
*/
class TPProxyCli {
public:
    TPProxyCli(TPType type, uint64_t id, uint64_t to_id);
    ~TPProxyCli();

    int Init(const std::string& to_ipport,
             int timeout_ms = config_const::kDataFetchTimeoutMs);
    int SyncReq(uint32 msg, IN BufHandle* arg, OUT BufHandle** ack_hdl);

protected:
    int _QueryStub();

protected:
    TPType          m_type;
    uint64_t        m_id;
    uint64_t        m_to_id;
    std::string     m_to_ipport;
    ESPNODE         m_to_node;
    ESPSTUB         m_to_stub;
    TPCli*          m_cli;
    int             m_timeout;
};

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTER_CLI_H__
