#include <signal.h>
#include "common/logger.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/dstream_httpd.h"
namespace dstream {
namespace httpd {

const static char* kHttpResponseString = "HTTP/1.1 200 OK\r\n"
                                         "Server: mongoose\r\n"
                                         "Cache: no-cache\r\n"
                                         "Content-Type: text/plain\r\n"
                                         "Content-Length: %d\r\n"
                                         "\r\n"
                                         "%s";

static dstream::error::Code GetOutputData(const HttpdResponder* responder,
                                          const std::string& category, 
                                          std::string * data_return) {
    assert(NULL != responder && NULL != data_return);
    RequestMap request_map = responder->GetRequestMap();
    RequestMapIter iter;
    std::string ret_page = "";
    iter = request_map.find(category);
    if (iter != request_map.end()) {
        dstream::error::Code err_code = iter->second(&ret_page);
        data_return->assign(ret_page);
        return err_code;
    }
    return dstream::error::NO_SUCH_REQUEST_URL;
}

// in begin request callback, return 1 means, you would take charge of the http server request.
// return 0, mongoose would take the default for you, which is not we desired.
static int BeginRequestHandler(struct mg_connection* mg_conn) {
    if (NULL == mg_conn) {
        DSTREAM_WARN("httpd meet an unexpected error, mg_conn is a null value");
        return 0;
    }
    std::string err_str;
    const struct mg_request_info *request_info = mg_get_request_info(mg_conn);
    if(request_info != NULL) {
        HttpdResponder *responder;
        if(NULL != request_info->user_data) {
            responder = reinterpret_cast<HttpdResponder*>(request_info->user_data);
        } else {
            err_str = "no httpd responder, can not handler request";
            DSTREAM_WARN("%s", err_str.c_str());
            mg_printf(mg_conn, 
                      kHttpResponseString,
                      err_str.length(),
                      err_str.c_str());
        }
        std::string ret_buf;
        std::string req_str(request_info->uri); 
        dstream::error::Code code_ret = GetOutputData(responder, req_str, &ret_buf);
        if(code_ret < error::OK) {
            err_str = "could not get any data for request:[";
            err_str += req_str;
            err_str += "], error:";
            err_str += error::ErrStr(code_ret);
            DSTREAM_WARN("%s", err_str.c_str());

            mg_printf(mg_conn, 
                      kHttpResponseString,
                      err_str.length(),
                      err_str.c_str());
        } else {
            mg_printf(mg_conn, 
                      kHttpResponseString,
                      ret_buf.length(),
                      ret_buf.c_str());
        }
        return 1;
    } else {
        err_str = "could not get request info";
        DSTREAM_WARN("%s", err_str.c_str());
        mg_printf(mg_conn, 
                  kHttpResponseString,
                  err_str.length(),
                  err_str.c_str());
    }
    return 1;
}

int32_t HttpdStart(const std::string &listen_port,
                   void **context, 
                   HttpdResponder *responder) {
    assert(NULL != context && NULL != responder);
    const char * kListenPortString = "listening_ports";
    const char * kThreadNumString = "num_threads";
    const char * options[5];
    options[0] = kListenPortString;
    options[1] = listen_port.c_str();
    options[2] = kThreadNumString;
    options[3] = strdup("1");
    options[4] = NULL;
    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = BeginRequestHandler;
    struct mg_context * httpd_context = mg_start(&callbacks,
                                                 reinterpret_cast<void *>(responder),
                                                 options);
    if(httpd_context == NULL) {
        DSTREAM_WARN("fail to start httpd server, perhaps the port[%s] is occupied",
                     listen_port.c_str());
        return dstream::error::HTTPD_FAILED;
    }
    signal(SIGCHLD, SIG_DFL); // restore SIGCHLD
    *context = (void *)httpd_context;
    DSTREAM_INFO("start httpd server on port: %s", listen_port.c_str());
    return dstream::error::OK;
}

void HttpdStop(void * httpd_context) {
    if(NULL != httpd_context) {
        mg_stop(reinterpret_cast<struct mg_context *>(httpd_context));
        DSTREAM_INFO("stop httpd server");
    } else {
        DSTREAM_WARN("httpd context is a null value.");
    }
}

} // namespace httpd 
} // namespace dstream
