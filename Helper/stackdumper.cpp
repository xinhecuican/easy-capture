#include "stackdumper.h"
#include<QLibrary>

QString TraceStack()
{
    static const int MAX_STACK_FRAMES = 5;
    QLibrary mylib("dbghelp.dll");
    mylib.load();

    void *pStack[MAX_STACK_FRAMES];

    HANDLE process = GetCurrentProcess();
    ((WINBOOL IMAGEAPI(*)(HANDLE hProcess,PCSTR UserSearchPath,WINBOOL fInvadeProcess))
            mylib.resolve("SymInitialize"))(process, NULL, TRUE);
    //SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);

    std::ostringstream oss;
    oss << "stack traceback: " << std::endl;
    for (WORD i = 0; i < frames; ++i) {
        DWORD64 address = (DWORD64)(pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD displacementLine = 0;
        IMAGEHLP_LINE64 line;
        //SymSetOptions(SYMOPT_LOAD_LINES);
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if((((WINBOOL IMAGEAPI (*)(HANDLE hProcess,DWORD64 Address,PDWORD64 Displacement,PSYMBOL_INFO Symbol))mylib.resolve("SymFromAddr"))(process, address, &displacementSym, pSymbol))
        //if (SymFromAddr(process, address, &displacementSym, pSymbol)
         && ((WINBOOL IMAGEAPI (*)(HANDLE hProcess,DWORD64 qwAddr,PDWORD pdwDisplacement,PIMAGEHLP_LINE64 Line64))mylib.resolve("SymGetLineFromAddr64"))(process, address, &displacementLine, &line)){
                //SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
            oss << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
        }
        else {
            oss << "\terror: " << GetLastError() << std::endl;
        }
    }
    return QString::fromStdString(oss.str());
}
