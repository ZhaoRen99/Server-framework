#include "http_connection.h"
#include "http_parser.h"
#include "../log.h"

namespace sylar {
namespace http{
static SYLAR__SYSTEM__LOG(g_logger);

HttpConnection::HttpConnection(Socket::ptr sock, bool owern)
    :SocketStream(sock, owern) {
    
}

HttpResponse::ptr HttpConnection::recvResponse() {
    HttpResponseParser::ptr parser(new HttpResponseParser);
    uint64_t buff_size = HttpResponseParser::GetHttpResponseBufferSize();
    // uint64_t buff_size = 8 * 1024;
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
            close();
            return nullptr;
        }
        // 当前已经读取的数据长度
        len += offset;
        data[len] = '\0';
        // 解析缓冲区data中的数据
        // execute会将data向前移动nparse个字节，nparse为已经成功解析的字节数
        size_t nparse = parser->execute(data, len, false);
        if (parser->hasError()) {
            close();
            return nullptr;
        }
        // 此时data还剩下len - nparse个字节
        offset = len - nparse;
        // 缓冲区满了还没解析完
        if (offset == (int)buff_size) {
            close();
            return nullptr;
        }
        // 解析结束
        if (parser->isFinished()) {
            break;
        }
    } while (true);
    auto& client_parser = parser->getParser();
    if (client_parser.chunked) {
        std::string body;
        int len = offset;
        do {
            do {
                int rt = read(data + len, buff_size - len);
                if (rt <= 0) {
                    close();
                    return nullptr;
                }
                len += rt;
                data[len] = '\0';
                size_t nparser = parser->execute(data, len, true);
                if (parser->hasError()) {
                    close();
                    return nullptr;
                }
                len -= nparser;
                if (len == (int)buff_size) {
                    close();
                    return nullptr;
                }
            } while (!parser->isFinished());
            len -= 2;
            if (client_parser.content_len <= len) {
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len, len - client_parser.content_len);
                len -= client_parser.content_len;
            } else {
                body.append(data, len);
                int left = client_parser.content_len - len;
                while (left > 0) {
                    int rt = read(data, left > (int)buff_size ? (int)buff_size : left);
                    if (rt <= 0) {
                        close();
                        return nullptr;
                    }
                    body.append(data, rt);
                    left -= rt;
                }
                len = 0;
            }
        } while (!client_parser.chunks_done); 
        parser->getData()->setBody(body);
    } else {
        // 获得body的长度
        int64_t length = parser->getContentLength();
        if (length > 0) {
            std::string body;
            body.resize(length);

            int len = 0;
            // 如果长度比缓冲区剩余的还大，将缓冲区全部加进来
            if (length >= offset) {
                mempcpy(&body[0], data, offset);
                len = offset;
            } else {
                // 否则将取length
                memcpy(&body[0], data, length);
                len = length;
            }

            length -= offset;
            // 缓冲区里的数据也不够，继续读取直到满足length
            if (length > 0) {
                if (readFixSize(&body[len], length) <= 0) {
                    close();
                    return nullptr;
                }
            }
            // 设置body
            parser->getData()->setBody(body);
        }
    }
    //返回解析完的HttpResponse
    return parser->getData();
}

int HttpConnection::sendRequest(HttpRequest::ptr req) {
    std::stringstream ss;
    ss << *req;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}
    
}
}