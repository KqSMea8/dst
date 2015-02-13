/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSELEMENT_PE_TRANSPORTER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_PE_TRANSPORTER_H__ // NOLINT

#include "transporter/transporter_cli.h"

namespace dstream {
namespace processelement {

class PEFetchCli : public transporter::TPProxyCli {
public:
    explicit PEFetchCli(uint64_t id);
    ~PEFetchCli() {}
    // fetch data
    int FetchData(BufHandle** data_hdl,
                  int*        batch,
                  int*        size,
                  uint64_t*   ack);
protected:
    transporter::TPDataMsg* m_pmsg;
    transporter::TPDataMsg  m_msg;
    BufHandle*              m_hdl;
}; // class PEFetchCli

class PEForwardCli : public transporter::TPProxyCli {
public:
    explicit PEForwardCli(uint64_t id);
    ~PEForwardCli() {}
    // send  data
    int SendData(BufHandle* data_hdl,
                 int        batch,
                 int        size,
                 uint64_t*  ack);
protected:
    transporter::TPDataMsg* m_pmsg;
    transporter::TPDataMsg  m_msg;
    BufHandle*              m_hdl;
}; // class PEForwardCli

} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_PE_TRANSPORTER_H__ NOLINT
