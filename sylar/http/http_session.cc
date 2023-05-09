#include "http_session.h"
#include "http_parser.h"

namespace sylar {
namespace http{

HttpSession::HttpSession(Socket::ptr sock, bool owern)
    :SocketStream(sock, owern) {
    
}

HttpRequest::ptr HttpSession::recvRequest() {
    HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    // uint64_t buff_size = 100;
    std::shared_ptr<char> buffer(
        new char[buff_size], [](char* ptr) {
            delete[] ptr;
        });
    char* data = buffer.get();
    int offset = 0;
    do {
        // 在offset后面接着读数据
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            return nullptr;
        }
        // 当前已经读取的数据长度
        len += offset;
        // 解析缓冲区data中的数据
        // execute会将data向前移动nparse个字节，nparse为已经成功解析的字节数
        size_t nparse = parser->execute(data, len);
        if (parser->hasError()) {
            return nullptr;
        }
        // 此时data还剩下len - nparse个字节
        offset = len - nparse;
        // 缓冲区满了还没解析完
        if (offset == (int)buff_size) {
            return nullptr;
        }
        // 解析结束
        if (parser->isFinished()) {
            break;
        }
    } while (true);
    // 获得body的长度
    int64_t length = parser->getContentLength();
    if (length > 0) {
        std::string body;
        body.reserve(length);

        // 如果长度比缓冲区剩余的还大，将缓冲区全部加进来
        if (length >= offset) {
            body.append(data, offset);
        } else {
            // 否则将取length
            body.append(data, length);
        }

        length -= offset;
        // 缓冲区里的数据也不够，继续读取直到满足length
        if (length > 0) {
            if (readFixSize(&body[body.size()], length) <= 0) {
                return nullptr;
            }
        }
        // 设置body
        parser->getData()->setBody(body);
    }
    //返回解析完的HttpRequest
    return parser->getData();
}

int HttpSession::sendResponse(HttpRespons::ptr rsp) {
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
    
}
}