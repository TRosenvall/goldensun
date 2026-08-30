extern void g(void);
int f1(signed char *p){ signed char c; c = *p; if (c != 0) g(); return 0; }
int f2(char *p){ if (*p != 0) g(); return 0; }
int f3(unsigned char *p){ int c; c = *p << 24; if (c != 0) g(); return 0; }
struct S { signed char f : 8; };
int f4(struct S *p){ if (p->f != 0) g(); return 0; }
int f5(signed char *p){ int c; c = *p; c <<= 24; if (c != 0) g(); return 0; }
int f6(unsigned char *p){ unsigned char c; c = *p; if (c != 0) g(); return 0; }
