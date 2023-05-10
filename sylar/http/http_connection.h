#ifndef __SYLAR_HTTP_CONNECTION_H__
#define __SYLAR_HTTP_CONNECTION_H__

#include "../socket_stream.h"
#include "http.h"

namespace sylar {
namespace http{

class HttpConnection : public SocketStream{
public:
    typedef std::shared_ptr<HttpConnection> ptr;
    HttpConnection(Socket::ptr sock, bool owern = true);
    HttpResponse::ptr recvResponse();
    int sendRequest(HttpRequest::ptr req);
};
    
}
}

#endif