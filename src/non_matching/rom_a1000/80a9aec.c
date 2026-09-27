/* Func_80a9aec -- NON-MATCHING, 63 encodings of 76, AND FOUR INSTRUCTIONS SHORT:
 * ref 76 encodings / ours 72, ref 168 bytes / ours 160.  DO NOT READ THE 63 AS A DISTANCE
 * -- the stream is four instructions short, so most of it measures the shift.
 *
 * (I first wrote this header as "76 encodings of 76", turning the agent's "ref 76 / ours
 * 72" into a claim the body could not produce.  parkcheck.py caught it BEFORE the commit.
 * Third instance of a header stating a number its body does not measure, and the first
 * caught by the tool rather than by the next reader.)
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a9aec.c \
 *     asm/rom_a1000/rom_a8604_c_c_a_a_a.s --func Func_80a9aec
 *
 * NO .sym EDIT NEEDED, and two existing entries are CONFIRMED rather than assumed:
 * message.sym:221 _MSG_182 and const.sym:77 _CONST_200 both already exist, both are
 * REQUIRED here, and both are measured.  The literal 0x182 is shiftable so it
 * rematerialises in all four case bodies and the function then saves only one high
 * register.  `_CONST_1ff` was tried and is BYTE-IDENTICAL to the literal -- do not add one.
 * _CONST_200 GAINS A SECOND ATTESTING SITE here, in the same bank and the same inventory
 * word at bit 9, where const.sym had recorded it on pool evidence alone.
 *
 * BLOCKER: global_alloc allocno_compare, and it is quantified to a single inequality.
 * The entire residue is r7 <-> r8 between `list` and `msg`, and the priority formula gives
 * list = 2692 against msg = 2700.  Across EVERY variant measured, live(msg) = 2*live(list)
 * - 4, so the condition reduces to **live(list) >= 56, i.e. FOUR MORE IN-LOOP
 * INSTRUCTIONS**.
 *
 * It is reachable and useless, which is the finding: `unsigned short v` costs exactly 4
 * and produces an exact tie that flips the allocation -- but those 4 are halfword masking
 * THE ROM DOES NOT HAVE (89 lines against 80).  So the open question is whether the
 * missing 4 instructions are real or a fixed point.  NOTE THE CIRCULARITY TRAP: the four
 * `mov r3, r8` are reload copies that exist BECAUSE msg is in r8, so reading them as the
 * cause is circular.
 */
/* Func_80a9aec -- 0x080a9aec, asm/rom_a1000/rom_a8604_c_c_a_a_a.s (2 functions,
 * datacheck.py clean).  PARKED at 76 lines against 80, ONE register exchange.
 *
 *   objcmp: XX SIZE ref 168 bytes, ours 160
 *           XX ENCODINGS differ in 63 place(s) (ref 76, ours 72)
 *           first at index 4: ref 4b0e ours 230e
 *           relocations: the 5 R_ARM_THM_CALL are at the ROM's own offsets up to
 *           _GetItemInfo (0x28 both sides); ours adds R_ARM_ABS32 _MSG_182 and
 *           _CONST_200 for the two pool words -- the aliased-symbol situation
 *           src/rom_a1000/rom_a1050_c_c_c_c_a.c records, where `make compare` is
 *           the only authority.
 *   tryc:   rom 80 lines, ours 76, first diff at 4, 59 differ
 *
 * NO .sym EDIT IS NEEDED. Both symbols already exist: message.sym:221
 * `_MSG_182 = 0x0182;` and const.sym:77 `_CONST_200 = 0x200;`. Both are required
 * and both are measured:
 *
 *   - 0x182 as a LITERAL is rematerialised as `mov r3,#0xc1 / lsl r3,#1` in each
 *     of the four case bodies (0x182 is a shiftable byte), and the function then
 *     saves only ONE high register where the ROM saves two. As the symbol it
 *     becomes a pool load that gcse hoists into the loop preheader and keeps in a
 *     callee-saved high register -- the ROM's r8 -- and the prologue/epilogue
 *     match exactly. This is the pool tell in the shape const.sym exists for,
 *     with the shiftable-byte wrinkle: the tell normally shows up as a pooled
 *     value a `mov` could build; here it shows up as a value that does not stay
 *     in a register.
 *   - 0x200 as a literal is `mov r3,#0x80 / lsl r3,#0x2`; `(int)&_CONST_200`
 *     gives the ROM's single `ldr r3, =0x200`.  const.sym's own entry already
 *     records eight literal spellings that fail, and this is a second attesting
 *     site for _CONST_200 -- same bank, same inventory word, bit 9.
 *   - 0x1ff needs NOTHING: it is not shiftable, so it pools as a literal. A
 *     `_CONST_1ff` spelling was tried and is byte-identical to the literal, so
 *     do not add one.
 *
 * THE WHOLE RESIDUE IS r7 <-> r8 BETWEEN `list` AND `msg`, AND IT IS A
 * global_alloc PRIORITY TIE MEASURED TO EIGHT PARTS IN TEN THOUSAND.
 * The ROM puts the list pointer in r7 and the message base in r8; gcc does the
 * reverse, which costs `mov r3,r8 / ldrh r2,[r3]` and `mov r3,#2 / add r8,r3` in
 * the loop head and saves the four `mov r3, r8` in the case bodies.
 *
 * global.c's allocno_compare ranks by
 *
 *     pri = floor_log2(n_refs) * n_refs / live_length * 10000
 *
 * and the -da .17.lreg dump prints both terms. Confirmed by predicting the whole
 * printed order ";; 7 regs to allocate: 46 37 36 32 34 33 35" exactly. For this
 * function:
 *
 *     33 = list  n_refs 7   live 52    pri 2*7/52*10000  = 2692
 *     34 = msg   n_refs 9   live 100   pri 3*9/100*10000 = 2700
 *
 * msg wins by 8 and takes r7. On a tie the lower allocno wins and `list`, being
 * a parameter, has the lower number -- so ANY perturbation that closes 8/10000
 * flips it. The two terms move together: across every variant measured,
 * live(msg) = 2*live(list) - 4, so the condition reduces to live(list) >= 56,
 * i.e. FOUR MORE INSTRUCTIONS LIVE IN THE LOOP. It is reachable: `unsigned short
 * v` instead of `unsigned int v` costs exactly 4 in-loop instructions, lands at
 * 56/108 -- an exact tie -- and the allocation flips to the ROM's. It is also
 * useless, because those four instructions are halfword masking the ROM does not
 * have (89 lines against 80).
 *
 * MEASURED AND INERT (all 76 lines, all the same exchange): 30 spellings over
 * list as unsigned int / unsigned short * / *p++ / p[0] / reversed increment,
 * msg as a local declared first, a local declared last, a local assigned inside
 * the loop, and written inline at all four call sites; then 72 more over v as
 * int/unsigned int, id as int/unsigned int, `default:` present and absent, for /
 * while / do-while, and `(v & m) != 0` against `v & m`. Only the `unsigned short
 * v` axis moves the allocation at all.
 *
 * WHAT WOULD SETTLE IT: four in-loop instructions the ROM has and this C does
 * not. The candidate is the ROM's `ldr r3,=0x1ff / mov r5,r3 / and r5,r0` (three
 * where we emit two) plus the four `mov r3, r8` in the case bodies -- but those
 * four are reload copies that exist only BECAUSE msg is in r8, so reading them
 * as the cause is circular. Either one more pre-reload instruction is missing
 * from the source, or this is a genuine fixed point and the park stands.
 *
 * The signature came from src/non_matching/rom_a1000/80a9a5c.c, which prototypes
 * this as `void Func_80a9aec(unsigned int win, unsigned int list)`; verified --
 * the pointer spelling is byte-identical, so `unsigned int` is kept to match the
 * caller's park.
 */
extern int _MSG_182;
extern int _CONST_200;
extern unsigned char *_GetItemInfo(int id);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);

void Func_80a9aec(unsigned int win, unsigned int list)
{
    int msg;
    int i;
    unsigned int v;
    unsigned int id;
    unsigned char *info;

    msg = (int)&_MSG_182;
    for (i = 14; i >= 0; i--) {
        v = *(unsigned short *)list;
        list += 2;
        if ((v & (int)&_CONST_200) != 0) {
            id = v & 0x1ff;
            info = _GetItemInfo(id);
            switch (info[2]) {
            case 1:
                _Func_801e7c0(msg + id, win, 8, 8);
                break;
            case 2:
                _Func_801e7c0(msg + id, win, 8, 0x38);
                break;
            case 3:
                _Func_801e7c0(msg + id, win, 8, 0x28);
                break;
            case 4:
                _Func_801e7c0(msg + id, win, 8, 0x18);
                break;
            }
        }
    }
}
