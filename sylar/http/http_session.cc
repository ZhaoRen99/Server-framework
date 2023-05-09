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
        // ��offset������Ŷ�����
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            return nullptr;
        }
        // ��ǰ�Ѿ���ȡ�����ݳ���
        len += offset;
        // ����������data�е�����
        // execute�Ὣdata��ǰ�ƶ�nparse���ֽڣ�nparseΪ�Ѿ��ɹ��������ֽ���
        size_t nparse = parser->execute(data, len);
        if (parser->hasError()) {
            return nullptr;
        }
        // ��ʱdata��ʣ��len - nparse���ֽ�
        offset = len - nparse;
        // ���������˻�û������
        if (offset == (int)buff_size) {
            return nullptr;
        }
        // ��������
        if (parser->isFinished()) {
            break;
        }
    } while (true);
    // ���body�ĳ���
    int64_t length = parser->getContentLength();
    if (length > 0) {
        std::string body;
        body.reserve(length);

        // ������ȱȻ�����ʣ��Ļ��󣬽�������ȫ���ӽ���
        if (length >= offset) {
            body.append(data, offset);
        } else {
            // ����ȡlength
            body.append(data, length);
        }

        length -= offset;
        // �������������Ҳ������������ȡֱ������length
        if (length > 0) {
            if (readFixSize(&body[body.size()], length) <= 0) {
                return nullptr;
            }
        }
        // ����body
        parser->getData()->setBody(body);
    }
    //���ؽ������HttpRequest
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