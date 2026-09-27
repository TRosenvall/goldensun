/* Func_8022a7c -- asm/rom_15000/rom_22a7c.s   (PARK, fresh target)
 *
 * AttachListNode (scroll arrow): take a display node with Func_8015e8c, give it
 * an OBJ tile slot (AllocUploadSpriteGFX(0x80)), write OAM attr words
 * 0x40000400 / 0, set the 9-bit x and 8-bit y from the parent's window
 * position (win->x*8 + x, win->y*8 + y), upload the up- or down-arrow graphic
 * (.L313a4 / .L31424) into the 10-bit tile field and link the node to the
 * window with Func_8016584.
 *
 * THIS IS A GCC NESTED FUNCTION.  It reads r9 (the Thumb STATIC_CHAIN_REGNUM)
 * with no defining write, saves it to a lone frame slot, and loads the window
 * pointer through [r9 - 4].  Its only caller is Func_8022b44 (RunItemScreen,
 * same .s, ~715 lines), which does "add r1, sp, #0x4c / mov r9, r1 / bl" before
 * each of its six calls, and whose first parameter (the window) is spilled to
 * sp+0x48 = chain-4.  See docs/elevation.md "The static-chain class: PROVE it by
 * compiling a nested function" -- DO NOT ship a non-nested transcription.
 * Landing it is a WHOLE-FILE job: elevate Func_8022b44 with this nested inside.
 *
 * The candidate below uses a STAND-IN parent (only the nested body is being
 * measured) and names the nested function with an asm label so the screening
 * tools can find it; drop the label once it lives in the real parent.
 *
 * 21 of 83 lines differ by tryc; objcmp cannot give a clean encoding count here
 * because the stand-in parent is emitted directly after the nested function and
 * is counted into its extent.
 *
 * Verify with:
 *   python3 tools/tryc.py src/non_matching/rom_15000/8022a7c.c --ref asm/rom_15000/rom_22a7c.s --full
 *   (read the Func_8022a7c block; ignore Func_8022b44)
 *
 * REMAINING DIFFERENCES (two):
 *   1. ROM computes s = n + 0x10 between "mov r3,#0x78" and the two strh; ours
 *      one slot later (sched2 order only).
 *   2. ROM keeps the chain-4 address in r5 across UploadSprite2 and RELOADS
 *      the window for Func_8016584 ("ldr r0,[r5]"); ours passes the named w.
 *      Passing the reloaded parent variable instead (as the ROM does) makes the
 *      chain pseudo tie with the chain-4 address: the chain is copied to r5 in
 *      the prologue, x takes r9, and a zero pseudo is hoisted into r8 -- 75 lines.
 *
 * WHAT WORKED (each measured, reusable):
 *   * The parent variable read through the chain must CONFLICT with the two
 *     OAM word stores (else sched2 hoists the load above them) but must NOT be
 *     invalidated by the x bitfield store.  A u32 parent variable + u32 word
 *     stores gives the first; a named local w for the x/y reads gives the second.
 *   * "int t = (w->y << 3) + y; s->oam.y = t;" -- routing the value through an
 *     int keeps the ROM's "ldrh [r0,#0xe]"; assigning directly to the u8 field
 *     lets fold narrow the load to ldrb.
 *   * Assigning s = &n->sub AFTER n->fF (not after the 0x78 stores) gives the
 *     ROM's n=r6 / s=r7 instead of the swap (37 -> 21 lines).
 *   * Observed (mechanism not traced): with struct Sub { u32 f0; struct Oam oam; }
 *     the x bitfield store invalidates a u32 parent load and the window is
 *     reloaded for y; with { void *f0; struct Oam oam; } it is not.  So the
 *     type of Sub's OTHER member decides whether s->oam.x aliases -- worth
 *     reading get_alias_set / record_component_aliases before relying on it.
 *
 * INERT / WORSE: statement order of fF/x/y/slot; "n->x = n->y = 0x78"; void vs
 * int Func_8016584; u32 vs u16 bitfield containers; manual RMW masks instead of
 * bitfields (mask becomes "mov #0xfe / lsl #8"); w declared at function scope.
 */
struct Win {
    unsigned char pad[0xc];
    unsigned short x;
    unsigned short y;
};

struct Oam {
    unsigned char y;
    unsigned char a0;
    unsigned short x:9;
    unsigned short a1:7;
    unsigned short tile:10;
    unsigned short a2:6;
};

struct Sub {
    unsigned int f0;
    struct Oam oam;
};

struct Node {
    unsigned int next;
    unsigned char f4;
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad[4];
    unsigned char slot;
    unsigned char fF;
    struct Sub sub;
};

extern struct Node *Func_8015e8c(void);
extern int Func_8016584(struct Win *, struct Node *);
extern int AllocUploadSpriteGFX(int size);
extern int UploadSprite2(int slot, void *gfx);
extern unsigned char gArrowUp[] __asm__(".L313a4");
extern unsigned char gArrowDown[] __asm__(".L31424");
extern int Func_stub(struct Win **);

void Func_8022b44(unsigned int win, int a, int b)
{
    auto void Func_8022a7c(int x, int y, int up) __asm__("Func_8022a7c");
    void Func_8022a7c(int x, int y, int up)
    {
        struct Node *n;
        struct Sub *s;
        struct Win *w;

        n = Func_8015e8c();
        if (n != 0) {
            n->f5 = 1;
            n->f4 = 1;
            n->slot = AllocUploadSpriteGFX(0x80);
            n->fF = 0xf0;
            s = &n->sub;
            n->x = 0x78;
            n->y = 0x78;
            *(unsigned int *)&s->oam = 0x40000400;
            *((unsigned int *)&s->oam + 1) = 0;
            w = (struct Win *)win;
            s->oam.x = (w->x << 3) + x;
            { int t = (w->y << 3) + y; s->oam.y = t; }
            s->oam.tile = UploadSprite2(n->slot, up ? gArrowUp : gArrowDown);
            Func_8016584(w, n);
        }
    }

    if (a != b)
        Func_8022a7c(0x50, 0xe, a > b);
}
