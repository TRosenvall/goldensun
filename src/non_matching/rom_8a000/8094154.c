/* Func_8094154 -- 0x08094154, asm/rom_8a000/rom_93304_a_c_c_c_c.s
 *
 * Converts a field actor's world position to screen coordinates: subtract the
 * camera origin at iwram_3001e70 + 0xe4 with its low 16 bits masked off, divide
 * both axes by 0x10000, and write the pair through the caller's pointer. Actors
 * whose kind nibble at +0x54 is 1 get their y nudged by the sprite's signed
 * byte at +8. Returns -1 if the actor does not exist, 0 otherwise.
 *
 * 18 of 64, with the instruction COUNT exact and everything from the divisions
 * onward matching. The whole residue is one block: the order in which four
 * loads and two masks are issued.
 *
 *     rom   ldr cam0 / ldr cam1 / ldr a->f10 / and cy / and cx / ldr a->f8 ...
 *     ours  ldr cam0 / ldr cam1 / and cx / and cy / ldr a->f10 / ldr a->f8 ...
 *
 * The ROM reads a->f10 BEFORE applying either mask; we apply both first. Same
 * operations, same count, different sequence, and the register assignment
 * rotates behind it.
 *
 * BLOCKER: the obvious lever makes it worse. NAMING THE LOADED VALUES -- the
 * eager-issue rule that has worked repeatedly elsewhere -- costs a
 * callee-saved register here and takes 19 back to 46-50, because the extra
 * pseudos push the prologue from `push {r5, r6, lr}` to a wider save. So the
 * ordering cannot be bought with locals, and without locals there is nothing
 * left to reorder in the source: both statements are single expressions whose
 * sub-expression order gcc chooses for itself.
 *
 * TRIED:
 *   a  out[0]/out[1] indexing, expressions inline                52 differ
 *   b  every load named, masks as compound assignments           54
 *   c  `*out++ = ...; *out = ...`                                48
 *   d  explicit `q = out; out = out + 1;` two-pointer form       19  <- the step
 *   e  d + cam/f10 reads named                                   50
 *   f  d + named reads and compound masks                        46
 *   g  d with the dy statement written FIRST                     18  <- best
 *   h  d with dy's parenthesisation flattened                    19
 *   i  dy split across two statements                            31
 *
 * WHAT WAS WON, and it is the transferable part: THE ROM ADVANCES A POINTER,
 * IT DOES NOT INDEX. `mov r2, r5 / add r5, #4` is two pointer variables --
 * `q = out; out = out + 1;` -- with the first store through q and the second
 * through the advanced out, which is also what makes the post-call re-read
 * `ldr r3, [r5]` fall out naturally. Writing `out[0]`/`out[1]` gives 52 and
 * `*out++` gives 48; the explicit pair gives 19. That single change is worth
 * more than everything else tried put together, and it is worth checking
 * whenever a ROM copies a pointer argument and adds to the original.
 *
 * Confirmed in passing: the bias-add of 0xffff under `bge` before `asr #16` is
 * the recorded division tell -- the source wrote `/ 0x10000`, and a `>> 16`
 * omits the bias entirely.
 */

struct Sub {
    unsigned char pad00[0x28];
    short *f28;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Sub *f50;
    unsigned char pad54[0];
    unsigned char f54;
};

struct Info {
    unsigned char pad00[8];
    signed char f8;
};

extern unsigned char *iwram_3001e70;
extern struct Actor *GetFieldActor(int id);
extern struct Info *_GetSpriteInfo(int id);

int Func_8094154(int id, int *out)
{
    struct Actor *a;
    int *cam;
    int dx, dy;
    int *q;

    a = GetFieldActor(id);
    if (a == 0)
        return -1;
    cam = (int *)(iwram_3001e70 + 0xe4);
    dy = (a->f10 - (cam[1] & 0xffff0000)) - a->fc;
    dx = a->f8 - (cam[0] & 0xffff0000);
    q = out;
    out = out + 1;
    *q = dx / 0x10000;
    *out = dy / 0x10000;
    if ((a->f54 & 0xf) == 1)
        *out -= _GetSpriteInfo(*a->f50->f28)->f8;
    return 0;
}