#ifndef __SYLAR_FDMANAGER_H_
#define __SYLAR_FDMANAGER_H_

#include <memory>

#include "mutex"
#include "iomanager.h"
#include "singleton.h"

namespace sylar {

class FdCtx : std::enable_shared_from_this<FdCtx> {
public:
    typedef std::shared_ptr<FdCtx> ptr;
    FdCtx(int fd);
    ~FdCtx();

    bool init();
    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClosed() const { return m_isClosed; }

    void setUserNonblock(bool v) { m_usrNonblock = v; }
    bool getUserNonblock() const { return m_usrNonblock; }

    void setSysNonblock(bool v) { m_sysNonblock = v; }
    bool getSysNonblock() const { return m_sysNonblock; }

    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);
    
private:
    bool m_isInit : 1;
    bool m_isSocket : 1;
    bool m_sysNonblock : 1;
    bool m_usrNonblock : 1;
    bool m_isClosed : 1;
    int m_fd;
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
    sylar::IOManager* m_iomanager;
};

class FdManager {
public:
    typedef RWMutex RWMUtexType;
    FdManager();
    
    FdCtx::ptr get(int fd, bool auto_create = false);
    void del(int fd);

private:
    RWMUtexType m_mutex;
    std::vector<FdCtx::ptr> m_datas;
};

typedef Singleton<FdManager> FdMgr;

}

#endif