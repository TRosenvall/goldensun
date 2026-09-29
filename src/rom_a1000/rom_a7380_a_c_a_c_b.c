/* Func_80a77a4 (0x080a77a4) -- 76 encodings, 172 bytes, exact.
 *
 * `cur` exists to be reused, and that is the whole match.  Passing
 * st->cursor[which] straight to Func_80a17c4 is 8 differing; naming it makes it
 * die in TWO places, so local_alloc refuses it (local-alloc.c:362 wants
 * REG_N_DEATHS == 1) and it becomes a global allocno with the highest
 * allocno_compare priority in the function -- allocated first.  The copy into r0
 * for the argument then gives it `preferences: 0`, and find_reg's preference
 * pass (global.c:1103) overrides the REG_ALLOC_ORDER pick, so it lands in r0
 * rather than r2 and the entry block's sched2 order follows for free.
 */
extern void _Func_8016498(unsigned int win);
extern int _GetFlag(int id);
extern void _Func_801e41c(unsigned int win, int a, int b, int c, int e);
extern void Func_80a1ac0(int x, int y);
extern int Func_80a7d68(void);
extern int Func_80a7a34(void);
extern void WaitFrames(int n);

struct Cursor {
    unsigned char pad0[5];
    unsigned char visible;
    unsigned char pad6[6];
    unsigned short timer;
};
extern void Func_80a17c4(struct Cursor *p);

struct StatusState {
    unsigned char pad0[0x10];
    unsigned int win;
    struct Cursor *cursor[2];
    signed char sel[4];
    unsigned char pad20[0x200];
    unsigned short mode;
};
extern struct StatusState *iwram_3001f2c;
int Func_80a77a4(int which)
{
    struct StatusState *st = iwram_3001f2c;
    struct Cursor *cur;
    int sel;
    int r;

    sel = st->sel[which];
    cur = st->cursor[which];
    r = 0;
    cur->visible = 1;
    cur->timer = r;

    _Func_8016498(st->win);
    if (_GetFlag(0x172))
        _Func_801e41c(st->win, 9, 1, 9, 3);
    if (sel == -1)
        st->sel[which] = 0;
    else
        Func_80a1ac0(sel * 24 - 10, 0x10);
    if (st->mode == 3)
        r = Func_80a7d68();
    else
        r = Func_80a7a34();
    cur = st->cursor[which];
    Func_80a17c4(cur);
    WaitFrames(1);
    return r;
}
