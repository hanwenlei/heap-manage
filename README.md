# 运行时堆的设计与实现
* 在Windows平台下编写的堆管理程序，实现类似于malloc和free的功能。
* 使用系统API分配若干页面，并以此作为堆空间资源。兼顾并发、时间、空间性能，设计并实现堆空间管理机制。
* main.cpp调用Test.cpp的函数进行测试
* MyMalloc.h是API接口
