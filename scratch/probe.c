extern void  vreg(int a, int b, int c);
extern int   ireg(int a, int b, int c);
extern void  vcst(int a, int b, int c);
extern int   icst(int a, int b, int c);
extern int   src(void);

void probe_reg_void(void) { int r = src(); vreg(r, 0, 0); }
void probe_reg_int(void)  { int r = src(); ireg(r, 0, 0); }
void probe_cst_void(void) { vcst(0xc, 0, 0xa); }
void probe_cst_int(void)  { icst(0xc, 0, 0xa); }
