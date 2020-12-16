/*
 * File: shell.cc
 * @bref
 * 
 * 
 * Author: YutengFu
 * Contact: <xxf@std.uestc.edu.cn>
 * ----------	---	----------------------------------------------------------
 */


#include <os/shell.h>
#include <os/config.h>

using namespace os;

Shell::Shell()
{
    strcpy(file_path, "../../a.txt");   // 默认存放在根目录下
}




int Shell::filterInput(std::string &input)
{
/* 用于将多个空格压缩为单个空格 */
    
    while(input[input.size() - 1] == ' ')
        input.erase(input.size() - 1, 1);   // 先清除末尾的空格
    
    int pos = 0;
    while(pos < input.size())
    {
        if(input[pos] == ' ')
        {
            while(input[pos + 1] == ' ')
                input.erase(pos, 1);    // 删除当前字符
        }
        pos++;
    }
    return S_OK;
}

int Shell::checkInput(std::string const input)
{
/* 主要检查命令是否为白名单，以及参数个数是否正确 */
    int count = 0;      // 参数个数
    for(int i = 0; i < input.size(); i++)
        if(input[i] == ' ') count++;
    std::string instr;   // 提取指令
    if(count != 0)  instr = input.substr(0, input.find(" "));
    switch (count)
    {
    case 0: // 0操作数指令
        if(ZERO_OPERANDS.find(input) != -1)     // 检查指令是否在白名单内
            return S_OK;    // 若在则返回OK
        break;
    case 1: // 
        if(SINGLE_OPERANDS.find(instr) != -1)
            return S_OK;
        break;
    case 2:
        if(DOUBLE_OPERANDS.find(instr) != -1)
            return S_OK;
    default:
        break;
    }

    printf("Input is illeg\n");    
    return S_FALSE;
}

int Shell::repeat() 
{
    while(true) //不断质询 处理用户的终端输入
    {
        std::getline(std::cin, order);              // 读入一行
        if(order.size() == 0) continue;     
        filterInput(order);                         // 过滤
        if(checkInput(order) == S_FALSE) continue;  // 非法输入则拒绝
        manager.dealWithOrder(order);               // 输入送manager模块
    }
    return S_OK;
}

int Shell::fromFile()
{
    FILE* fp;
    fp = fopen(file_path, "r");
    if(fp == NULL)  return S_FALSE;

    line = fgets(buffer, 255, fp);       // 读取一行
    while(line != NULL)
    {
        order = buffer;
        if(order[order.size() - 1] == '\n') // 去除末尾的换行符
            order = order.substr(0, order.size() - 1); 
        filterInput(order);
        if(checkInput(order) == S_OK)   manager.dealWithOrder(order);       // 送入Manager
        line = fgets(buffer, 255, fp);
    }   

    fclose(fp);

    return S_OK;
}

