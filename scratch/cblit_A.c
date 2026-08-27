#include "gba/types.h"

typedef void (*CopyFn)(void *dst, void *src, u32 len);
typedef void (*FillFn)(void *dst, u32 len, u32 value);

extern void Func_8001af8(void *dst, void *src, u32 len);
extern void Func_80008d8(void *dst, u32 len, u32 value);
extern void BlitFade_Div2(void *src, void *dst, u32 len);
extern void BlitFade_Div4(void *src, void *dst, u32 len);
extern void BlitFade_Sub(void *src, u32 amt, void *dst, u32 len);
extern void BlitFade_Add(void *src, u32 amt, void *dst, u32 len);
extern unsigned char *iwram_3001eec;

void Task_BlitAnim(void)
{
    unsigned char **pp;
    unsigned char *b;
    void *s;
    CopyFn copy;
    FillFn fill;
    u32 val;

    pp = &iwram_3001eec;
    b = pp[0];
    if (*(int *)(b + 0x7824) == 1) {
        s = pp[1];
        switch (*(int *)(b + 0x7780)) {
        case 0:
            copy = Func_8001af8;
            copy((void *)0x6004000, s, 0x4000);
            break;
        case 1:
            copy = Func_8001af8;
            copy((void *)0x6004000, s, 0x4000);
            val = *(u32 *)(b + 0x7784);
            fill = Func_80008d8;
            fill(s, 0x4000, val);
            break;
        case 2:
            if (*(int *)(b + 0x7784) == 0x32)
                BlitFade_Div2(s, (void *)0x6004000, 0x4000);
            else
                BlitFade_Div4(s, (void *)0x6004000, 0x4000);
            break;
        case 3:
            BlitFade_Sub(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        case 4:
            BlitFade_Add(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        }
        *(int *)(b + 0x7824) = 0;
        *(int *)(b + 0x7820) = 1;
    } else {
        *(int *)(b + 0x7820) = *(int *)(b + 0x7820) + 1;
    }
}
