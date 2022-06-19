#include "Debugger.Thread.h"
#include "Debugger.Thread.Registers.h"

#define BITSET(a,x) (a|=1<<x)
#define BITCLEAR(a,x) (a&=~(1<<x))
#define BITTOGGLE(a,x) (a^=1<<x)
#define BITGET(a,x) (a&(1<<x))

namespace GleeBug
{
    enum DR7_MODE
    {
        MODE_DISABLED = 0, //00
        MODE_LOCAL = 1, //01
        MODE_GLOBAL = 2 //10
    };

    enum DR7_TYPE
    {
        TYPE_EXECUTE = 0, //00
        TYPE_WRITE = 1, //01
        TYPE_READWRITE = 3 //11
    };

    enum DR7_SIZE
    {
        SIZE_1 = 0, //00
        SIZE_2 = 1, //01
        SIZE_8 = 2, //10
        SIZE_4 = 3 //11
    };

#pragma pack(1)
    struct DR7
    {
        BYTE DR7_MODE[HWBP_COUNT];
        BYTE DR7_TYPE[HWBP_COUNT];
        BYTE DR7_SIZE[HWBP_COUNT];
    };

    static ptr dr7_ptr(const DR7 & dr7)
    {
        ptr result = 0;
        if(BITGET(dr7.DR7_MODE[0], 0))
            BITSET(result, 0);
        if(BITGET(dr7.DR7_MODE[0], 1))
            BITSET(result, 1);
        if(BITGET(dr7.DR7_MODE[1], 0))
            BITSET(result, 2);
        if(BITGET(dr7.DR7_MODE[1], 1))
            BITSET(result, 3);
        if(BITGET(dr7.DR7_MODE[2], 0))
            BITSET(result, 4);
        if(BITGET(dr7.DR7_MODE[2], 1))
            BITSET(result, 5);
        if(BITGET(dr7.DR7_MODE[3], 0))
            BITSET(result, 6);
        if(BITGET(dr7.DR7_MODE[3], 1))
            BITSET(result, 7);
        if(BITGET(dr7.DR7_TYPE[0], 0))
            BITSET(result, 16);
        if(BITGET(dr7.DR7_TYPE[0], 1))
            BITSET(result, 17);
        if(BITGET(dr7.DR7_SIZE[0], 0))
            BITSET(result, 18);
        if(BITGET(dr7.DR7_SIZE[0], 1))
            BITSET(result, 19);
        if(BITGET(dr7.DR7_TYPE[1], 0))
            BITSET(result, 20);
        if(BITGET(dr7.DR7_TYPE[1], 1))
            BITSET(result, 21);
        if(BITGET(dr7.DR7_SIZE[1], 0))
            BITSET(result, 22);
        if(BITGET(dr7.DR7_SIZE[1], 1))
            BITSET(result, 23);
        if(BITGET(dr7.DR7_TYPE[2], 0))
            BITSET(result, 24);
        if(BITGET(dr7.DR7_TYPE[2], 1))
            BITSET(result, 25);
        if(BITGET(dr7.DR7_SIZE[2], 0))
            BITSET(result, 26);
        if(BITGET(dr7.DR7_SIZE[2], 1))
            BITSET(result, 27);
        if(BITGET(dr7.DR7_TYPE[3], 0))
            BITSET(result, 28);
        if(BITGET(dr7.DR7_TYPE[3], 1))
            BITSET(result, 29);
        if(BITGET(dr7.DR7_SIZE[3], 0))
            BITSET(result, 30);
        if(BITGET(dr7.DR7_SIZE[3], 1))
            BITSET(result, 31);
        return result;
    }

    static DR7 ptr_dr7(ptr dr7)
    {
        DR7 result;
        memset(&result, 0, sizeof(DR7));
        if(BITGET(dr7, 0))
            BITSET(result.DR7_MODE[0], 0);
        if(BITGET(dr7, 1))
            BITSET(result.DR7_MODE[0], 1);
        if(BITGET(dr7, 2))
            BITSET(result.DR7_MODE[1], 0);
        if(BITGET(dr7, 3))
            BITSET(result.DR7_MODE[1], 1);
        if(BITGET(dr7, 4))
            BITSET(result.DR7_MODE[2], 0);
        if(BITGET(dr7, 5))
            BITSET(result.DR7_MODE[2], 1);
        if(BITGET(dr7, 6))
            BITSET(result.DR7_MODE[3], 0);
        if(BITGET(dr7, 7))
            BITSET(result.DR7_MODE[3], 1);
        if(BITGET(dr7, 16))
            BITSET(result.DR7_TYPE[0], 0);
        if(BITGET(dr7, 17))
            BITSET(result.DR7_TYPE[0], 1);
        if(BITGET(dr7, 18))
            BITSET(result.DR7_SIZE[0], 0);
        if(BITGET(dr7, 19))
            BITSET(result.DR7_SIZE[0], 1);
        if(BITGET(dr7, 20))
            BITSET(result.DR7_TYPE[1], 0);
        if(BITGET(dr7, 21))
            BITSET(result.DR7_TYPE[1], 1);
        if(BITGET(dr7, 22))
            BITSET(result.DR7_SIZE[1], 0);
        if(BITGET(dr7, 23))
            BITSET(result.DR7_SIZE[1], 1);
        if(BITGET(dr7, 24))
            BITSET(result.DR7_TYPE[2], 0);
        if(BITGET(dr7, 25))
            BITSET(result.DR7_TYPE[2], 1);
        if(BITGET(dr7, 26))
            BITSET(result.DR7_SIZE[2], 0);
        if(BITGET(dr7, 27))
            BITSET(result.DR7_SIZE[2], 1);
        if(BITGET(dr7, 28))
            BITSET(result.DR7_TYPE[3], 0);
        if(BITGET(dr7, 29))
            BITSET(result.DR7_TYPE[3], 1);
        if(BITGET(dr7, 30))
            BITSET(result.DR7_SIZE[3], 0);
        if(BITGET(dr7, 31))
            BITSET(result.DR7_SIZE[3], 1);
        return result;
    }

    static DR7_SIZE size_dr7(HardwareSize size)
    {
        switch(size)
        {
        case HardwareSize::SizeByte:
            return SIZE_1;
        case HardwareSize::SizeWord:
            return SIZE_2;
        case HardwareSize::SizeDword:
            return SIZE_4;
#ifdef _WIN64
        case HardwareSize::SizeQword:
            return SIZE_8;
#endif //_WIN64
        default:
            return SIZE_1;
        }
    }

    static DR7_TYPE type_dr7(HardwareType type)
    {
        switch(type)
        {
        case HardwareType::Access:
            return TYPE_READWRITE;
        case HardwareType::Write:
            return TYPE_WRITE;
        case HardwareType::Execute:
            return TYPE_EXECUTE;
        default:
            return TYPE_EXECUTE;
        }
    }

    bool Thread::SetHardwareBreakpoint(ptr address, HardwareSlot slot, HardwareType type, HardwareSize size)
    {
        //check if the alignment is correct
        if((address % int(size) != 0))
            return false;

        Registers registers(hThread, CONTEXT_DEBUG_REGISTERS);

        //set the address register
        switch(slot)
        {
        case HardwareSlot::Dr0:
            registers.Dr0 = address;
            break;
        case HardwareSlot::Dr1:
            registers.Dr1 = address;
            break;
        case HardwareSlot::Dr2:
            registers.Dr2 = address;
            break;
        case HardwareSlot::Dr3:
            registers.Dr3 = address;
            break;
        default:
            return false;
        }

        //set the Dr7 register
        auto dr7 = ptr_dr7(registers.Dr7());
        auto index = int(slot);
        dr7.DR7_MODE[index] = MODE_LOCAL;
        dr7.DR7_SIZE[index] = size_dr7(size);
        dr7.DR7_TYPE[index] = type_dr7(type);
        registers.Dr7 = dr7_ptr(dr7);

        return true;
    }

    bool Thread::DeleteHardwareBreakpoint(HardwareSlot slot)
    {
        Registers registers(hThread, CONTEXT_DEBUG_REGISTERS);

        //zero the address register
        switch(slot)
        {
        case HardwareSlot::Dr0:
            registers.Dr0 = 0;
            break;
        case HardwareSlot::Dr1:
            registers.Dr1 = 0;
            break;
        case HardwareSlot::Dr2:
            registers.Dr2 = 0;
            break;
        case HardwareSlot::Dr3:
            registers.Dr3 = 0;
            break;
        default:
            return false;
        }

        //set the Dr7 register
        auto dr7 = ptr_dr7(registers.Dr7());
        auto index = int(slot);
        dr7.DR7_MODE[index] = MODE_DISABLED;
        dr7.DR7_SIZE[index] = SIZE_1;
        dr7.DR7_TYPE[index] = TYPE_EXECUTE;
        registers.Dr7 = dr7_ptr(dr7);

        return true;
    }
};