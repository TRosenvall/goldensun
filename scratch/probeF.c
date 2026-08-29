extern void f(int a, int b, int c);
extern int cond(void);
/* two uses in MUTUALLY EXCLUSIVE arms -- neither dominates the other */
void p9(void) { if (cond()) f(0, 0x9999, 1); else f(1, 0x9999, 2); }
/* two uses, second in a guarded block, first unguarded (first dominates) */
void p10(void) { f(0, 0x9999, 1); if (cond()) f(1, 0x9999, 2); }
