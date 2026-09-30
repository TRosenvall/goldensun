/* Func_802592c -- NON-MATCHING, 795 of 838 encodings differ.
 * (objcmp at PRODUCTION FLAGS, pre-split reference, --func Func_802592c.)
 *
 * SIZE IS NOT EXACT: ref 1876 bytes, ours 1896 -- we are 20 bytes LONG.
 * COUNT IS NOT EXACT: ref 838 encodings, ours 849 -- 11 instructions LONG.
 * So the 795 SATURATES; rank with the aligncmp figure.
 * aligncmp: 438 aligned-equal = 52.3 percent of ref; 547 differing/ins/del in
 * 163 hunks.
 * PIN-FREE: tools/shimcount.py reports nothing at all.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/802592c.c \
 *       asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s --func Func_802592c
 * AFTER THE SPLIT IS INSTALLED the reference path becomes
 *       asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.s
 *
 * THE SPLIT SHAPE -- RUN THE SPLIT FOR THIS FUNCTION AND BOTH TARGETS ARE READY.
 * tools/split_s.py --dry-run asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s Func_802592c
 *   ..._a.s   1 function,  873 lines  <- Func_8025200
 *   ..._b.s   1 function,  892 lines  <- Func_802592c   (this file)
 *   ..._c.s   2 functions, 1949 lines <- Func_8026080, Func_8026e80
 *   REMOVE asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s ; rewrite stage1.ld
 * Confirm make compare is still GREEN after the split and BEFORE any .c lands --
 * a layout mistake and a bad decompilation look identical at the end.
 *
 * ================ WHAT TRANSFERRED FROM Func_8025200 ================
 *
 * THE DUPLICATE HYPOTHESIS STAYS NEGATIVE (47.2 percent opcode-sequence ratio,
 * measured in the batch-307 recon) -- these are two reconstructions.  But the
 * 94-INSTRUCTION SHARED RUN IS ONE SOURCE and was written ONCE:
 * everything from the prologue through the four Func_80251d4 calls -- the
 * iwram_3001e8c read, the two -1 sentinels, AllocUploadSpriteGFX, the first
 * CreateUIBox, the ctx->f34/f30/f38 triple, the second CreateUIBox, the five-OBJ
 * setup loop with its union Ent *e walker, the five-sprite upload loop, and the
 * 0xf018/0xf019 pairs -- is character-for-character the same in both files,
 * differing only in the CreateUIBox arguments.  It came out right on the FIRST
 * compile here, which is the leverage the brief asked for.
 *
 * ================ WHAT DOES NOT TRANSFER -- READ THE PREAMBLE ================
 *
 * (1) nitems IS INITIALIZED TO 5 HERE AND TO 0 IN Func_8025200.
 *     mov r2,#5 / str r2,[sp,#0x28] right after the first CreateUIBox.  Do not
 *     copy the sibling's initializer; read the preamble.
 * (2) cx IS (boxB->x << 3) - 4 HERE AND - 2 THERE.
 * (3) THERE IS A BARE int slot; AT sp+0x4c, BELOW the objs array at sp+0x50.
 *     It is LoadMoveIcon's out-parameter, and what publishes it is the FOUR-BYTE
 *     GAP between the frame's arrays: objs 0x50..0x8b, gfxs 0x8c..0x9f,
 *     buf 0xa0..0x11f, cur 0x120..0x12b -- 0x4c is left over, so an addressed
 *     scalar lives there.  A GAP IN THE AGGREGATE LAYOUT IS A DECLARATION.
 * (4) THE CLEANUP ORDER IS REVERSED: this function frees the five gfxs entries
 *     FIRST and Func_8003f3c(gfx) after; Func_8025200 does gfx first.
 * (5) THE ACCEPT PATH HAS NO SOUND AND NO MESSAGE.  It reads
 *     _GetMoveInfo(b[top + row]) and exits if info[1] & 0x80 is set; there is no
 *     _PlaySound(0x72) and no Func_801965c, unlike the item screen.
 * (6) v IS RE-SUBSCRIPTED AS b[top + n] EACH ITERATION HERE, where the item
 *     screen walks a u16 * with q++.  ldrh r5,[r3,r0] reg-plus-reg at both the
 *     loop entry and the bottom is the tell.
 *
 * ================ LEVERS THAT PAID, IN ORDER, WITH FIGURES ================
 *
 * (1) THE SIBLING'S VARIABLE PARTITION MUST BE PARTLY UNDONE.  Func_8025200 is
 *     two allocnos SHORT and needed every split; this function is two allocnos
 *     LONG and needs the opposite.  Merging the two gfx walkers back into one gp,
 *     the four loop counters back into one i, and the second divide loop's
 *     k2/m2/lim2 back onto k/m/lim: count 853 -> 849, size 1904 -> 1896,
 *     aligned 46.4 -> 50.5 percent.  Measured separately, merging the WALKERS
 *     alone gives 849 / 48.7 percent and merging the COUNTERS alone or the
 *     DIVIDE LOCALS alone gives 853 / 46.1 and 47.1 percent -- i.e. the walkers
 *     carry it and the rest only helps in company.  THE PARTITION IS A PER-
 *     FUNCTION MEASUREMENT, NOT A TRANSFERABLE RECIPE.
 * (2) NO EXPLICIT & 0x3ff ON THE slot STORE: objs[n].f.tile = slot, not
 *     slot & 0x3ff.  aligned 50.7 -> 52.3 percent, differing/ins/del 574 -> 547.
 *     NOTE THE OPPOSITE SIGN FROM THE SIBLING, where an explicit value mask was
 *     worse (805 instructions, 45.2 percent).  There the value comes from a CALL
 *     RETURN in r0; here it comes from a MEMORY READ of the out-parameter, and
 *     the ROM's ldr r3,=0x3ff / and r0,r3 before the ldrh is what the bitfield
 *     store's own mask produces on a memory operand.
 * (3) THE info[8] TERNARY INLINE IN THE ARGUMENT, not through a named local:
 *     Func_80218dc(boxB, 0x10, n * 2, info[8] == 0xff ? 0xb : info[8] - 1, 0).
 *     50.5 -> 50.7 percent.  The ROM's sub r3,#1 reuses the ldrb's own register
 *     on the else arm, which a named local breaks.
 *
 * ================ THE BLOCKER ================
 *
 * REGISTER ALLOCATION, and here it points the OTHER way from the sibling: our
 * frame carries TWO SPILL SLOTS TOO MANY.  Ours computes add r5,sp,#0xa8 for buf
 * where the ROM computes add r5,sp,#0xa0, and every scalar slot is 8 bytes high
 * (our top at sp+0x44 against the ROM's sp+0x3c), so the excess is two words of
 * spill space, not a mislaid array.  The ROM also keeps info in r8 -- a HIGH
 * register held across _GetMoveInfo, LoadMoveIcon, three SetTextColor calls,
 * Func_801e7c0, Func_801e9d4, two Func_8019000 and Func_80218dc -- while ours
 * keeps it in r6; that single role difference re-seats r6/r8/r9/r10/r11 through
 * the whole item loop and is most of the 547.
 * WHAT RULES OUT THE ALTERNATIVES:
 *   - NOT scheduling: sched1 does not run in this build, and a scheduler cannot
 *     change how many quantities live in memory.
 *   - NOT the shared block: it is byte-for-byte the sibling's and compiles to the
 *     ROM's field offsets and loop forms here too.
 *   - NOT the bitfield or container spelling: the sibling's u32-container probe
 *     is byte-identical and its manual-RMW probe moves the masks without moving
 *     the frame.
 *   - It IS the allocno COUNT, measured in four directions (lever 1 above): the
 *     merge that removed four allocnos moved the frame 8 bytes and the aligned
 *     figure 4 points, and no further merge or split found the last two.
 * REMAINING INSTRUCTION-LEVEL ITEMS, all downstream of the above: our
 * ldrb r3,[r1,#0xe] against the ROM's ldrh for boxB->y feeding the byte store at
 * OBJ+4 (four spellings tried on the sibling, all inert -- combine narrows the
 * HImode MEM along with the QImode arithmetic and there is no source handle);
 * and gcc's constant-pool placement, which differs by one dump position and
 * shifts every later pc-relative offset.
 *
 * ================ ONE ISA FACT, SO NOBODY SPENDS A PROBE ON IT ============
 *
 * mov r0,#0x3a / ldrsh r3,[r1,r0] for *(short *)(unit + 0x3a) is NOT an instance
 * of the Func_8018efc index-local lever.  THUMB ldrsh HAS NO IMMEDIATE-OFFSET
 * FORM AT ALL -- register-plus-register is the only encoding, so gcc must
 * materialise the 0x3a in a register whatever the source says.  Same for ldrsb.
 * The lever applies to ldrh/strh/ldr/str, where an immediate form exists and the
 * choice is real.
 */
#include "gba/types.h"

struct Win {
    unsigned char pad[8];
    u16 w;
    u16 h;
    u16 x;
    u16 y;
};

union Ent {
    int w[3];
    struct {
        int f0;
        u8 f4;
        u8 f5;
        u16 x : 9;
        u16 f6hi : 7;
        u16 tile : 10;
        u16 f8hi : 6;
    } f;
};

struct Ctx {
    unsigned char pad0[0x30];
    int f30;
    int f34;
    int f38;
    unsigned char pad3c[0x10];
    int f4c;
};

extern unsigned char *iwram_3001e8c;
extern struct Ctx *iwram_3001f34;
extern volatile unsigned int iwram_3001e40;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern unsigned char Data_310a4[];

extern int AllocUploadSpriteGFX(int size);
extern int UploadSprite2(int slot, void *gfx);
extern struct Win *CreateUIBox(int a, int b, int c, int d, int e);
extern int CloseUIBox(void *box, int n);
extern void WaitFrames(int n);
extern void Func_80251d4(int a, int b);
extern void Func_8022768(int x, int y, int w, int h, int c);
extern void Func_8016738(void);
extern void Func_801965c(int msg, u16 *out, u32 n);
extern void Func_8017aa4(u16 *buf, void *box, int c, int d);
extern void Func_8016498(void *box);
extern unsigned char *_GetUnit(int n);
extern unsigned char *_GetMoveInfo(int id);
extern void LoadMoveIcon(int id, int b, int *gfx, int *out, int e);
extern void SetTextColor(int c);
extern void Func_801e7c0(int id, void *box, int x, int y);
extern void Func_801e9d4(int a, int b, void *box, int d, int e);
extern void Func_80218dc(void *box, int b, int c, int d, int e);
extern void Func_8019000(void *box, int id, int a, int b, int e);
extern void Func_800352c(void);
extern void Func_8003dec(union Ent *e, int n);
extern void Func_8003f3c(int n);
extern void _PlaySound(int id);

int Func_802592c(int a, u16 *b, int c)
{
    union Ent cur;
    u16 buf[0x40];
    int gfxs[5];
    union Ent objs[5];
    int slot;
    unsigned char *s;
    int top;
    int prev_top;
    int row;
    int prev_row;
    int gfx;
    unsigned char *unit;
    struct Win *boxA;
    int nitems;
    int keep;
    int cy;
    int cx;
    struct Win *boxB;
    int ret;
    int i;
    int k;
    int n;
    int m;
    int g;
    int v;
    int lim;
    unsigned char *info;
    int *gp;
    struct Ctx *ctx;
    union Ent *e;

    s = iwram_3001e8c;
    prev_top = -1;
    prev_row = -1;
    gfx = AllocUploadSpriteGFX(0x80);
    unit = _GetUnit(a);
    boxA = CreateUIBox(0, 5, 0x1e, 4, 0x2a);
    nitems = 5;
    ctx = *(struct Ctx **)((char *)&iwram_3001e8c + 0xa8);
    top = ctx->f34;
    row = ctx->f30;
    keep = ctx->f38;
    boxB = CreateUIBox(9, 9, 0x15, 0xb, 6);
    e = objs;
    for (i = 0; i <= 4; i++) {
        e->w[1] = 0x80 << 23;
        e->w[2] = 0;
        e->f.x = (boxB->x << 3) + 8;
        e->f.f4 = ((i * 2 + boxB->y) << 3) + 4;
        e++;
    }
    gp = gfxs;
    for (i = 0; i <= 4; i++) {
        g = AllocUploadSpriteGFX(0x80);
        *gp++ = g;
        objs[i].f.tile = UploadSprite2(g, (void *)-1);
    }
    Func_80251d4(0xf018, 0x80 << 2);
    Func_80251d4(0xf018, 0x201);
    Func_80251d4(0xf019, 0x84 << 2);
    Func_80251d4(0xf019, 0x211);
    while (1) {
        if (top != prev_top || row != prev_row) {
            s[0xea6] = 1;
            Func_8022768(boxB->x + 1, boxB->y + prev_row * 2 + 1,
                         boxB->w - 2, 1, 0xf);
            Func_8016738();
            if (c != 0)
                Func_801965c((b[top + row] & 0x3fff) + 0x53a, buf, 0x34);
            else
                Func_801965c(0x8e7, buf, 0x34);
            Func_8017aa4(buf, boxA, 0, 4);
            prev_row = row;
            if (top != prev_top) {
                Func_8016498(boxB);
                v = b[top];
                n = 0;
                if (v != 0) {
                    do {
                        info = _GetMoveInfo(v);
                        Func_8019000(boxB, 0xf01f, 0xb, n * 2, 0);
                        Func_8019000(boxB, 0xf01e, 0xc, n * 2, 0);
                        LoadMoveIcon(v & 0x3fff, 0, &gfxs[n], &slot, 1);
                        objs[n].f.tile = slot;
                        if ((info[1] & 0x80) == 0)
                            SetTextColor(4);
                        else if (info[9] > *(short *)(unit + 0x3a))
                            SetTextColor(2);
                        else if (unit[0x13d] != 0)
                            SetTextColor(9);
                        s[0xea7] = 5;
                        Func_801e7c0(v + 0x333, boxB, 0x10, n << 4);
                        Func_801e9d4(info[9], 2, boxB, 0x68, n << 4);
                        SetTextColor(0xf);
                        s[0xea7] = 0xf;
                        if (info[2] != 4)
                            Func_8019000(boxB, info[2] + 0x5001, 0xf, n * 2, 0);
                        Func_80218dc(boxB, 0x10, n * 2,
                                     info[8] == 0xff ? 0xb : info[8] - 1, 0);
                        n++;
                        if (n > 4)
                            break;
                        v = b[top + n];
                    } while (v != 0);
                }
                nitems = n;
                prev_top = top;
            }
            if (c > 5) {
                lim = c + 4;
                for (k = 0; k < lim / 5; k++) {
                    m = k + 0xf301;
                    if (k == top / 5)
                        m = k + 0xf30b;
                    Func_8019000(boxB, m, boxB->w - lim / 5 + k - 2, -1, 0);
                }
            }
            Func_8022768(boxB->x + 1, boxB->y + row * 2 + 1, boxB->w - 2, 1, 0xe);
            s[0xea3] = 1;
            s[0xea6] = 0;
        }
        if (c > 5) {
            lim = c + 4;
            for (k = 0; k < lim / 5; k++) {
                m = k + 0xf301;
                if ((iwram_3001e40 & 0xf) <= 0xb && k == top / 5)
                    m = k + 0xf30b;
                Func_8019000(boxB, m, boxB->w - lim / 5 + k - 2, -1, 0);
            }
            Func_8019000(boxB, 0xf334, boxB->w - lim / 5 - 3, -1, 0);
            Func_8019000(boxB, 0xf335, boxB->w - 2, -1, 0);
            s[0xea3] |= 2 << ((boxB->y - 1) >> 2);
        }
        if (nitems > 0) {
            for (i = 0; i < nitems; i++)
                Func_8003dec(&objs[i], 0xf0);
        }
        cx = (boxB->x << 3) - 4;
        cy = ((row * 2 + boxB->y) << 3) + 0x14;
        cur.w[1] = 0x80 << 23;
        cur.w[2] = 0;
        cur.f.tile = UploadSprite2(gfx, Data_310a4);
        cur.f.x = cx + ((iwram_3001e40 & 4) >> 1) + 0xfffc;
        cur.f.f4 = cy - ((iwram_3001e40 & 4) >> 2) + 0xf8;
        if (c != 0)
            Func_8003dec(&cur, 0xf2);
        iwram_3001f34->f34 = top;
        iwram_3001f34->f30 = row;
        iwram_3001f34->f38 = keep;
        if ((gKeyPress & 1) != 0) {
            if (c == 0) {
                ret = -1;
                break;
            }
            ret = top + row;
            info = _GetMoveInfo(b[top + row]);
            if ((info[1] & 0x80) != 0)
                break;
        } else if (iwram_3001f34->f4c == 0 || (gKeyPress & 2) != 0) {
            _PlaySound(0x71);
            ret = -1;
            break;
        }
        if (c != 0) {
            if ((gKeyRepeat & 0x80) != 0) {
                _PlaySound(0x6f);
                row++;
                if (row == 5 || top + row == c)
                    row = 0;
                keep = row;
            } else if ((gKeyRepeat & 0x40) != 0) {
                _PlaySound(0x6f);
                row--;
                if (row < 0) {
                    if (top == (c - 1) / 5 * 5)
                        row = c - top - 1;
                    else
                        row = 4;
                }
                keep = row;
            } else if ((gKeyRepeat & 0x10) != 0) {
                _PlaySound(0x6f);
                Func_800352c();
                if (top + 5 < c) {
                    top += 5;
                    row = keep;
                    if (top == (c - 1) / 5 * 5) {
                        row = c - top - 1;
                        if (row > keep)
                            row = keep;
                    }
                } else if (top != 0) {
                    row = keep;
                    top = 0;
                }
            } else if ((gKeyRepeat & 0x20) != 0) {
                _PlaySound(0x6f);
                Func_800352c();
                if (top != 0) {
                    row = keep;
                    top -= 5;
                } else {
                    top = (c - 1) / 5 * 5;
                    row = keep;
                    if (top != 0) {
                        row = c - top - 1;
                        if (row > keep)
                            row = keep;
                    }
                }
            }
        }
        WaitFrames(1);
    }
    CloseUIBox(boxA, 1);
    CloseUIBox(boxB, 1);
    WaitFrames(1);
    gp = gfxs;
    for (i = 4; i >= 0; i--)
        Func_8003f3c(*gp++);
    Func_8003f3c(gfx);
    WaitFrames(1);
    return ret;
}
