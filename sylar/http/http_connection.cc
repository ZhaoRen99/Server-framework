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
        // ��offset������Ŷ�����
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }
        // ��ǰ�Ѿ���ȡ�����ݳ���
        len += offset;
        data[len] = '\0';
        // ����������data�е�����
        // execute�Ὣdata��ǰ�ƶ�nparse���ֽڣ�nparseΪ�Ѿ��ɹ��������ֽ���
        size_t nparse = parser->execute(data, len, false);
        if (parser->hasError()) {
            close();
            return nullptr;
        }
        // ��ʱdata��ʣ��len - nparse���ֽ�
        offset = len - nparse;
        // ���������˻�û������
        if (offset == (int)buff_size) {
            close();
            return nullptr;
        }
        // ��������
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
        // ���body�ĳ���
        int64_t length = parser->getContentLength();
        if (length > 0) {
            std::string body;
            body.resize(length);

            int len = 0;
            // ������ȱȻ�����ʣ��Ļ��󣬽�������ȫ���ӽ���
            if (length >= offset) {
                mempcpy(&body[0], data, offset);
                len = offset;
            } else {
                // ����ȡlength
                memcpy(&body[0], data, length);
                len = length;
            }

            length -= offset;
            // �������������Ҳ������������ȡֱ������length
            if (length > 0) {
                if (readFixSize(&body[len], length) <= 0) {
                    close();
                    return nullptr;
                }
            }
            // ����body
            parser->getData()->setBody(body);
        }
    }
    //���ؽ������HttpResponse
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