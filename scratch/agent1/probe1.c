struct S { unsigned char p[0x1e]; short f; };
extern int g(int);
void f1(struct S *s, int i) { int v = i << 12; g(v); s->f = v + 0xffffc000; }
void f2(struct S *s, int i) { int v = i << 12; g(v); s->f = v - 0x4000; }
void f3(struct S *s, int i) { int v = i << 12; int t; g(v); t = v + 0xffffc000; s->f = t; }
void f4(struct S *s, unsigned int i) { int v = i << 12; g(v); s->f = (short)(v + 0xffffc000); }
void f5(struct S *s, int i) { int v = i << 12; g(v); *(volatile short *)&s->f = v + 0xffffc000; }
