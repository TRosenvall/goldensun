# 0 "src/lib/agb_flash/agb_flash.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "src/lib/agb_flash/agb_flash.c"
# 1 "include/gba/gba.h" 1







# 1 "include/gba/types.h" 1



typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long int u64;
# 16 "include/gba/types.h"
typedef char s8;

typedef short s16;
typedef int s32;
typedef long long int s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float f32;
typedef double f64;

typedef u8 bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8 vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;



typedef s32 fx32;
typedef s16 fx16;

typedef struct {
    fx32 x, y;
} vec2_t;

typedef struct {
    fx32 x, y, z;
} vec3_t;

typedef fx32 matrix_t[4][3];
# 9 "include/gba/gba.h" 2
# 1 "/opt/agbcc/include/stdint.h" 1
# 31 "/opt/agbcc/include/stdint.h"
# 1 "/opt/agbcc/include/limits.h" 1
# 32 "/opt/agbcc/include/stdint.h" 2



typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;



typedef signed char int_least8_t;
typedef short int_least16_t;
typedef int int_least32_t;
typedef long long int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;



typedef int int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
typedef long long int_fast64_t;
typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;



typedef int intptr_t;
typedef unsigned int uintptr_t;



typedef long long intmax_t;
typedef unsigned long long uintmax_t;
# 10 "include/gba/gba.h" 2
# 1 "include/gba/defines.h" 1
# 11 "include/gba/gba.h" 2
# 1 "include/gba/io.h" 1
# 12 "include/gba/gba.h" 2
# 1 "include/gba/syscall.h" 1
# 13 "include/gba/syscall.h"
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
# 13 "include/gba/gba.h" 2
# 1 "include/gba/cpuset_macros.h" 1
# 14 "include/gba/gba.h" 2
# 2 "src/lib/agb_flash/agb_flash.c" 2
# 1 "include/lib/agb_flash/flash_internal.h" 1
# 18 "include/lib/agb_flash/flash_internal.h"
struct FlashSector {
    u32 size;
    u8 shift;
    u16 count;
    u16 top;
};

struct FlashType {
    u32 romSize;
    struct FlashSector sector;
    u16 wait[2];
    union {
        struct {
            u8 makerId;
            u8 deviceId;
        } separate;
        u16 joined;
    } ids;
};




struct FlashSetupInfo {
    u16 (*programFlashSector)(u16, void *);
    u16 (*eraseFlashChip)(void);
    u16 (*eraseFlashSector)(u16);
    u16 (*WaitForFlashWrite)(u8, u8 *, u8);
    const u16 *maxTime;
    struct FlashType type;
};




struct FlashChipSetup {
    const void *header[5];
    struct FlashType type;
};


extern const struct FlashType *gFlash;
extern u16 gFlashNumRemainingBytes;
extern u16 (*ProgramFlashSector)(u16, void *);
extern u16 (*EraseFlashChip)(void);
extern u16 (*EraseFlashSector)(u16);
extern u16 (*WaitForFlashWrite)(u8, u8 *, u8);
extern u8 gFlashTimeoutFlag;


extern const struct FlashSetupInfo *const gSetup512KInfos[];
extern const struct FlashSetupInfo DefaultFlash512K;
extern const struct FlashChipSetup gFlashChip2;
extern const struct FlashChipSetup gFlashChip3;


u16 ReadFlashId(void);
u16 IdentifyFlash(void);
void FlashTimerIntr(void);
u16 SetFlashTimerIntr(u8 timerNum, void (**intrFunc)(void));
void StartFlashTimer(u8 phase);
void StopFlashTimer(void);
u8 ReadFlash1(u8 *addr);
void SetReadFlash1(u16 *dest);
u16 WaitForFlashWrite_Common(u8 phase, u8 *addr, u8 lastData);
void ReadFlash_Core(u8 *src, u8 *dest, u32 size);
void ReadFlash(u16 sectorNum, u32 offset, void *dest, u32 size);
u32 VerifyFlashSector_Core(u8 *src, u8 *tgt, u16 size);
u32 VerifyFlashSector(u16 sectorNum, u8 *src);

u16 EraseFlashChip_MX(void);
u16 EraseFlashSector_MX(u16 sectorNum);
u16 ProgramByte(u8 *src, u8 *dest);
u16 ProgramFlashSector_MX(u16 sectorNum, u8 *src);
u32 VerifyEraseSector_Core(u8 *addr);

u16 VerifyEraseSector(u8 *addr, u32 (*verify)(u8 *));

u16 Func_8006f84(u16 sectorNum, u8 *src);
u16 EraseFlashChip_AT(void);
u16 EraseFlashSector_AT_2(u16 page);
u16 EraseFlashSector_AT(u16 sectorNum);
u16 ProgramFlashSector_AT_2(u16 page, u8 *src);
u16 ProgramFlashSector_AT(u16 sectorNum, u8 *src);
# 3 "src/lib/agb_flash/agb_flash.c" 2


extern u8 sTimerNum;
extern u16 sTimerCount;
extern vu16 *gTimerReg;
extern u16 gSavedIme;
extern const u16 *gFlashMaxTime;

extern u8 (*PollFlashStatus)(u8 *);
# 20 "src/lib/agb_flash/agb_flash.c"
u16 ReadFlashId(void)
{
    u16 flashId;
    u16 readFlash1Buffer[0x20];
    u8 (*readFlash1)(u8 *);

    SetReadFlash1(readFlash1Buffer);
    readFlash1 = (u8 (*)(u8 *))((intptr_t)readFlash1Buffer + 1);


    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x90));
    do { vu16 i; for (i = 20000; i != 0; i--) ; } while (0);

    flashId = readFlash1(((u8 *)0xE000000) + 1) << 8;
    flashId |= readFlash1(((u8 *)0xE000000));


    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xF0));



    do { vu16 i; for (i = 20000; i != 0; i--) ; } while (0);

    return flashId;
}

u16 IdentifyFlash(void)
{
    u16 result;
    u16 flashId;
    const struct FlashSetupInfo *const *setupInfo;

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    flashId = ReadFlashId();

    setupInfo = gSetup512KInfos;
    result = 1;

    for (;;) {
        if ((*setupInfo)->type.ids.separate.makerId == 0)
            break;

        if (flashId == (*setupInfo)->type.ids.joined) {
            result = 0;
            break;
        }

        setupInfo++;
    }

    ProgramFlashSector = (*setupInfo)->programFlashSector;
    EraseFlashChip = (*setupInfo)->eraseFlashChip;
    EraseFlashSector = (*setupInfo)->eraseFlashSector;
    WaitForFlashWrite = (*setupInfo)->WaitForFlashWrite;
    gFlashMaxTime = (*setupInfo)->maxTime;
    gFlash = &(*setupInfo)->type;

    return result;
}

void FlashTimerIntr(void)
{
    if (sTimerCount != 0)
        if (--sTimerCount == 0)
            gFlashTimeoutFlag = 1;
}

u16 SetFlashTimerIntr(u8 timerNum, void (**intrFunc)(void))
{
    if (timerNum >= 4)
        return 1;

    sTimerNum = timerNum;
    gTimerReg = &(*(vu32 *)((0x4000000 + 0x100) + ((sTimerNum) * 4)));
    *intrFunc = FlashTimerIntr;
    return 0;
}

void StartFlashTimer(u8 phase)
{
    const u16 *maxTime = &gFlashMaxTime[phase * 3];
    gSavedIme = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    (*(vu16 *)(0x4000000 + 0x200)) |= ((1 << 3) << sTimerNum);
    (*(vu16 *)(0x4000000 + 0x208)) = 1;
    gFlashTimeoutFlag = 0;
    sTimerCount = *maxTime++;
    *gTimerReg++ = *maxTime++;
    *gTimerReg-- = *maxTime++;
}

void StopFlashTimer(void)
{
    *gTimerReg++ = 0;
    *gTimerReg-- = 0;
    (*(vu16 *)(0x4000000 + 0x208)) = 0;
    (*(vu16 *)(0x4000000 + 0x200)) &= ~((1 << 3) << sTimerNum);
    (*(vu16 *)(0x4000000 + 0x208)) = gSavedIme;
}



u8 ReadFlash1(u8 *addr) { return *addr; }

void SetReadFlash1(u16 *dest)
{
    u16 *src;
    u16 i;

    PollFlashStatus = (u8 (*)(u8 *))((intptr_t)dest + 1);

    src = (u16 *)ReadFlash1;
    src = (u16 *)((intptr_t)src ^ 1);

    i = ((intptr_t)SetReadFlash1 - (intptr_t)ReadFlash1) >> 1;

    while (i != 0) {
        *dest++ = *src++;
        i--;
    }
}

u16 WaitForFlashWrite_Common(u8 phase, u8 *addr, u8 lastData)
{
    u16 result = 0;

    StartFlashTimer(phase);

    while (PollFlashStatus(addr) != lastData) {
        if (gFlashTimeoutFlag) {
            if (PollFlashStatus(addr) == lastData)
                break;

            if (gFlash->ids.joined == 0x1CC2)
                ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xF0));

            result = phase | 0xC000u;
            break;
        }
    }

    StopFlashTimer();

    return result;
}

void ReadFlash_Core(u8 *src, u8 *dest, u32 size)
{
    while (size-- != 0) {
        *dest++ = *src++;
    }
}

void ReadFlash(u16 sectorNum, u32 offset, void *dest, u32 size)
{
    u8 *src;
    u16 i;
    u16 readFlash_Core_Buffer[0x40];
    u16 *funcSrc;
    u16 *funcDest;
    void (*readFlash_Core)(u8 *, u8 *, u32);

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);
# 196 "src/lib/agb_flash/agb_flash.c"
    funcSrc = (u16 *)ReadFlash_Core;
    funcSrc = (u16 *)((intptr_t)funcSrc ^ 1);
    funcDest = readFlash_Core_Buffer;

    i = ((intptr_t)ReadFlash - (intptr_t)ReadFlash_Core) >> 1;

    while (i != 0) {
        *funcDest++ = *funcSrc++;
        i--;
    }

    readFlash_Core = (void (*)(u8 *, u8 *, u32))((intptr_t)readFlash_Core_Buffer + 1);


    src = ((u8 *)0xE000000) + (sectorNum << DefaultFlash512K.type.sector.shift) + offset;




    readFlash_Core(src, dest, size);
}

u32 VerifyFlashSector_Core(u8 *src, u8 *tgt, u16 size)
{
    while (size-- != 0) {
        if (*tgt++ != *src++)
            return (uintptr_t)(tgt - 1);
    }

    return 0;
}

u32 VerifyFlashSector(u16 sectorNum, u8 *src)
{
    u16 i;
    u16 verifyFlashSector_Core_Buffer[0x80];
    u16 *funcSrc;
    u16 *funcDest;
    u8 *tgt;
    u16 size;
    u32 (*verifyFlashSector_Core)(u8 *, u8 *, u32);

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);
# 247 "src/lib/agb_flash/agb_flash.c"
    funcSrc = (u16 *)VerifyFlashSector_Core;
    funcSrc = (u16 *)((intptr_t)funcSrc ^ 1);
    funcDest = verifyFlashSector_Core_Buffer;

    i = ((intptr_t)VerifyFlashSector - (intptr_t)VerifyFlashSector_Core) >> 1;

    while (i != 0) {
        *funcDest++ = *funcSrc++;
        i--;
    }

    verifyFlashSector_Core = (u32 (*)(u8 *, u8 *, u32))((intptr_t)verifyFlashSector_Core_Buffer + 1);


    tgt = ((u8 *)0xE000000) + (sectorNum << DefaultFlash512K.type.sector.shift);
    size = DefaultFlash512K.type.sector.size;





    return verifyFlashSector_Core(src, tgt, size);
}
