/*
 * File: pcb.h
 * @bref
 * 定义进程的pcb结构
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */

#ifndef PCB_H
#define PCB_H

#define x first
#define y second

#include <os/rcb.h>
#include <vector>

namespace os{

typedef std::pair<std::string,int> PCI;

// 进程所处队列
const int READY_LIST = 0;
const int BLOCKED_LIST = 1;

// 进程当前状态
const int RUNNING_TYPE = 0;
const int READY_TYPE = 1;
const int BLOCK_TYPE = 2;

typedef struct pcb{
    unsigned pid;           // 唯一标识PID号
    int type;               // 进程当前状态 run/ready/block
    char name;              // 进程的唯一标识name
    bool list;              // 进程所处队列 ready/block
    int priority;           // 优先级 0-2
    struct pcb *parent;     // 父进程
    std::vector<pcb*> child;     // 子进程列表
    std::vector<PCI> resource;   // 标识拥有的资源
}pcb;

// 全局pcb 用于表示init进程
__declspec(selectany) pcb* g_pcb;
// 全局name - pcb映射表
__declspec(selectany) std::unordered_map<char, pcb*> g_pcb_map; 
// 全局pid号
__declspec(selectany) int g_pid = 0;

}   //namespace os

#endif
