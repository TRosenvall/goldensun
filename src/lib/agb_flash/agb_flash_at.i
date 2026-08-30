# 1 "src/lib/agb_flash/agb_flash_at.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 385 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "src/lib/agb_flash/agb_flash_at.c" 2
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
# 1 "tools/agbcc/include/stdint.h" 1
# 31 "tools/agbcc/include/stdint.h"
# 1 "tools/agbcc/include/limits.h" 1
# 32 "tools/agbcc/include/stdint.h" 2



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
# 2 "src/lib/agb_flash/agb_flash_at.c" 2
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
# 3 "src/lib/agb_flash/agb_flash_at.c" 2








u16 Func_8006f84(u16 sectorNum, u8 *src)
{
    u16 result;
    u8 *dest;
    u16 readFlash1Buffer[0x20];

    if (sectorNum >= 16)
        return 0x80FF;

    result = EraseFlashSector_MX(sectorNum);
    if (result != 0)
        return result;

    SetReadFlash1(readFlash1Buffer);
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlash->wait[0];
    gFlashNumRemainingBytes = gFlash->sector.size;
    dest = ((u8 *)0xE000000) + (sectorNum << gFlash->sector.shift);

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

u16 EraseFlashChip_AT(void)
{
    u16 result;
    u16 readFlash1Buffer[0x20];

    SetReadFlash1(readFlash1Buffer);
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlashChip3.type.wait[0];

    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x80));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0x10));

    result = WaitForFlashWrite(3, ((u8 *)0xE000000), 0xFF);

    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}

u16 EraseFlashSector_AT_2(u16 page)
{
    u8 *dest;
    u32 i;
    u16 savedIme;
    u16 result;

    dest = ((u8 *)0xE000000) + (page << gFlashChip3.type.sector.shift);

    savedIme = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;

    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xA0));

    i = gFlashChip3.type.sector.size;
    while (i != 0) {
        *dest++ = 0xFF;
        i--;
    }

    dest--;
    (*(vu16 *)(0x4000000 + 0x208)) = savedIme;

    result = WaitForFlashWrite(1, dest, 0xFF);
    if (result != 0)
        result = (result & 0xFF00) | 2;

    return result;
}

u16 EraseFlashSector_AT(u16 sectorNum)
{
    u16 result;
    u16 j;
    u16 page;
    u16 i;
    u16 readFlash1Buffer[0x20];

    if (sectorNum >= 16)
        return 0x80FF;

    SetReadFlash1(readFlash1Buffer);
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlashChip3.type.wait[0];

    page = sectorNum;
    page <<= 5;
    for (i = 0; i <= 0x1f; i++) {
        for (j = 2; j != 0; j--) {
            result = EraseFlashSector_AT_2(page);
            if (result == 0)
                break;
        }
        page++;
        if (result != 0)
            break;
    }
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}

u16 ProgramFlashSector_AT_2(u16 page, u8 *src)
{
    u8 *dest;
    u32 i;
    u16 savedIme;

    dest = ((u8 *)0xE000000) + (page << gFlashChip3.type.sector.shift);

    savedIme = (*(vu16 *)(0x4000000 + 0x208));
    (*(vu16 *)(0x4000000 + 0x208)) = 0;

    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xAA));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x2AAA))) = (0x55));
    ((*(vu8 *)(((u8 *)0xE000000) + (0x5555))) = (0xA0));

    i = gFlashChip3.type.sector.size;
    while (i != 0) {
        *dest++ = *src++;
        i--;
    }

    dest--;
    src--;
    (*(vu16 *)(0x4000000 + 0x208)) = savedIme;

    return WaitForFlashWrite(1, dest, *src);
}

u16 ProgramFlashSector_AT(u16 sectorNum, u8 *src)
{
    u16 result;
    u16 j;
    u16 page;
    u16 readFlash1Buffer[0x20];

    if (sectorNum >= 16)
        return 0x80FF;

    SetReadFlash1(readFlash1Buffer);
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | gFlashChip3.type.wait[0];

    page = sectorNum << 5;
    for (gFlashNumRemainingBytes = gFlashChip2.type.sector.size; gFlashNumRemainingBytes != 0;
         gFlashNumRemainingBytes -= gFlashChip3.type.sector.size, src += gFlashChip3.type.sector.size, page++) {
        for (j = 2; j != 0; j--) {
            result = ProgramFlashSector_AT_2(page, src);
            if (result == 0)
                break;
        }
        if (result != 0)
            break;
    }
    (*(vu16 *)(0x4000000 + 0x204)) = ((*(vu16 *)(0x4000000 + 0x204)) & ~(3 << 0)) | (3 << 0);

    return result;
}
