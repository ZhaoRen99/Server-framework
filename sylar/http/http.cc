#include "http.h"

namespace sylar {
namespace http{

HttpMethod StringToHttpMethod(const std::string& m){
#define XX(num, name, string) \
    if(strcmp(#string, m.c_str()) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharsToHttpMethod(const char* m) {
#define XX(num, name, string) \
    if(strcmp(#string, m) == 0){ \
        return HttpMethod::name; \
    } 
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID_METHOD;
}

static const char* s_method_string[] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const char* HttpMethodToString(const HttpMethod& m) {
    size_t idx = static_cast<size_t>(m);
    if (idx >= sizeof(s_method_string) / sizeof(s_method_string[0])) {
        return "<unknow>";
    }
    return s_method_string[idx];
}

const char* HttpStatusToString(const HttpStatus& s) {
    switch (s) {
#define XX(code, name, msg) \
        case HttpStatus::name: \
            return #msg;
        HTTP_STATUS_MAP(XX)
#undef XX
        default:
            return "<unknow>";
    }
}

bool CaseInsensitiveLess::operator()(const std::string& lhs
    , const std::string& rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HttpRequest::HttpRequest(uint8_t version, bool close)
    :m_method(HttpMethod::GET)
    ,m_version(version)
    ,m_close(close)
    ,m_path("/") {
}

std::string HttpRequest::getHeader(const std::string& key, const std::string& def) {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string& key, const std::string& def) {
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HttpRequest::getCookie(const std::string& key, const std::string& def) {
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}

void HttpRequest::setHeader(const std::string& key, const std::string& val) {
    m_headers[key] = val;
}
void HttpRequest::setParam(const std::string& key, const std::string& val) {
    m_params[key] = val;
}

void HttpRequest::setCookie(const std::string& key, const std::string& val) {
    m_cookies[key] = val;
}

void HttpRequest::delHeader(const std::string& key) {
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string& key) {
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string& key) {
    m_cookies.erase(key);
}

bool HttpRequest::hasHeader(const std::string& key, std::string* val) {
    auto it = m_headers.find(key);
    if (it == m_headers.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasParam(const std::string& key, std::string* val) {
    auto it = m_params.find(key);
    if (it == m_params.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasCookie(const std::string& key, std::string* val)  {
        auto it = m_cookies.find(key);
    if (it == m_cookies.end()) {
        return false;
    }
    if (val) {
        *val = it->second;
    }
    return true;
}

std::ostream& HttpRequest::dump(std::ostream& os) {
/*
    GET /URI HTTP/1.1
    Host: www.sylar.top


*/
    os << HttpMethodToString(m_method)
        << " "
        << m_path
        << (m_query.empty() ? "" : "?")
        << m_query
        << (m_fragment.empty() ? "" : "#")
        << m_fragment
        << " "
        << "HTTP/"
        << (uint32_t)(m_version >> 4)
        << "."
        << (uint32_t)(m_version & 0x0F)
        << "\r\n";
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
    for (auto& i : m_headers) {
        if (strcasecmp(i.first.c_str(), "connection") == 0) continue;
        os << i.first << ": " << i.second << "\r\n";
    }

    if (!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n"
            << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

HttpRespons::HttpRespons(uint8_t version, bool close)
    :m_status(HttpStatus::OK)
    ,m_version(version)
    ,m_close(close) {

}

std::string HttpRespons::getHeader(const std::string& key, const std::string& def) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

void HttpRespons::setHeader(const std::string& key, const std::string& value) {
    m_headers[key] = value;
}

void HttpRespons::delHeader(const std::string& key) {
    m_headers.erase(key);
}

std::ostream& HttpRespons::dump(std::ostream& os) {
/*
    HTTP/1.1 301 Moved Permanently
    Server: nginx/1.12.2
    Date: Thu, 20 Apr 2023 12:35:50 GMT
    Content-Type: text/html
    Content-Length: 185
    Location: http://www.sylar.top/blog/index.php
    Connection: keep-alive
    
*/
    os << "HTTP/"
        << (uint32_t)(m_version >> 4)
        << "."
        << (uint32_t)(m_version & 0x0F)
        << " "
        << static_cast<uint32_t>(m_status)
        << " "
        << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
        << "\r\n";

    for (auto& i : m_headers) {
        if (strcasecmp(i.first.c_str(), "connection") == 0) continue;
        os << i.first << ": " << i.second << "\r\n";
    }
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    if (!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n"
            << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

}
}
