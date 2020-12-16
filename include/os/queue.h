/*
 * File: queue.h
 * @bref
 * 队列的声明
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */

#ifndef QUEUE_H
#define QUEUE_H
#include <vector>
#include <unordered_map>
#include <os/pcb.h>

namespace os{

// 全局就绪队列 - 下标为优先级
__declspec(selectany) std::vector<pcb*> g_ready_queue[3];


} //namespace os

#endif