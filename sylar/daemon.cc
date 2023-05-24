#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "daemon.h"
#include "log.h"
#include "config.h"

namespace sylar {

static SYLAR__SYSTEM__LOG(g_logger);

static ConfigVar<uint32_t>::ptr g_daemon_restart_interval
    = sylar::Config::Lookup("daemon.restart_interval", (uint32_t)5, "daemon restart interval");

static uint32_t s_daemin_restart_interval = 0;

namespace {
struct _RestartTimeIniter {
    _RestartTimeIniter() {
        s_daemin_restart_interval = g_daemon_restart_interval->getValue();

        g_daemon_restart_interval->addListener([](const uint32_t& ov, const uint32_t& nv) {
            s_daemin_restart_interval = nv;
        });
    }
};

static _RestartTimeIniter _init;
}

std::string ProcessInfo::toString() {
    std::stringstream ss;
    ss << "[ProcessInfo partent_id = " << parent_id
        << " main_id = " << main_id
        << " parent_start_time = " << sylar::Time2Str(parent_start_time)
        << " main_start_time = " << sylar::Time2Str(main_start_time)
        << " restart_count = " << restart_count << "]";
    return ss.str();
}

static int real_start(int argc, char** argv
    , std::function<int(int argc, char** argv)> main_cb) {
    return main_cb(argc, argv);
}

static int real_daemon(int argc, char** argv
    , std::function<int(int argc, char** argv)> main_cb) {
    int rt = daemon(1, 0);
    if (rt == -1) {
        SYLAR_LOG_ERROR(g_logger) << "daemon fail";
        return -1;
    }
    ProcessInfoMgr::GetInstance()->parent_id = getpid();
    ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
    while (true) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程返回
            ProcessInfoMgr::GetInstance()->main_id = getpid();
            ProcessInfoMgr::GetInstance()->main_start_time = time(0);
            SYLAR_LOG_INFO(g_logger) << "process start pid = " << getpid();
            
            return real_start(argc, argv, main_cb);
        } else if (pid < 0) {
            SYLAR_LOG_ERROR(g_logger) << "fork fail return = " << pid
                << " errno = " << errno << " errstr = " << strerror(errno);
            return -1;
        } else {
            // 父进程返回
            int status = 0;
            waitpid(pid, &status, 0);
            if (status) {
                SYLAR_LOG_ERROR(g_logger) << "child crash pid = " << pid
                    << " status = " << status;
            } else {
                SYLAR_LOG_INFO(g_logger) << "child finished pid = " << pid;
                break;
            }
            ProcessInfoMgr::GetInstance()->restart_count += 1;
            sleep(s_daemin_restart_interval);
        }
    }
    return 0;
}

int start_daemon(int argc, char** argv
    , std::function<int(int argc, char** argv)> main_cb
    , bool is_daemon) {
    if (!is_daemon) {
        return real_start(argc, argv, main_cb);
    }
    return real_daemon(argc, argv, main_cb);
}

}