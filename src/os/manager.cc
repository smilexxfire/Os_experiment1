/*
 * File: manager.cc
 * @bref
 * 
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */

#include <os/manager.h>
#include <os/queue.h>
#include <os/rcb.h>
#include <os/config.h>

using namespace os;

int Manager::dealWithOrder(std::string order)
{
    // 以空格分割参数
    std::string first;          // 获取order的第1个参数
    std::string second;         // 获取order的第2部分
    int pos = order.find(" ");
    if(pos == -1) first = "init";
    else{
        first = order.substr(0, pos);
        second = order.substr(pos + 1);
    }
    // 特判
    if(order == "to")   first = "to";
    
    // 根据指令来处理
    if(first == "init") init();     // init
    else if(first == "cr")   create(order.substr(pos + 1));
    else if(first == "to")  timeOut();
    else if(first == "list" && second == "ready")   listReady();
    else if(first == "list" && second == "res")   listRes();
    else if(first == "list" && second == "block")   listBlock();
    else if(first == "req") reqSource(second, isRunning());
    else if(first == "rel") relSource(second, isRunning());
    else if(first == "del") del(second[0]);
    
    return S_OK;
}

int Manager::scheduler()
{
    // 原运行进程就绪
    g_pcb_map[running]->type = READY_TYPE;
    g_pcb_map[running]->list = READY_LIST;

    // 设置运行进程
    running = isRunning()->name;
    g_pcb_map[running]->type = RUNNING_TYPE;
    g_pcb_map[running]->list = READY_LIST;

    return S_OK;
}

int Manager::relSource(std::string rel, pcb* cur_pcb)
{
    if(iit == false)    {printf("You need to init\n"); return S_FALSE;}
    // 获取释放资源的rcb
    std::string rid = rel.substr(0,2);
    if(g_rcb_map.count(rid) == 0)   return S_FALSE;
    rcb* rel_rcb = g_rcb_map[rid];
    
    // 获得释放数量
    if(rel.find(" ") == -1) return S_FALSE;
    int rel_num = atoi(rel.substr(rel.find(" ") + 1).c_str());
    
    // 没找到资源
    auto i = cur_pcb->resource.begin();
    for(; i != cur_pcb->resource.end(); i++)
        if((*i).x == rid)   break;
    if(i == cur_pcb->resource.end())   return S_FALSE;
    
    // 找到资源    
    for(; i != cur_pcb->resource.end(); i++)
    {
        if((*i).x == rid)
        {
            if((*i).y > rel_num)    (*i).y -= rel_num , rel_rcb->available_num += rel_num;
            else    rel_rcb->available_num += (*i).y, (*i).y = 0;
            // 剩余占有资源 ==0 则移出占有资源队列
            if((*i).y == 0) cur_pcb->resource.erase(i);
            break;
        }
    }

    printf("process %c release %d %s. ", cur_pcb->name, rel_num, rid.c_str());
    // 若阻塞队列不为空
    auto j_pcb = rel_rcb->waiting_list.begin();
    while( j_pcb != rel_rcb->waiting_list.end() )
    {
        pcb* block_pcb = (*j_pcb);
        auto k = block_pcb->resource.end() - 1;
        // 剩余资源数不足分配则退出
        if(rel_rcb->available_num < (*k).y) return S_FALSE;
        
        // 资源足够 则阻塞进程加入就绪队列
        block_pcb->type = READY_TYPE;            // 改为就绪态
        block_pcb->list = READY_LIST;
        g_ready_queue[block_pcb->priority].push_back(block_pcb);
        rel_rcb->waiting_list.erase(j_pcb);     // 等待队列中移除

        printf("wake up process %c\n", block_pcb->name);
        // 请求对应资源
        int req_num = (*k).y;
        std::string req = rid + " " + std::to_string(req_num);  // 构造请求
        block_pcb->resource.erase(k);            // 删除阻塞请求的资源
        reqSource(req, block_pcb);               // 重新请求
        // 重复判断
        j_pcb = rel_rcb->waiting_list.begin();
    }

    scheduler();    // 可能唤醒进程优先级高，需调度
    return S_OK;
}

int Manager::reqSource(std::string req, pcb* cur_pcb)
{
    if(iit == false)    {printf("You need to init\n"); return S_FALSE;}
    // 获取请求资源的rcb
    std::string rid = req.substr(0,2);
    if(g_rcb_map.count(rid) == 0)   return S_FALSE;
    rcb* req_rcb = g_rcb_map[rid];
    
    // 获得请求数量
    if(req.find(" ") == -1) return S_FALSE;
    unsigned req_num = atoi(req.substr(req.find(" ") + 1).c_str());

    // 请求数不合理
    if(req_num > req_rcb->sum_num) {printf("ERROR!request is bigger than sum\n"); return S_FALSE;}
    
    // 检测是否会产生死锁
    if(checkDeadLock(req_rcb, cur_pcb, req_num) == S_FALSE)
    {
        printf("DeadLock!No\n");
        return S_FALSE;
    }
    
    // 请求可以满足
    if(req_num <= req_rcb->available_num)
    {
        req_rcb->available_num -= req_num;
        // 若请求资源在队列中
        for(int i = 0; i < cur_pcb->resource.size(); i++)
            if(cur_pcb->resource[i].x == rid)
                {cur_pcb->resource[i].y += req_num; return S_OK;}
        // 不在则push
        PCI pci = {rid, req_num};
        cur_pcb->resource.push_back(pci);
        //打印
        if(cur_pcb == isRunning())
            printf("process %c requests %d %s\n", cur_pcb->name, req_num, req_rcb->rid.c_str());
    }
    // 暂时无法满足，则阻塞进程
    else{
        cur_pcb->type = BLOCK_TYPE;
        cur_pcb->list = BLOCKED_LIST;
        // 从就绪队列移除
        g_ready_queue[cur_pcb->priority].erase(g_ready_queue[cur_pcb->priority].begin());
        cur_pcb->resource.push_back({rid, req_num});    // 阻塞状态下的资源请求
        req_rcb->waiting_list.push_back(cur_pcb);       // 加入到资源的等待队列中

        // 打印
        printf("process %c is running. process %c is blocked\n", isRunning()->name, cur_pcb->name);
    }
    return S_OK;
}

int Manager::listRes()
{
    printf("%s %d\n", g_rcb[0].rid.c_str(), g_rcb[0].available_num);
    printf("%s %d\n", g_rcb[1].rid.c_str(), g_rcb[1].available_num);
    printf("%s %d\n", g_rcb[2].rid.c_str(), g_rcb[2].available_num);
    printf("%s %d\n", g_rcb[3].rid.c_str(), g_rcb[3].available_num);
    return S_OK;
}

int Manager::listBlock()
{
    // 资源R1
    printf("R1 ");
    if(!g_rcb[0].waiting_list.empty())
        printf("%c", g_rcb[0].waiting_list[0]->name);
    for(int i = 1; i < g_rcb[0].waiting_list.size(); i++)
        printf("-%c", g_rcb[0].waiting_list[i]->name);
    printf("\n");
    
    // 资源R2
    printf("R2 ");
    if(!g_rcb[1].waiting_list.empty())
        printf("%c", g_rcb[1].waiting_list[0]->name);
    for(int i = 1; i < g_rcb[1].waiting_list.size(); i++)
        printf("-%c", g_rcb[1].waiting_list[i]->name);
    printf("\n");

    // 资源R3
    printf("R3 ");
    if(!g_rcb[2].waiting_list.empty())
        printf("%c", g_rcb[2].waiting_list[0]->name);
    for(int i = 1; i < g_rcb[2].waiting_list.size(); i++)
        printf("-%c", g_rcb[2].waiting_list[i]->name);
    printf("\n");

    // 资源R4
    printf("R4 ");
    if(!g_rcb[3].waiting_list.empty())
        printf("%c", g_rcb[3].waiting_list[0]->name);
    for(int i = 1; i < g_rcb[3].waiting_list.size(); i++)
        printf("-%c", g_rcb[3].waiting_list[i]->name);
    printf("\n");

    return S_OK;
}

int Manager::listReady()
{   
    // 优先级为2
    printf("2: ");
    if(!g_ready_queue[2].empty())
        printf("%c", g_ready_queue[2][0]->name);
    for(int i = 1; i < g_ready_queue[2].size(); i++)
        printf("-%c", g_ready_queue[2][i]->name);
    printf("\n");
    
    // 优先级为1
    printf("1: ");
    if(!g_ready_queue[1].empty())
        printf("%c", g_ready_queue[1][0]->name);
    for(int i = 1; i < g_ready_queue[1].size(); i++)
        printf("-%c", g_ready_queue[1][i]->name);
    printf("\n");
    
    // 优先级为0
    printf("0: ");
    if(g_ready_queue[0].size() > 0)
        printf("init");
    printf("\n");

    return S_OK;
}

int Manager::timeOut()
{
    pcb* head;  // 指向优先级最高，且对应就绪列队中第一个进程

    if( !g_ready_queue[2].empty() )
    {
        head = g_ready_queue[2][0];
        g_ready_queue[2].erase(g_ready_queue[2].begin());       // 删除头部元素
        head->priority--;                                       // 降低优先级
        g_ready_queue[head->priority].push_back(head);          // 放入对应的就绪列队
    }    
    else if( !g_ready_queue[1].empty() )
    {
        // 只有一个进程,则该进程继续运行
        if(g_ready_queue[1].size() == 1){
            printf("process %c is still running\n", isRunning()->name);
            return S_OK;
        }
        head = g_ready_queue[1][0];
        g_ready_queue[1].erase(g_ready_queue[1].begin());   // 删除头部元素
        g_ready_queue[1].push_back(head);                   // 头部元素再入队列
    }

    scheduler();                    // 调度
    // 打印信息
    printf("process %c is running. process %c is ready\n", isRunning()->name, head->name);

    return S_OK;
}

int Manager::del(char name)
{
    if(g_pcb_map.count(name) == 0)   return S_FALSE;
    // 找到要删除进程的pcb
    pcb* cur_pcb = g_pcb_map[name];

    // 父进程的child容器删除自己
    pcb* parent = cur_pcb->parent;
    for(auto i = parent->child.begin(); i != parent->child.end(); i++)
        if((*i)->name == name)
        {
            parent->child.erase(i);
            break;
        }
    
    // 就绪进程的删除
    if(cur_pcb->list == READY_LIST)
    {
        // 就绪队列中移除自己
        for(auto i = g_ready_queue[cur_pcb->priority].begin(); i != g_ready_queue[cur_pcb->priority].end(); i++)
            if( (*i) == cur_pcb)
            {
                g_ready_queue[cur_pcb->priority].erase(i);
                break;
            }
    }

    // 阻塞进程的删除
    else
    {
        rcb* block_res = g_rcb_map[cur_pcb->resource[cur_pcb->resource.size() - 1].x];  // 定位到阻塞的资源
            for(auto i = block_res->waiting_list.begin(); i != block_res->waiting_list.end(); i++)
                if((*i) == cur_pcb)
                {
                    block_res->waiting_list.erase(i);   // 阻塞资源的等待队列中删除自己
                    break;
                }
        // 占有资源的最后一项为阻塞请求,先清除
        cur_pcb->resource.erase(cur_pcb->resource.end() - 1);
    }
    
    // 释放占有的资源
    while(cur_pcb->resource.size() > 0)
    {
        PCI occupy_res = cur_pcb->resource[0];
        std::string rel = occupy_res.x;
        relSource(occupy_res.x + " " + std::to_string(occupy_res.y), cur_pcb);
    }

    // 递归删除所有子进程
    while(cur_pcb->child.size() > 0)
        del(cur_pcb->child[0]->name);

    // 删除自己，释放内存
    delete cur_pcb;

    return S_OK;
}

int Manager::create(std::string order)
{
    if(iit == false)    {printf("You need to init\n"); return S_FALSE;}
    pcb* new_pcb = new pcb;
    // 初始化PCB结构
    new_pcb->pid = g_pid++;
    new_pcb->type = READY_TYPE;    // 就绪状态
    new_pcb->list = READY_LIST;
    new_pcb->name = order[0];
    new_pcb->priority = int(order[2] - '0');
    new_pcb->parent = isRunning(); // 父进程为当前运行进程
    
    // 父进程的child列表
    pcb* parent = new_pcb->parent;
    parent->child.push_back(new_pcb);

    //加入对应的就绪队列
    g_ready_queue[new_pcb->priority].push_back(new_pcb);

    // 加入全局map
    if(g_pcb_map.count(new_pcb->name) == 0) 
        g_pcb_map[new_pcb->name] = new_pcb;

    scheduler();       // 新创建进程的优先级可能更高,需调度
    
    // 打印当前运行进程
    printf("process %c is running\n", isRunning()->name);

    return S_OK;
}

pcb* Manager::isRunning()
{
    // 优先级高的优先运行
    if( !g_ready_queue[2].empty())
        return g_ready_queue[2][0];
    else if( !g_ready_queue[1].empty())
        return g_ready_queue[1][0];
    return g_pcb;
}

int Manager::init()
{
    g_pcb = new pcb;
    // 初始化init进程的PCB
    g_pcb->pid = g_pid++;
    g_pcb->type = RUNNING_TYPE;
    g_pcb->list = READY_LIST;
    g_pcb->parent = nullptr;
    g_pcb->priority = 0;
    g_pcb->name = '\0';

    // 放入就绪队列
    g_ready_queue[0].push_back(g_pcb);
    
    // 初始化全局资源rcb
    g_rcb = new rcb[4];
    g_rcb[0].rid = "R1";
    g_rcb[1].rid = "R2";
    g_rcb[2].rid = "R3";
    g_rcb[3].rid = "R4";
    for(int i = 0; i < 4; i++)
        g_rcb_map[g_rcb[i].rid] = &g_rcb[i];
    
    // 正在运行进程
    running = '\0';             // init进程name为\0
    g_pcb_map['\0'] = g_pcb;

    iit = true;
    //输出
    printf("init process is running\n");

    return S_OK;
}

int Manager::checkDeadLock(rcb* req, pcb* cur_pcb, unsigned num)
{
    int available[4];                       // 资源可得数
    std::unordered_map<char, int[4]> allocation_m;    // 已分配资源数组
    std::unordered_map<char, int[4]> request_m;       // 进程请求数组
    std::unordered_map<char, bool>    sign_m;         // 进程标记数组
    
    // 获取资源可得数
    for(int i = 0; i < 4; i++)
        available[i] = g_rcb[i].available_num;           
    
    // 填充allocation数组和request数组
    std::unordered_map<char, pcb*>::iterator iter = g_pcb_map.begin();
    while(iter != g_pcb_map.end()){     // 找到所有进程
        if(iter->second->name != '\0')  // 非init进程
        {
            char name = iter->second->name;
            allocation_m[name];
            request_m[name];
            sign_m[name] = false;
            fillAllocation(name, allocation_m); // 填充allocation数组对应行
            fillRequest(name, request_m);       // 填充request数组对应行 
        }
        iter++;
    }
    if(req->rid == "R1")    request_m[cur_pcb->name][0] += num;
    if(req->rid == "R2")    request_m[cur_pcb->name][1] += num;
    if(req->rid == "R3")    request_m[cur_pcb->name][2] += num;
    if(req->rid == "R4")    request_m[cur_pcb->name][3] += num;

    // 标记allocation数组中全为0的进程
    std::unordered_map<char, int[4]>::iterator ite = allocation_m.begin();
    while(ite != allocation_m.end()){
        if(ite->second[0] == 0 && ite->second[1] == 0 && ite->second[2] == 0 && ite->second[3] == 0)
            sign_m[ite->first] = true;
        ite++;
    }

    // 寻找可运行结束的进程
    ite = request_m.begin();
    while(ite != request_m.end())
    {
        char name = ite->first;
        if(sign_m[name] == false && 
            request_m[name][0] <= available[0] && request_m[name][1] <= available[1] &&
            request_m[name][2] <= available[2] && request_m[name][3] <= available[3]
        )
        {
            sign_m[name] = true;
            available[0]+= allocation_m[name][0];
            available[1]+= allocation_m[name][1];
            available[2]+= allocation_m[name][2];
            available[3]+= allocation_m[name][3];
            ite = request_m.begin();
            continue;
        }
        ite++;
    }

    // 是否有未标记的进程
    for(auto i = sign_m.begin(); i != sign_m.end(); i++)
        if(i->second == false)  return S_FALSE;

    return S_OK;
} 

int Manager::fillAllocation(char name, std::unordered_map<char, int[4]> &m)
{
    pcb* cur_pcb = g_pcb_map[name]; // 拿到进程pcb
    int len = cur_pcb->resource.size();
    for(int i = 0; i < len; i++)
    {
        // 阻塞进程的最后一个资源为阻塞请求
        if(cur_pcb->type == BLOCK_TYPE && i == len - 1)
            break;
        PCI cur_resource = cur_pcb->resource[i];
        if(cur_resource.first == "R1")  m[name][0] = cur_resource.second;
        if(cur_resource.first == "R2")  m[name][1] = cur_resource.second;
        if(cur_resource.first == "R3")  m[name][2] = cur_resource.second;
        if(cur_resource.first == "R4")  m[name][3] = cur_resource.second;
    }
    return S_OK;
}

int Manager::fillRequest(char name, std::unordered_map<char, int[4]> &m)
{
    pcb* cur_pcb = g_pcb_map[name];
    if(cur_pcb->type == BLOCK_TYPE)
    {
        PCI req_resource = cur_pcb->resource.back();
        if(req_resource.first == "R1") m[name][0] += req_resource.second;
        if(req_resource.first == "R2") m[name][1] += req_resource.second;
        if(req_resource.first == "R3") m[name][2] += req_resource.second;
        if(req_resource.first == "R4") m[name][3] += req_resource.second;
    }
    return S_OK;
}
