/*
 * File: manager.h
 * @bref
 * 进程和资源管理器
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */


#ifndef MANAGER_H
#define MANAGER_H

#include <cstdio>
#include <os/pcb.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace os{

class Manager{
    private:
        char running;       // 运行进程name
        char buffer[100];   // 缓冲
    public:
        int dealWithOrder(std::string order);   // 处理用户命令  
        int init();                             // 初始化init进程
        int create(std::string order);          // 创建进程
        int del(char name);                     // 删除进程                           
        pcb* isRunning();                       // 返回正在running的进程          
        int scheduler();                        // 调度
        int timeOut();                          // 时钟中断
        int listReady();                        // 列出就绪队列
        int listBlock();                        // 列出阻塞队列
        int listRes();                          // 列出资源剩余情况
        int reqSource(std::string req, pcb* cur_pcb); // 请求资源
        int relSource(std::string rel, pcb* cur_pcb); // 释放资源
        int checkDeadLock(rcb* req, pcb* cur_pcb, unsigned num);    // 检查是否可能死锁
        int fillAllocation(char name, std::unordered_map<char, int[4]> &m);
        int fillRequest(char name, std::unordered_map<char, int[4]> &m);
};

}

#endif