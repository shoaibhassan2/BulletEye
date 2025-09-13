#define _CRT_SECURE_NO_WARNINGS

#include <time.h>
#include <globals.h>
#include <crash_handler/crash_ui.h>
#include <crash_handler/crash_handler.h>
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>
// --------------------------- Exception descriptions ---------------------------
static const char *GetExceptionDescription(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        return "Access Violation";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "Array Bounds Exceeded";
    case EXCEPTION_BREAKPOINT:
        return "Breakpoint";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "Datatype Misalignment";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "Float Denormal Operand";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "Float Divide by Zero";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "Float Inexact Result";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "Float Invalid Operation";
    case EXCEPTION_FLT_OVERFLOW:
        return "Float Overflow";
    case EXCEPTION_FLT_STACK_CHECK:
        return "Float Stack Check";
    case EXCEPTION_FLT_UNDERFLOW:
        return "Float Underflow";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "Illegal Instruction";
    case EXCEPTION_IN_PAGE_ERROR:
        return "In Page Error";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "Integer Divide by Zero";
    case EXCEPTION_INT_OVERFLOW:
        return "Integer Overflow";
    case EXCEPTION_INVALID_DISPOSITION:
        return "Invalid Disposition";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "Noncontinuable Exception";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "Privileged Instruction";
    case EXCEPTION_SINGLE_STEP:
        return "Single Step";
    case EXCEPTION_STACK_OVERFLOW:
        return "Stack Overflow";
    case EXCEPTION_GUARD_PAGE:
        return "Guard Page Violation";
    case EXCEPTION_INVALID_HANDLE:
        return "Invalid Handle";
    case STATUS_FLOAT_MULTIPLE_FAULTS:
        return "Multiple Floating-Point Faults";
    case STATUS_FLOAT_MULTIPLE_TRAPS:
        return "Multiple Floating-Point Traps";
    case STATUS_REG_NAT_CONSUMPTION:
        return "NaT Consumption (Itanium)";
    default:
        return "Unknown Exception";
    }
}

// --------------------------- Helpers: symbol engine ---------------------------
static void EnsureSymInitialized()
{
    static volatile LONG g_inited = 0;
    if (InterlockedCompareExchange(&g_inited, 1, 0) == 0)
    {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
        (void)SymInitialize(GetCurrentProcess(), NULL, TRUE); // use _NT_SYMBOL_PATH if set
    }
}

static void ResolveSymbol(DWORD64 address, char *out, size_t outSize)
{
    out[0] = '\0';
    HANDLE hProcess = GetCurrentProcess();

    // Prepare SYMBOL_INFO buffer
    BYTE buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
    PSYMBOL_INFO pSym = (PSYMBOL_INFO)buffer;
    pSym->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSym->MaxNameLen = MAX_SYM_NAME;

    DWORD64 disp64 = 0;
    if (SymFromAddr(hProcess, address, &disp64, pSym))
    {
        snprintf(out, outSize, "%s + 0x%llX", pSym->Name, (unsigned long long)disp64);
    }
    else
    {
        snprintf(out, outSize, "0x%p", (void *)address);
    }

    // Try to append file:line
    IMAGEHLP_LINE64 line = {0};
    line.SizeOfStruct = sizeof(line);
    DWORD disp32 = 0;
    if (SymGetLineFromAddr64(hProcess, address, &disp32, &line))
    {
        size_t len = strlen(out);
        if (len < outSize)
        {
            snprintf(out + len, outSize - len, "  (%s:%lu)", line.FileName, line.LineNumber);
        }
    }
}

// --------------------------- Stack trace ---------------------------
static void BuildStackTrace(CONTEXT *ctx, char *out, size_t outSize, int maxFrames)
{
    out[0] = '\0';
    EnsureSymInitialized();

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    STACKFRAME64 sf = {};
    DWORD machine = 0;

#if defined(_M_IX86)
    machine = IMAGE_FILE_MACHINE_I386;
    sf.AddrPC.Offset = ctx->Eip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = ctx->Ebp;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = ctx->Esp;
    sf.AddrStack.Mode = AddrModeFlat;
#elif defined(_M_X64)
    machine = IMAGE_FILE_MACHINE_AMD64;
    sf.AddrPC.Offset = ctx->Rip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = ctx->Rsp;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = ctx->Rsp;
    sf.AddrStack.Mode = AddrModeFlat;
#else
    _snprintf(out, outSize, "(Stack tracing unsupported on this arch)\n");
    return;
#endif

    char line[512];
    for (int i = 0; i < maxFrames; ++i)
    {
        if (!StackWalk64(machine, hProcess, hThread, &sf, ctx, NULL,
                         SymFunctionTableAccess64, SymGetModuleBase64, NULL))
        {
            break;
        }
        if (sf.AddrPC.Offset == 0)
            break;

        char sym[512];
        ResolveSymbol(sf.AddrPC.Offset, sym, sizeof(sym));
        _snprintf(line, sizeof(line), "#%d %s\n", i, sym);

        size_t used = strlen(out);
        size_t avail = (used < outSize) ? (outSize - used) : 0;
        if (avail <= 1)
            break;
        strncat(out, line, avail - 1);
    }
}

// --------------------------- Minidump writing ---------------------------
static void MakeTimestampedPath(wchar_t *path, size_t cchPath)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t *slash = wcsrchr(exePath, L'\\');
    if (slash)
        *slash = 0;

    // Optional: create "crash_dumps" folder next to EXE
    wchar_t dumpDir[MAX_PATH];
    _snwprintf(dumpDir, MAX_PATH, L"%s\\crash_dumps", exePath);
    CreateDirectoryW(dumpDir, NULL);

    _snwprintf(path, cchPath, L"%s\\crash_%04u%02u%02u_%02u%02u%02u.dmp",
               dumpDir,
               st.wYear, st.wMonth, st.wDay,
               st.wHour, st.wMinute, st.wSecond);
}

static void WriteMiniDump(EXCEPTION_POINTERS *ep)
{
    wchar_t path[MAX_PATH];
    MakeTimestampedPath(path, MAX_PATH);

    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;

    MINIDUMP_EXCEPTION_INFORMATION mei = {};
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = ep;
    mei.ClientPointers = FALSE;

    // Include useful extras
    DWORD flags =
        MiniDumpWithDataSegs |
        MiniDumpWithHandleData |
        MiniDumpNormal |
        MiniDumpWithIndirectlyReferencedMemory |
        MiniDumpScanMemory;

    (void)MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                            hFile, (MINIDUMP_TYPE)flags, &mei, NULL, NULL);
    CloseHandle(hFile);
}

// --------------------------- Crash handler ---------------------------
static void FormatRegisters(CONTEXT *ctx, char *out, size_t outSize)
{
#if defined(_M_IX86)
    _snprintf(out, outSize,
              "EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n"
              "ESI=%08X EDI=%08X EBP=%08X ESP=%08X\n"
              "EIP=%08X FLAGS=%08X\n",
              ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx,
              ctx->Esi, ctx->Edi, ctx->Ebp, ctx->Esp,
              ctx->Eip, ctx->EFlags);
#elif defined(_M_X64)
    _snprintf(out, outSize,
              "RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX\n"
              "RSI=%016llX RDI=%016llX RBP=%016llX RSP=%016llX\n"
              "R8 =%016llX R9 =%016llX R10=%016llX R11=%016llX\n"
              "R12=%016llX R13=%016llX R14=%016llX R15=%016llX\n"
              "RIP=%016llX FLAGS=%08X\n",
              (unsigned long long)ctx->Rax, (unsigned long long)ctx->Rbx,
              (unsigned long long)ctx->Rcx, (unsigned long long)ctx->Rdx,
              (unsigned long long)ctx->Rsi, (unsigned long long)ctx->Rdi,
              (unsigned long long)ctx->Rbp, (unsigned long long)ctx->Rsp,
              (unsigned long long)ctx->R8, (unsigned long long)ctx->R9,
              (unsigned long long)ctx->R10, (unsigned long long)ctx->R11,
              (unsigned long long)ctx->R12, (unsigned long long)ctx->R13,
              (unsigned long long)ctx->R14, (unsigned long long)ctx->R15,
              (unsigned long long)ctx->Rip, ctx->EFlags);
#else
    _snprintf(out, outSize, "Unknown architecture\n");
#endif
}

// --------------------------- Crash handler ---------------------------
LONG WINAPI CrashHandler(EXCEPTION_POINTERS *ep)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

    const DWORD code = ep->ExceptionRecord->ExceptionCode;
    void *const addr = ep->ExceptionRecord->ExceptionAddress;
    CONTEXT *ctx = ep->ContextRecord;

    char regs[1024];
    FormatRegisters(ctx, regs, sizeof(regs));
    char stack[8192];
    BuildStackTrace(ctx, stack, sizeof(stack), 64);

    WriteMiniDump(ep);

    char msg[12000];
    _snprintf(msg, sizeof(msg),
              "Oops! The program has crashed.\n\n"
              "Error: %s (0x%08X)\n"
              "Address: %p\n\n"
              "Registers:\n%s\n"
              "Stack trace:\n%s\n"
              "A crash dump was written next to the executable in the 'crash_dumps' folder.\n"
              "The application will now close.",
              GetExceptionDescription(code), code, addr, regs, stack);

    ShowCrashWindow(msg);

    ExitProcess(code);
    return EXCEPTION_EXECUTE_HANDLER;
}

// --------------------------- Public API ---------------------------
void InstallCrashHandler(void)
{
    SetUnhandledExceptionFilter(CrashHandler);
}