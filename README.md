# OS实验1

## 环境

- cmake3.18.2
- Visual Studio 2019
- Windows

## 安装&运行

~~~shell
cd build
cmake ..
~~~

生成os.sln后，用VS打开，设置os为启动项目，`F5`编译运行即可。

## 项目结构

~~~shell
F:\os.
│  CMakeLists.txt
│  config.h.in
│  README.md
│
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

  





