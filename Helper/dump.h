#ifndef DUMP_H
#define DUMP_H
#pragma once
#include <windows.h>
#include <Dbghelp.h>
#include <iostream>
#include <vector>
#include <tchar.h>
#include "debug.h"
using namespace std;


#pragma comment(lib, "Dbghelp.lib")


namespace NSDumpFile
{
    void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
    {
        // 创建Dump文件
        //
        HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDumpFile != INVALID_HANDLE_VALUE)
        {
            // Dump信息
            //
            MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
            dumpInfo.ExceptionPointers = pException;
            dumpInfo.ThreadId = GetCurrentThreadId();
            dumpInfo.ClientPointers = TRUE;


            // 写入Dump文件内容
            //
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
        }
#ifdef QT_NO_DEBUG
    QString version = Update::now_version.get_version();
    QString cmd = "CrashHandler.exe " + version;
    QProcess::startDetached(cmd.toStdString().c_str());
#else
    //这里弹出一个错误对话框并退出程序
    Debug::show_error_message(QObject::tr("Program crash\nSee crash.dmp for more information"));
#endif

        CloseHandle(hDumpFile);
    }


    LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
    {
        return NULL;
    }


    BOOL PreventSetUnhandledExceptionFilter()
    {
        HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
        if (hKernel32 ==   NULL)
            return FALSE;


        void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
        if(pOrgEntry == NULL)
            return FALSE;


        unsigned char newJump[ 100 ];
        DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
        dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far


        void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
        DWORD dwNewEntryAddr = (DWORD) pNewFunc;
        DWORD dwRelativeAddr = dwNewEntryAddr -  dwOrgEntryAddr;


        newJump[ 0 ] = 0xE9;  // JMP absolute
        memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
        SIZE_T bytesWritten;
        BOOL bRet = WriteProcessMemory(GetCurrentProcess(),    pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
        return bRet;
    }


    LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException)
    {
        TCHAR szMbsFile[MAX_PATH] = { 0 };
        ::GetModuleFileName(NULL, szMbsFile, MAX_PATH);
        TCHAR* pFind = _tcsrchr(szMbsFile, '\\');
        if(pFind)
        {
            *(pFind+1) = 0;
            _tcscat(szMbsFile, _T("crash.dmp"));
            CreateDumpFile(szMbsFile, pException);
        }


        // TODO: MiniDumpWriteDump
        FatalAppExit(-1,  _T("Fatal Error"));
        return EXCEPTION_CONTINUE_SEARCH;
    }


    void RunCrashHandler()
    {
        SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
        PreventSetUnhandledExceptionFilter();
    }
};


#define DeclareDumpFile() NSDumpFile::RunCrashHandler();
#endif // DUMP_H
