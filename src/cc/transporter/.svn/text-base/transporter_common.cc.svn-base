#include "transporter/transporter_common.h"
#include "common/proto/pub_sub.pb.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace transporter {

const int     kEspStartPort = 9700;
const int     kEspStopPort  = 9900;

CESPSystem    g_esp;
volatile bool g_init         = false;
uint16_t      g_esp_port     = kEspStartPort;
uint32_t      g_precision_ms = 1;   // 1ms

int     g_kKylinExecWorkers         = 6;
int     g_kKylinNetWorkers          = 2;
int     g_kKylinDiskWorkers         = 1;
int     g_kKylinBufSmallTranBufNum  = 1024 * 512;
int     g_kKylinBufSmallTranBufSize = 1024 * 16;
int     g_kKylinBufBigTranBufNum    = 32; // num of big buf in one block,
// each block is 64MB,
// so each buf size is 64MB/32 = 2MB
double  g_kKylinBufLowMark          = 0.90f;
int     g_kKylinWriteWaterMark      = 80; // 80%

void TPSetTranBuf(int nSmallBuf, int smallBufSize, int nBigBuf, int wWaterMark) {
    g_kKylinBufSmallTranBufNum = nSmallBuf;
    g_kKylinBufSmallTranBufSize = smallBufSize;
    g_kKylinBufBigTranBufNum = nBigBuf;
    g_kKylinWriteWaterMark = wWaterMark;
}

bool TPInitKylin() {
    if (!g_init) {
        SetTranBuf(g_kKylinBufSmallTranBufNum,  g_kKylinBufBigTranBufNum,
                   g_kKylinBufSmallTranBufSize, g_kKylinBufLowMark);
        if (!g_esp.Init(g_kKylinExecWorkers, g_kKylinNetWorkers,
                        g_kKylinDiskWorkers, NULL, g_precision_ms)) {
            return false;
        }
        if (0 != g_esp_port) {
            bool bind_succ  = false;
            for (g_esp_port  = kEspStartPort;
                 g_esp_port != kEspStopPort;
                 g_esp_port++) {
                if (INVALID_ESPNODE != g_esp.Bind(g_esp_port)) {
                    bind_succ = true;
                    break;
                }
            }
            if (!bind_succ) {
                return false;
            }
        }
        if (!g_esp.Run()) {
            return false;
        }
        // KLSetLog(stdout, 6, NULL);
        g_init = true;
    }
    DSTREAM_INFO("Start kylin OK, SmallBufSize(%d), "
            "SmallBufNum(%d), "
            "BigBufNum(%d), "
            "WriteWaterMark(%d%%), "
            "ESP Port(%d).",
            g_kKylinBufSmallTranBufSize,
            g_kKylinBufSmallTranBufNum,
            g_kKylinBufBigTranBufNum,
            g_kKylinWriteWaterMark,
            g_esp_port);
    return true;
}

void TPStopKylin() {
    if (g_init) {
        g_esp.StopNetwork();
        g_esp.Stop(true);
        g_init = false;
    }
}

void TPPrintStatus(int intv) {
    (void)intv;

    printf("=========Status========\n");
    g_esp.Print();
    g_pBufPoolManager->Print((uint32) - 1);
}

static void Proc(Job* job) {
    AsyncContext* ctx = reinterpret_cast<AsyncContext*>(job);
    CAsyncClient* pClient = ctx->pClient;

    ctx->fProc = NULL;
    pClient->OnCompletion(ctx);
    pClient->Release();
}

APF_ERROR QueueDiskExec(int naction, CAsyncClient* pclient, AsyncContext* ctx, int nwhich) {
    DSTREAM_UNUSED_VAL(nwhich);
    ctx->fProc   = Proc;
    ctx->nAction = naction;
    ctx->pClient = pclient;
    ctx->pClient->AddRef();
    g_pDiskMan->_QueueJob(ctx, pclient->GetAsyncId() % g_kKylinDiskWorkers);
    return APFE_OK;
}

ErrCode DumpTuplesFromBufHdl(IN  BufHandle* hdl,
                             IN  int size,
                             IN  int batch,
                             OUT std::string* s) {
    CBufReadStream brs(hdl, NULL);
    return DumpTuplesFromBufStream(&brs, size, batch, s);
}

ErrCode DumpTuplesFromBufHdlWithMsgHead(IN  BufHandle* hdl,
                                        IN  int size,
                                        IN  int batch,
                                        OUT std::string* s) {
    CBufReadStream brs(hdl, NULL);
    UNLIKELY_IF(!brs.Seek(static_cast<int>(sizeof(TPDataMsg)))) {
        return error::DATA_CORRUPTED;
    }
    return DumpTuplesFromBufStream(&brs, size, batch, s);
}

ErrCode DumpTuplesFromBufStream(IN  CBufReadStream* brs,
                                IN  int size,
                                IN  int batch,
                                OUT std::string* s) {
    VERIFY_OR_RETURN(NULL != s, error::BAD_ARGUMENT);
    char tag[kMaxTagSize];
    *s += "[size=";
    StringAppendNumber(s, size);
    *s += ", batch=";
    StringAppendNumber(s, batch);
    *s += ":[";
    for (int i = 0; i < batch; ++i) {
        int    tuple_len;
        int    tag_len;
        uint64 hash;
        UNLIKELY_IF(!brs->GetObject<int>(&tuple_len) ||
                    !brs->GetObject<uint64>(&hash)   ||
                    !brs->GetObject<int>(&tag_len)   ||
                    (tag_len >= kMaxTagSize)  ||
                    (tag_len != brs->GetBuffer(tag, tag_len)) ||
                    !brs->Seek(tuple_len - static_cast<int>(sizeof(uint64)) -
                               static_cast<int>(sizeof(int)) - tag_len)) {
            return error::DATA_CORRUPTED;
        }
        if (0 != i) {
            *s += ", ";
        }
        *s += "[len=";
        StringAppendNumber(s, tuple_len);
        *s += ", hash=";
        StringAppendNumber(s, hash);
        s->append(", tag=").append(tag);
        *s += "]";
    }
    *s += "]]";
    return error::OK;
}

} // namespace transporter
} // namespace dstream
