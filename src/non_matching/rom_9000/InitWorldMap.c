/* InitWorldMap -- 0x080109e8, asm/rom_9000/rom_108e4_a.s
 *
 * NON-MATCHING, 284 of 365 encodings differ.
 *
 * SIZE  ref 864 bytes, ours 876   (ours is 12 bytes LONG)
 * COUNT ref 365 encodings, ours 366.  The reference's own instruction count is
 *       331; objcmp's 365 counts the 33 pool words as encodings.  The counts are
 *       NOT equal, so objcmp's 284 is SATURATED and ranks nothing -- use
 *       tools/aligncmp.py: this candidate reads aligned-equal 230 (63.0% of ref),
 *       201 in 64 hunks.
 * RELOCATIONS: 34 THM_CALL and 18 ABS32, the right symbols, but the SEQUENCE
 *       differs in two real ways (see the residue below): our pool is SPLIT, so
 *       seven ABS32 words land mid-function, and one indirect call goes through
 *       r11 (`_call_via_fp`) where the ROM's goes through r3.
 *
 * NO SHIMS, NO PINS, NO asm (tools/shimcount.py: silent).  No fakematch row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/InitWorldMap.c \
 *     asm/rom_9000/rom_108e4_a.s --func InitWorldMap
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_9000/InitWorldMap.c \
 *     asm/rom_9000/rom_108e4_a.s InitWorldMap -v
 *
 * SPLIT SHAPE.  tools/datacheck.py is SILENT on rom_108e4_a.s -- no data section,
 * so a plain function cut.  The .s holds two functions and InitWorldMap is the
 * SECOND:
 *     asm/rom_9000/rom_108e4_a_a.s   Func_80108e4 (LoadTilesetQuadrant)
 *     src/rom_9000/rom_108e4_a_b.c   InitWorldMap
 * NO exports.s change: `.export_func InitWorldMap` at src/rom_9000/exports.s:41
 * is a `_InitWorldMap` thumb stub and is indifferent to which object defines the
 * target, and neither function calls the other.
 * WATCH stage1.ld: rom_108e4_a.o appears TWICE, at line 224 in `.text` and at
 * line 291 in `.rodata`, even though the .s has no data.  BOTH lines have to
 * become two, in the same order, or the .rodata placement silently changes for
 * the neighbouring _b / _c objects.  datacheck.py does not see this, because it
 * reads the .s and the .s is clean; the second line is only in the script.
 *
 * THE REFERENCE'S PROSE DESCRIBES THE PROLOGUE AND CALLS IT THE FUNCTION.
 * Everything it says is true and it covers roughly the first 80 of 331
 * instructions: the 0xC1FF DISPCNT mask, Func_3bb4(0), the 0x35C-byte tag-8
 * allocation and its DMA zero-fill, the camera and bounds seeds at +0xE4..+0xF8,
 * resources 0xD4 and 0xD6.  Then it says "Finally sets REG_BLDCNT to 0x3F9E,
 * REG_BLDALPHA to 0x1010 and REG_BLDY to 0" -- and that is instruction ~80.
 * What follows and is NOT mentioned at all:
 *   * two more resources (0xD5 -> gBuffer, 0xD7 -> ewram_202c000);
 *   * three BGxCNT writes and BOTH affine matrices (BG2PA..BG2Y, BG3PA..BG3Y)
 *     set to identity, reached as one walking base register;
 *   * a 0x4C-byte EWRAM camera block (tag 0xC) and a 0x3484-byte IWRAM buffer
 *     (tag 7);
 *   * the whole 3-D set-up: Func_8005258, InitMatrixStack / MatrixTranslatev /
 *     MatrixYaw / MatrixPitch / MatrixSetLook, three indirect calls
 *     (Func_80009c0 twice, Func_80008ac once), cos/sin, Func_80123f4, and a
 *     fourth indirect call through gPtrs[0x2E] with a FOUR-argument signature;
 *   * a DMA3 copy of Func_800a0f8 (0x284 bytes) into a fresh tag-0x2E block --
 *     i.e. code copied to RAM to be run from there, as UnpackTilemap does;
 *   * two StartTask calls (Func_80111b4 tag 0xC85, Func_8010ff0 tag 0x480);
 *   * a 256-entry descending fill of the halfword table at +0x138..+0x337 with
 *     0..0xFF.
 * So "NAMED rather than numbered means the behaviour is understood" does not hold
 * here: the name is right and the note is a third of a function.
 *
 * WHAT IS ALREADY EXACT.  The first 80 instructions are instruction-for-
 * instruction the ROM's, including two details that look like defects and are
 * not: DISPCNT's address is materialised TWICE (`mov #0x80 / lsl #19` for the
 * read and again for the write) because it is `volatile`, and the BGxCNT /
 * affine block is one base register walked with `add r3,#2` / `sub r3,#2`, which
 * falls out of writing the REG_ macros as separate statements -- that is
 * reload_cse_move2add, not something to spell.  Note this tree's gba.inc puts
 * BG1CNT at 0x0400000A, BG2CNT at 0x0400000C and BG3CNT at 0x0400000E, so the
 * `add r3, #0x16` after the three CNT stores lands on BG2PA; reading them as the
 * conventional 0x08/0x0A/0x0C puts every later store two bytes low and makes the
 * two 32-bit BG2X / BG3X writes look misaligned.
 * `gPtrs[0xb8 / 4]` is the fourth indirect call's target, taken from a table:
 * `ldr r1,=gPtrs / add r1,#0xb8 / ldr r4,[r1]`, and its fourth argument is
 * `blk + (iwram_3001e40 & 1) * 0x1400` where `blk = buf + 0xC80`.
 *
 * THE RESIDUE, ITEMISED.
 *   (a) OUR POOL SPLITS AND THE ROM'S DOES NOT.  gcc emits `b.n` barriers and
 *       dumps constants at +0x108 and +0x154; the ROM carries ONE pool from
 *       +0x2DC to the end.  That alone is most of the 12 extra bytes: we hold 38
 *       pool words against the ROM's 33 and 294 halfword instructions against
 *       298.  Range is not the reason -- the ROM's furthest reach is 712 bytes
 *       against Thumb's 1020.
 *   (b) THE 0x284 DMA COUNT IS NOT FOLDED IN THE ROM.  It spends
 *       `ldr r5,=0x284 / mov r2,#0x84 / lsr r5,#2 / lsl r2,#24 / orr r2,r5`,
 *       five instructions, where gcc constant-folds `0x84000000 | (0x284/4)` to
 *       one pooled word.  Naming the size in a local does NOT stop the fold
 *       (measured, 0) -- gcc-2.96 propagates it.  Five instructions and one pool
 *       word of the delta live here.  The SAME shape blocks LoadMapData's
 *       0x194 (see src/non_matching/rom_9000/LoadMapData.c), so this is a class,
 *       not a one-off: A CONSTANT THE ROM KEEPS IN A REGISTER ACROSS TWO USES.
 *   (c) ONE EXTRA STACK SLOT.  `sub sp, #36` against the ROM's `#32`, and the
 *       DMA3_CLEAR zero word sits at `sp+20` against the ROM's `sp+28`.  The
 *       ROM's 32 bytes are exactly four spill slots + a 12-byte vec3 + the zero
 *       word, and its four slots are `&st->yaw`, `&st->f34c`, `blk` and `buf`.
 *       We spill five things.
 *   (d) `_call_via_fp` for Func_80008ac where the ROM has `_call_via_r3`.  This
 *       is NOT the known `_call_via_sl`/`_call_via_r10` name aliasing -- r11 and
 *       r3 are different registers.  Assigning the pointer AFTER both cos and
 *       sin returns fixes the register and trades badly overall: 364 encodings
 *       and 872 bytes (both CLOSER) but 60.5% aligned (worse) --
 *       scratch_elev/b300h/iwm_E1.c.  Worth revisiting once (a) and (b) are out.
 *
 * MEASURED, all against 63.0% / 366 encodings / 876 bytes:
 *   a named pointer for the ewram_202d000 destination           63.0 -> 52.6%
 *   named pointers `&st->yaw` / `&st->pitch` (the ROM's sp+0    63.0 -> 49.0%
 *     and r11 slots)                                           and 368 enc
 *   a named pointer `&st->f34c` (the ROM's sp+4 slot)           0, byte-identical
 *   a named local for the 0x284 galloc/DMA size                 0, byte-identical
 *   the fp2 assignment moved below cos/sin                      63.0 -> 60.5%,
 *                                                   size 876 -> 872, count 366 -> 364
 * The two that read 0 are interesting: they are the ROM's OWN stack layout and
 * gcc reaches it unprompted, which is a check on (c) -- the extra slot is not one
 * of those two.
 *
 * NEXT.  (b) first: it is five instructions and a pool word, it is shared with
 * LoadMapData, and until the constant stays in a register the pool contents are
 * wrong, which makes (a) unmeasurable.  Then (c).  (d) last.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "file_table.h"

struct World {
    u8 pad00[0x10];
    int f10;
    u16 f14;
    u8 f16;
    u8 pad17[0xe4 - 0x17];
    int camx;
    int camy;
    int bx;
    int by;
    int bz;
    int bw;
    u8 padfc[0x100 - 0xfc];
    u16 f100;
    u16 f102;
    u8 pad104[0x110 - 0x104];
    void *f110;
    u8 pad114[0x118 - 0x114];
    u16 pitch;
    u16 yaw;
    u8 pad11c[0x138 - 0x11c];
    u16 tbl[256];
    u8 pad338[0x348 - 0x338];
    int f348;
    int f34c;
    u8 pad350[0x354 - 0x350];
    int f354;
    u16 f358;
};

struct Cam {
    u8 pad0[0xc];
    vec3_t pos;
    u8 pad18[0x20 - 0x18];
};

extern int gPhysVec[];
extern void *gPtrs[];
extern u8 gBuffer[];
extern u8 ewram_202c000[];
extern u8 ewram_202d000[];
extern int iwram_3001f60;
extern int iwram_3001af4;
extern int iwram_3001e40;
extern u16 iwram_3001ad0[];

extern void Func_8003bb4(int a);
extern void *galloc_iwram(s32 tag, s32 size);
extern void *galloc_ewram(s32 tag, s32 size);
extern void DecompressLZ(void *src, void *dst);
extern void Func_80118d8(void *p);
extern void Func_8005258(int a, int b, int c);
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void MatrixSetLook(void *a, vec3_t *b);
extern int cos(int a);
extern int sin(int a);
extern void Func_80123f4(int a, vec3_t *v, void *b);
extern void Func_80111b4(void);
extern void Func_8010ff0(void);
extern void Func_800a0f8(void);
extern void Func_80009c0(vec3_t *a, void *b);
extern int Func_80008ac(int a, int b);

void InitWorldMap(void)
{
    struct World *st;
    struct Cam *cam;
    void *buf;
    void *blk;
    vec3_t v;
    vec3_t *cp;
    void (*fp)(vec3_t *, struct Cam *);
    int (*fp2)(int, int);
    void (*fp3)(struct Cam *, vec3_t *, void *, void *);
    int q;
    int i;
    u16 *p;

    REG_DISPCNT = REG_DISPCNT & 0xc1ff;
    Func_8003bb4(0);
    st = galloc_iwram(8, 0xd7 << 2);
    DMA3_CLEAR(st, 0xd7 << 2);
    st->camx = 0;
    st->camy = 0;
    st->bx = 0x80 << 14;
    st->by = 0x80 << 15;
    st->bz = 0xff << 21;
    st->bw = 0xff << 21;
    st->f10 = 0;
    st->f110 = GetFile(FILE_d4);
    DecompressLZ(GetFile(FILE_d6), ewram_202d000);
    Func_80118d8(ewram_202d000);
    REG_BLDCNT = 0x3f9e;
    REG_BLDALPHA = 0x1010;
    REG_BLDY = 0;
    DecompressLZ(GetFile(FILE_d5), gBuffer);
    DecompressLZ(GetFile(FILE_d7), ewram_202c000);
    st->f14 = 0xf8 << 5;
    st->f16 = 0x80;
    REG_BG3CNT = 0xa80a;
    REG_BG2CNT = 0xaa0e;
    REG_BG1CNT = 0x501;
    REG_BG2PA = 0x80 << 1;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0x80 << 1;
    REG_BG2X = 0;
    REG_BG2Y = 0;
    REG_BG3PA = 0x80 << 1;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 0x80 << 1;
    REG_BG3X = 0;
    REG_BG3Y = 0;
    cam = galloc_ewram(0xc, 0x4c);
    buf = galloc_iwram(7, 0x3484);
    cp = &cam->pos;
    blk = (char *)buf + (0xc8 << 4);
    q = 0xff << 17;
    st->f348 = q;
    st->f34c = q;
    st->f354 = 0x80 << 9;
    st->f358 = 0;
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x60;
    Func_8005258(q, q >> 1, q << 1);
    cp->x = 0;
    cp->y = 0;
    cp->z = 0;
    InitMatrixStack();
    MatrixTranslatev(cp);
    MatrixYaw(st->yaw);
    MatrixPitch(st->pitch);
    v.x = 0;
    v.y = 0;
    v.z = q;
    fp = Func_80009c0;
    fp(&v, cam);
    InitMatrixStack();
    MatrixSetLook(cam, cp);
    DMA3_COPY(Func_800a0f8, galloc_iwram(0x2e, 0x284), 0x284);
    fp2 = Func_80008ac;
    Func_80123f4(fp2(cos(st->pitch), sin(st->pitch)), cp, buf);
    iwram_3001f60 = 0;
    iwram_3001af4 = st->pitch;
    fp3 = gPtrs[0xb8 / 4];
    fp3(cam, cp, buf, (char *)blk + (iwram_3001e40 & 1) * 0x1400);
    cp->x = 0;
    cp->y = 0;
    cp->z = 0;
    InitMatrixStack();
    st->pitch = 0xe0 << 8;
    st->yaw = 0;
    InitMatrixStack();
    MatrixTranslatev(cp);
    MatrixYaw(st->yaw);
    MatrixPitch(st->pitch);
    v.x = 0;
    v.y = 0;
    v.z = st->f34c + (0x80 << 9);
    fp = Func_80009c0;
    fp(&v, cam);
    REG_MOSAIC = 0;
    REG_DISPCNT = 0x42;
    iwram_3001ad0[2] = 0;
    iwram_3001ad0[3] = 0;
    iwram_3001ad0[4] = 0;
    iwram_3001ad0[5] = 0;
    iwram_3001ad0[6] = 0;
    iwram_3001ad0[7] = 0;
    st->f100 = 0;
    st->f102 = 0x9f;
    StartTask(Func_80111b4, 0xc85);
    StartTask(Func_8010ff0, 0x90 << 3);
    p = &st->tbl[255];
    for (i = 0xff; i >= 0; i--) {
        *p = i;
        p--;
    }
}
