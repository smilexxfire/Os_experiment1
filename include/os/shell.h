/*
 * File: shell.h
 * @bref
 * 用户终端
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */

#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <iostream>
#include <os/manager.h> 

namespace os{

// 命令白名单
const std::string ZERO_OPERANDS = "init,to,rel";    // 零操作数
const std::string SINGLE_OPERANDS = "del,list,";    // 单操作数
const std::string DOUBLE_OPERANDS = "cr,req";       // 双操作数

class Shell{
    private:
        std::string order;      // 保存命令
        char buffer[255];       // 缓冲
        char file_path[100];    // 存放命令的文件路径   
        char* line;             // 指向一行输入              
    public:
        Manager manager;
    public:
        int repeat();           // 守护进程 等待用户输入并解析
        int fromFile();         // 从文件读入命令
        int filterInput(std::string &input);     // 过滤输入 
        int checkInput(std::string const input); // 检查输入是否合法
        Shell();
};
}

#endif