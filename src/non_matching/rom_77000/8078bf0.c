/* Func_8078bf0 (0x08078bf0) -- NON-MATCHING, 246 of 272 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_77000/8078bf0.c asm/rom_77000/rom_78b9c_a_c_c_a.s \
 *       --func Func_8078bf0
 *
 * SIZE 560 vs ref 568.  INSTRUCTIONS 271 vs ref 272.  NEITHER matches, so the
 * objcmp count is SATURATED and is NOT a distance -- rank with tools/aligncmp.py,
 * which reads aligned-equal 81 of 272 (29.8%), 285 differing in 40 hunks.
 * THIS IS THE WEAKEST OF THE FIVE FUNCTIONS IN THIS BATCH and the honest summary
 * is that only the shape is settled, not the allocation.
 * SHIMS: 1 register pin (`arr` in r8).  Needs a fakematch.txt row.  The pin is
 * worth 3 aligned-equal points and 11 instructions and is discussed below; a
 * pin-free draft is described so it can be recovered.
 *
 * SPLIT: none needed.  asm/rom_77000/rom_78b9c_a_c_c_a.s holds ONE function;
 * tools/datacheck.py reports NO required data exports.
 *
 * ================== THE PROSE COMMENT, CHECKED ================================
 * "RecomputeEquipmentEffects / r0 = combatant id.  Walks the inventory and the
 * 32-entry array at +0x58, applying every equipped item's modifiers to the derived
 * stats through Func_78414 and Func_79ad8.  320 lines"
 *   - "r0 = combatant id" and "the 32-entry array at +0x58" are RIGHT and are the
 *     two facts worth having.
 *   - Func_78414 and Func_79ad8 ARE NOT CALLED.  The only callees are GetUnit,
 *     GetClassInfo and GetItemInfo.  Nothing here applies a modifier to a stat.
 *   - "320 lines" is a .s LINE count; the function is 272 instructions.  The
 *     batch brief's "262" is a line count too.
 * WHAT IT ACTUALLY DOES: it REBUILDS the 32-slot ability list at unit+0x58 (stride
 * 4, the id in the low halfword, bits 14 and 15 as provenance tags) from two
 * sources, in six passes:
 *   1. clear every slot tagged 0x8000 (class-granted)
 *   2. clear every slot tagged 0x4000 (equipment-granted)
 *   3. compact DOWNWARD -- pack the survivors against the TOP of the array,
 *      walking index 31 -> 0 and writing at a pointer that starts at arr+0x7c and
 *      decreases, then zero what is left below
 *   4. for each of the 16 class entries at classinfo+0x10 (stride 4: id, minlevel)
 *      whose minlevel <= unit[0x0f], add `id | 0x8000` to the first empty slot if
 *      the id is not already present -- and BREAK OUT of the outer loop entirely
 *      when the array is full
 *   5. for each of the 15 inventory halfwords at unit+0xd8, if bit 9 is set and
 *      GetItemInfo(item)[0xc] == 3, add `info[0x28] | 0x4000` the same way,
 *      matching on the low 14 bits only
 *   6. compact UPWARD and zero the tail
 * It returns 0 on every path, including the early `unit[0x129] == 0` refusal.
 *
 * ================== WHAT IS SETTLED ==========================================
 *  - `lvl = unit + 0x129` as a POINTER, built constant-first (`ldr r5,=0x129 /
 *    add r5,r9`), and read TWICE -- once as GetClassInfo's argument and once for
 *    the refusal test.  Two reads across the call is what forces the reload.
 *  - all four compaction/clear loops are do-while with the test AFTER the
 *    decrement, e.g. `do { k--; *q = 0; q -= 2; } while (k >= 0);` which is the
 *    ROM's `sub r5,#1 / strh / sub r3,#4 / cmp r5,#0 / bge` exactly.
 *  - passes 3 and 6 duplicate the counter step into BOTH arms of the if
 *    (`if (v == 0) { i--; } else { ...; i--; ... }`).  The ROM has two `sub r4,#1`.
 *  - `arr` as `unsigned short *` indexed `arr[i * 2]`, which gives the ROM's
 *    `lsl r3,r4,#2 / ldrh r2,[r3,r0]` register-offset form.
 *  - pass 4 reaches the SAME class byte through THREE different expressions --
 *    `e[0]` (a walking pointer), `ent[0]`/`ent[1]` (a second walking pointer) and
 *    `cls[b]` with `b` a byte offset stepped by 4.  That is not a transcription
 *    slip: the ROM keeps r7, lr and sl on the same table and RELOADS `cls` from
 *    its stack slot inside the inner search loop every iteration.  Collapsing them
 *    to one pointer loses the shape.
 *  - `0x200` IS POOLED in pass 5 (`ldr r3, =0x200`) although 0x80 << 2 is
 *    buildable.  Left as a literal here; it is a candidate for the const.sym tell
 *    but the in-function control test is NOT met (nothing else in this function
 *    builds 0x200), so it is not proposed.
 *  - three of the four zero stores are POOLED in the ROM (`ldr r1, =0` twice for
 *    passes 1 and 2, `ldr r2, =0` for pass 3, `ldr r1, =0` for pass 6).  A bare
 *    `0` through an `unsigned short *` is the pooling case (800c880.c lever 4).
 *
 * ================== WHAT WAS TRIED AND MADE IT WORSE =========================
 * Baseline pin-free draft: 260 instructions / 536 bytes, aligned-equal 26.8%.
 *   ONE SHARED `int zero = 0;` for all four zero stores            255/520, 25.4%
 *        -- WORSE, and instructive: it lets gcc CSE one zero into a register for
 *        the whole function, where the ROM reloads the SAME pool word four times.
 *        The c880 lever ("a bare 0 through a short * pools") applies to a SINGLE
 *        store; sharing the carrier across four undoes it.
 *   flipping passes 3 and 6 to `if (v == 0)` first                 no change
 *   a named `m = 0x3fff` for pass 5's mask                         258/532, 24.3%
 *   pinning `unit` to r9 as well as `arr` to r8                    269/556, 29.8%
 *        -- r9 does land, and nothing else moves; the second pin buys nothing.
 * Best pin-free draft is the 260/536 one; its only difference from this file is
 * `unsigned short *arr;` in place of the register declaration and plain 0x8000 /
 * 0x4000 literals in passes 1 and 2.
 *
 * ================== THE RESIDUE ==============================================
 * Same wall as the rest of this batch, and here it is the most expensive:
 *  - `arr` belongs in r8 and gcc puts it in r7.  PINNED here, because it is worth
 *    11 instructions: with arr in a low register every access is a bare
 *    `ldrh r2,[r1]`, where the ROM pays `mov r0, r8` first.  This is the clearest
 *    single demonstration in the batch that the ROM's build is under more register
 *    pressure than gcc-2.96 gives us -- the ROM is paying for a high register and
 *    we are not.
 *  - the ROM copies the loaded halfword before testing it (`ldrh r2,[r3,r0] /
 *    mov r3,r2 / cmp r3,#0`) at three sites; we test the loaded register directly.
 *  - the frame: ROM `sub sp, #8` with only [sp,#4] used (`cls`); ours `sub sp, #4`.
 *    4 bytes of phantom slack and 4 of the 8 missing bytes.  There is no local
 *    aggregate to hang a pad on here -- an unused `int` is deleted -- so this one
 *    needs the second spill to be generated, not reserved.
 *  - pass 4's constants sit in lr and sl in the ROM (`add lr,r2 / add sl,r2` to
 *    step them) and in ip and r5 for us.
 *
 * NEXT: this function should be re-screened after the REG_ALLOC_ORDER experiment
 * HANDOFF batch 295 proposes -- it is the largest and cleanest instance of that
 * class found so far, and if rebuilding gcc-2.96 with the order starting at 4 does
 * not move it, that hypothesis needs a different explanation.  Do not spend more
 * source-level effort here first; five distinct spellings have now been measured
 * and the best pin-free result has not moved past 27%.
 */
extern unsigned char *GetUnit(int id);
extern unsigned char *GetClassInfo(int cls);
extern unsigned char *GetItemInfo(int item);

int Func_8078bf0(int id)
{
    unsigned char *unit;
    unsigned char *cls;
    unsigned char *info;
    unsigned char *lvl;
    unsigned char *ent;
    unsigned char *e;
    register unsigned short *arr __asm__("r8");
    unsigned short *p;
    unsigned short *q;
    int i;
    int j;
    int k;
    int n;
    int b;
    int v;
    int m;
    int want;

    unit = GetUnit(id);
    lvl = unit + 0x129;
    arr = (unsigned short *)(unit + 0x58);
    cls = GetClassInfo(*lvl);
    if (*lvl == 0)
        return 0;

    m = 0x8000;
    p = arr;
    n = 0x1f;
    do {
        if ((*p & m) != 0)
            *p = 0;
        n--;
        p += 2;
    } while (n >= 0);

    m = 0x4000;
    p = arr;
    n = 0x1f;
    do {
        if ((*p & m) != 0)
            *p = 0;
        n--;
        p += 2;
    } while (n >= 0);

    q = &arr[31 * 2];
    i = 0x1f;
    k = 0x1f;
    do {
        v = arr[i * 2];
        if (v != 0) {
            *q = v;
            i--;
            q -= 2;
            k--;
        } else {
            i--;
        }
    } while (i >= 0);
    if (k >= 0) {
        q = &arr[k * 2];
        do {
            k--;
            *q = 0;
            q -= 2;
        } while (k >= 0);
    }

    ent = cls + 0x10;
    m = 0x8000;
    b = 0x10;
    e = ent;
    for (i = 0; i <= 0xf; i++) {
        if (e[0] != 0 && unit[0xf] >= ent[1]) {
            v = arr[0];
            j = 0;
            if (v != ent[0]) {
                p = arr;
                for (;;) {
                    j++;
                    if (j > 0x1f)
                        break;
                    p += 2;
                    if (*p == cls[b])
                        break;
                }
            }
            if (j == 0x20) {
                j = 0;
                if (v == 0) {
                    arr[0] = e[0] | m;
                } else {
                    for (;;) {
                        j++;
                        if (j > 0x1f)
                            break;
                        p = &arr[j * 2];
                        if (*p == 0) {
                            *p = e[0] | m;
                            break;
                        }
                    }
                }
                if (j == 0x20)
                    break;
            }
        }
        ent += 4;
        e += 4;
        b += 4;
    }

    b = 0xd8;
    for (i = 0; i <= 0xe; i++) {
        v = *(unsigned short *)(unit + b);
        if (v != 0 && (v & 0x200) != 0) {
            info = GetItemInfo(*(unsigned short *)(unit + b));
            if (info[0xc] == 3) {
                v = arr[0];
                want = *(unsigned short *)(info + 0x28);
                j = 0;
                if ((v & 0x3fff) != want) {
                    p = arr;
                    for (;;) {
                        j++;
                        if (j > 0x1f)
                            break;
                        p += 2;
                        if ((*p & 0x3fff) == want)
                            break;
                    }
                }
                if (j == 0x20) {
                    j = 0;
                    if (v == 0) {
                        arr[0] = want | 0x4000;
                    } else {
                        for (;;) {
                            j++;
                            if (j > 0x1f)
                                break;
                            p = &arr[j * 2];
                            if (*p == 0) {
                                *p = want | 0x4000;
                                break;
                            }
                        }
                    }
                    if (j == 0x20)
                        break;
                }
            }
        }
        b += 2;
    }

    i = 0;
    k = 0;
    q = arr;
    do {
        v = arr[i * 2];
        if (v != 0) {
            *q = v;
            i++;
            q += 2;
            k++;
        } else {
            i++;
        }
    } while (i <= 0x1f);
    if (k <= 0x1f) {
        q = &arr[k * 2];
        n = 0x20 - k;
        do {
            n--;
            *q = 0;
            q += 2;
        } while (n != 0);
    }
    return 0;
}
