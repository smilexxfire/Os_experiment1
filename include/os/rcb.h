/*
 * File: rcb.h
 * @bref
 * 定义资源控制块RCB
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */


#ifndef RCB_H
#define RCB_H

#include <string>
#include <os/pcb.h>
#include <unordered_map>
#include <vector>

namespace os{

const int RESOURCE_INIT_NUM = 5;    
const int RESOURCE_VARIATY_NUM = 4;     // 资源种类数   

typedef struct rcb{
    std::string rid;
    unsigned sum_num;                   // 资源总数
    unsigned available_num;             // 资源可得数
    std::vector<struct pcb*> waiting_list;     // 等待队列
    
    rcb()
        : sum_num(RESOURCE_INIT_NUM)
        , available_num(RESOURCE_INIT_NUM)
    {}
}rcb;
// 四类全局资源
__declspec(selectany) rcb* g_rcb;
// rid - rcb的映射
__declspec(selectany) std::unordered_map<std::string,rcb*> g_rcb_map;


}

#endif