#ifndef __SYLAR_DAEMON_H__
#define __SYLAR_DAEMON_H__

#include <functional>
#include <unistd.h>

#include "singleton.h"

namespace sylar {

struct ProcessInfo {
    // ������id
    pid_t parent_id;
    // ������id
    pid_t main_id;
    // ����������ʱ��
    uint64_t parent_start_time = 0;
    // ����������ʱ��
    uint64_t main_start_time = 0;
    // �����������Ĵ���
    uint32_t restart_count = 0;

    std::string toString();
};

typedef sylar::Singleton<ProcessInfo> ProcessInfoMgr;

/**
 * @brief ��������ѡ�����ػ����̵ķ�ʽ
 * 
 * @param argc ��������
 * @param argv ����ֵ����
 * @param main_cb ��������
 * @param is_daemon �Ƿ��ػ�����
 * @return int ���غ���ִ�н��
 */
int start_daemon(int argc, char** argv
    , std::function<int(int argc, char** argv)> main_cb
    , bool is_daemon);
}

#endif