# 0 "src/lib/agb_flash/agb_flash_mx.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "src/lib/agb_flash/agb_flash_mx.c"
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
# 2 "src/lib/agb_flash/agb_flash_mx.c" 2
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
# 3 "src/lib/agb_flash/agb_flash_mx.c" 2







u16 EraseFlashChip_MX(void)
{
    u16 result;
    u16 readFlash1Buffer[0x20];

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlash->wait[0];

    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x80));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x10));

    SetReadFlash1(readFlash1Buffer);

    result = WaitForFlashWrite(3, ((u8 *)0xE000000), 0xFF);

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}

u16 EraseFlashSector_MX(u16 sectorNum)
{
    u16 result;
    u8 *addr;
    u16 readFlash1Buffer[0x20];

    if (sectorNum >= 16)
        return 0x80FF;

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlash->wait[0];

    addr = ((u8 *)0xE000000) + (sectorNum << gFlash->sector.shift);

    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x80));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    *addr = 0x30;

    SetReadFlash1(readFlash1Buffer);

    result = WaitForFlashWrite(2, addr, 0xFF);

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}

u16 ProgramByte(u8 *src, u8 *dest)
{
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xA0));
    *dest = *src;

    return WaitForFlashWrite(1, dest, *src);
}

u16 ProgramFlashSector_MX(u16 sectorNum, u8 *src)
{
    u16 result;
    u8 *dest;
    u8 numTries;
    u8 numErases;
    u8 j;
    u16 n;
    u16 *funcSrc;
    u16 *funcDest;
    u16 verifyBuffer[0x30];

    if (sectorNum >= 16)
        return 0x80FF;

    dest = ((u8 *)0xE000000) + (sectorNum << gFlash->sector.shift);

    funcSrc = (u16 *)((intptr_t)VerifyEraseSector_Core ^ 1);
    funcDest = verifyBuffer;
    n = (intptr_t)VerifyEraseSector - (intptr_t)VerifyEraseSector_Core;
    while (n != 0) {
        *funcDest++ = *funcSrc++;
        n -= 2;
    }

    numTries = 0;
    for (;;) {
        result = EraseFlashSector_MX(sectorNum);
        if (result == 0) {
            result = VerifyEraseSector(dest, (u32 (*)(u8 *))((intptr_t)verifyBuffer + 1));
            if (result == 0)
                break;
        }
        if (++numTries == 0x51)
            return result;
    }

    numErases = 1;
    if (numTries != 0)
        numErases = 6;
    for (j = 1; j <= numErases; j++)
        EraseFlashSector_MX(sectorNum);

    SetReadFlash1(verifyBuffer);
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlash->wait[0];
    gFlashNumRemainingBytes = gFlash->sector.size;
    while (gFlashNumRemainingBytes != 0) {
        result = ProgramByte(src, dest);
        if (result != 0)
            break;
        gFlashNumRemainingBytes--;
        src++;
        dest++;
    }
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}



u32 VerifyEraseSector_Core(u8 *addr)
{
    u32 size = gFlash->sector.size;

    while (size != 0) {
        if (*addr++ != 0xFF)
            break;
        size--;
    }

    return size;
}
