#include "application.h"
#include "env.h"
#include "config.h"
#include "daemon.h"

#include <unistd.h>

namespace sylar {
    
static SYLAR__SYSTEM__LOG(g_logger);

static sylar::ConfigVar<std::string>::ptr g_server_work_path =
    sylar::Config::Lookup("server.work_path"
        , std::string("/apps/work/sylar")
        , "server work path");

static sylar::ConfigVar<std::string>::ptr g_server_pid_file =
    sylar::Config::Lookup("server.pid_file"
        , std::string("sylar.pid")
        , "server pid file");

static sylar::ConfigVar<std::vector<TcpServerConf> >::ptr g_servers_conf =
    sylar::Config::Lookup("servers", std::vector<TcpServerConf>(), "http server config");

Application* Application::s_instance = nullptr;

Application::Application() {
    s_instance = this;
}

bool Application::init(int argc, char** argv) {
    m_argc = argc;
    m_argv = argv;
    
    sylar::EnvMgr::GetInstance()->addHelp("s", "start with the triminal");
    sylar::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    sylar::EnvMgr::GetInstance()->addHelp("c", "conf path default: ./conf");
    sylar::EnvMgr::GetInstance()->addHelp("p", "print help");
    
    if (!sylar::EnvMgr::GetInstance()->init(argc, argv)) {
        sylar::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    if (sylar::EnvMgr::GetInstance()->has("p")) {
        sylar::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    int run_type = 0;
    if (sylar::EnvMgr::GetInstance()->has("s")) {
        run_type = 1;
    }
    
    if (sylar::EnvMgr::GetInstance()->has("d")) {
        run_type = 2;
    }

    if (run_type == 0) {
        sylar::EnvMgr::GetInstance()->printHelp();
        return false;
    }
    
    std::string pidfile = g_server_work_path->getValue()
        + "/" + g_server_pid_file->getValue();
    if (sylar::FSUtil::IsRunningPidfile(pidfile)) {
        SYLAR_LOG_ERROR(g_logger) << "server is running: " << pidfile;
        return false;
    }
    std::string conf_path = sylar::EnvMgr::GetInstance()->getAbolutePath(
        sylar::EnvMgr::GetInstance()->get("c", "conf")
    );
    SYLAR_LOG_INFO(g_logger) << "load conf path: " << conf_path;
    sylar::Config::LoadFromConfDir(conf_path);

    if (!sylar::FSUtil::Mkdir(g_server_work_path->getValue())) {
        SYLAR_LOG_FATAL(g_logger) << "Create work path ["
            << g_server_work_path->getValue()
            << " errno = " << errno << " errstr = " << strerror(errno);
        return false;
    }
    
    return true;
}

bool Application::run() {
    bool is_daemon = sylar::EnvMgr::GetInstance()->has("d");
    return start_daemon(m_argc, m_argv
        , std::bind(&Application::main, this, std::placeholders::_1
            , std::placeholders::_2), is_daemon);
}

int Application::main(int argc, char** argv) {
    std::string pidfile = g_server_work_path->getValue()
        + "/" + g_server_pid_file->getValue();
    std::ofstream ofs(pidfile);
    if (!ofs) {
        SYLAR_LOG_ERROR(g_logger) << "open pidfile " << pidfile << "failed";
        return false;
    }
    ofs << getpid();

    

    sylar::IOManager iom(1);
    iom.schedule(std::bind(&Application::run_fiber, this));
    iom.stop();
    
    return 0;
}

int Application::run_fiber() {
    auto http_confs = g_servers_conf->getValue();
    for (auto& i : http_confs) {
        SYLAR_LOG_INFO(g_logger) << LexicalCast<TcpServerConf, std::string>()(i);

        std::vector<Address::ptr> address;
        for (auto& a : i.address) {
            size_t pos = a.find(":");
            if (pos == std::string::npos) {
                SYLAR_LOG_ERROR(g_logger) << "invaild address: " << a;
                continue;
            }
            auto addr = sylar::Address::LookupAny(a);
            if (addr) {
                address.push_back(addr);
                continue;
            }
            std::vector<std::pair<Address::ptr, uint32_t> >result;
            bool rt = sylar::Address::GetInterfaceAddresses(result, a.substr(0, pos));
            if (!rt) {
                SYLAR_LOG_ERROR(g_logger) << "invalid address: " << a;
                continue;
            }
            for (auto& x : result) {
                auto ipaddr = std::dynamic_pointer_cast<IPAddress>(x.first);
                if (ipaddr) {
                    ipaddr->setPort(atoi(a.substr(pos + 1).c_str()));
                }
                address.push_back(ipaddr);
            }
        }
        
        sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(i.keepalive));
        std::vector<Address::ptr> fails;
        if (!server->bind(address, fails)) {
            for (auto& y : fails) {
                SYLAR_LOG_ERROR(g_logger) << "bind address fail: " << *y;
            }
            _exit(0);
        }
        server->setName(i.name);
        server->start();
        m_httpservers.push_back(server);
    }
    
    return 0;
}

}