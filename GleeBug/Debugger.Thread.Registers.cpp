#include "Debugger.Thread.Registers.h"

namespace GleeBug
{
    Registers::Registers(HANDLE hThread, DWORD ContextFlags) :
        Dr0(this),
        Dr1(this),
        Dr2(this),
        Dr3(this),
        Dr6(this),
        Dr7(this),

        Eflags(this),

        Eax(this),
        Ax(this),
        Ah(this),
        Al(this),
        Ebx(this),
        Bx(this),
        Bh(this),
        Bl(this),
        Ecx(this),
        Cx(this),
        Ch(this),
        Cl(this),
        Edx(this),
        Dx(this),
        Dh(this),
        Dl(this),
        Edi(this),
        Di(this),
        Esi(this),
        Si(this),
        Ebp(this),
        Bp(this),
        Esp(this),
        Sp(this),
        Eip(this),

#ifdef _WIN64
        Rax(this),
        Rbx(this),
        Rcx(this),
        Rdx(this),
        Rsi(this),
        Sil(this),
        Rdi(this),
        Dil(this),
        Rbp(this),
        Bpl(this),
        Rsp(this),
        Spl(this),
        Rip(this),
        R8(this),
        R8d(this),
        R8w(this),
        R8b(this),
        R9(this),
        R9d(this),
        R9w(this),
        R9b(this),
        R10(this),
        R10d(this),
        R10w(this),
        R10b(this),
        R11(this),
        R11d(this),
        R11w(this),
        R11b(this),
        R12(this),
        R12d(this),
        R12w(this),
        R12b(this),
        R13(this),
        R13d(this),
        R13w(this),
        R13b(this),
        R14(this),
        R14d(this),
        R14w(this),
        R14b(this),
        R15(this),
        R15d(this),
        R15w(this),
        R15b(this),
#endif //_WIN64

        Gax(this),
        Gbx(this),
        Gcx(this),
        Gdx(this),
        Gdi(this),
        Gsi(this),
        Gbp(this),
        Gsp(this),
        Gip(this),

        Gs(this),
        Fs(this),
        Es(this),
        Ds(this),
        Cs(this),
        Ss(this),

        TrapFlag(this),
        ResumeFlag(this)
    {
        memset(&mContext, 0, sizeof(CONTEXT));
        mContext.ContextFlags = ContextFlags;
        if(!!GetThreadContext(hThread, &mContext))
        {
            this->hThread = hThread;
            memcpy(&mOldContext, &mContext, sizeof(CONTEXT));
        }
        else
        {
            this->hThread = nullptr;
        }
    }

    Registers::~Registers()
    {
        if(hThread && memcmp(&mContext, &mOldContext, sizeof(CONTEXT)) != 0)
            SetThreadContext(hThread, &mContext);
    }

    PCONTEXT Registers::GetContext()
    {
        return &mContext;
    }
};