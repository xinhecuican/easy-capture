#ifndef STACKDUMPER_H
#define STACKDUMPER_H
#include <process.h>
#include <iostream>
#include <Windows.h>
#include<dbghelp.h>
#include<sstream>
#include<imagehlp.h>
#include "dbghelp.h"
#include<QString>
using namespace std;





/// @brief 获取当前调用堆栈
/// @details 默认获取堆栈深度为5层。\n
///    https://msdn.microsoft.com/en-us/library/windows/desktop/ms680578(v=vs.85).aspx \n
///    在MSDN示例代码中获取文件行号信息需要设置SymSetOptions(SYMOPT_LOAD_LINES)，\n
///    并强调只有在设置这个参数后才能检索到文件行号信息。\n
///    但本人在测试过程中，未设置该参数，仍然能获取到行号信息，因此在代码中屏蔽了该设置。\n
///    如果需要设置该参数，要注意SymSetOptions并不是线程安全的，要注意同步问题。\n
///    参见 https://msdn.microsoft.com/en-us/library/windows/desktop/ms681366(v=vs.85).aspx \n里面说明：\n
///    所有的DbgHelp接口，正如SymSetOptions，是单线程的。因此，多线程调用这个接口，\n
///    将很可能导致预料外的行为或者内存异常。为了避免这个问题，在多线程调用这个接口的时候要进行同步处理。\n
/// @warning 需要注意多线程问题。

QString TraceStack();
#endif
