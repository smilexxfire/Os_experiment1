# OS实验1

## 环境

- cmake3.18.2
- Visual Studio 2019
- Windows

## 安装&运行

~~~shell
git clone https://github.com/xshuaiy/Os_experiment1.git
cd build
cmake ..
~~~

生成os.sln后，用VS打开，**设置os为启动项目**，`F5`编译运行即可。

## 项目结构

~~~shell
F:\os.
│  CMakeLists.txt
│  config.h.in
│  README.md
│  a.txt		# 首先执行的命令列表
├─build
├─include
│  ├─db
│  │      config.h
│  │
│  └─os
│          config.h
│          manager.h
│          pcb.h
│          queue.h
│          rcb.h
│          shell.h
│
└─src
    │  CMakeLists.txt
    │  main.cc
    │
    └─os
            manager.cc
            shell.cc
~~~

## 命名规范

- 类的命名采用驼峰法（首字母大写），例如`Shell`
- 方法的命名采用驼峰法（首字母小写），例如`listReady`，`dealWithOrder`
- 变量的命名以小写字母或小写字母+下划线组成，例如`cur_pcb`，`waiting_list`
- 全局变量的命名以g开头，例如`g_ready_queue`，`g_pcb`
- 常量以大写字母和下划线命名，例如`RESOURCE_INIT_NUM`，`ZERO_OPERANDS`

## 实现的命令

- `init`        初始化（第一个执行的命令
- `cr x 1`	创建进程
- `del x`      删除进程
- `req R1 3` 当前进程请求资源（初始化有R1 - R4 4个资源
- `rel R1 3` 当前进程释放资源
- `to`            时钟中断
- `list ready` 展示就绪进程
- `list block` 展示阻塞进程
- `list res`     展示资源情况







