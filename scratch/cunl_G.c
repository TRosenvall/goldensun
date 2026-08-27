#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);

extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern void *galloc_iwram(s32 tag, s32 size);
extern void AnimStart(s32 n);
extern void StartTask(void *fn, s32 arg);
extern void Func_80cc960(void);
extern void Task_BlitAnim(void);

void Anim_UnleashIntro(s32 kind)
{
    u8 *buf;
    u8 *data;
    u32 pal;
    u8 *dst;
    CopyFn copy;
    s32 id;
    s32 len;
    s32 arg;

    buf = galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    AnimStart(0);
    *(s32 *)(buf + 0x77b4) = 0x18;
    REG_BG2PA = 0x100;
    REG_BLDALPHA = 0x1010;
    switch (kind) {
    case 0:
        id = FILE_c8;
        break;
    case 1:
        id = FILE_cf;
        break;
    case 2:
        id = FILE_b4;
        break;
    case 3:
        id = FILE_cb;
        break;
    case 4:
    default:
        id = FILE_be;
        break;
    }
    data = GetFile(id);
    pal = 0xa0;
    copy = Func_8001af8;
    len = 0x80;
    dst = (u8 *)(pal << 19);
    copy((volatile u16 *)dst, data, len);
    *(s32 *)(buf + 0x778c) = 0;
    *(s32 *)(buf + (0xef << 7)) = 3;
    arg = 0xc8;
    *(s32 *)(buf + 0x7784) = 0x6060606;
    arg <<= 4;
    StartTask(Func_80cc960, arg);
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
}
