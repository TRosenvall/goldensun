extern void f1(int);
int t1(int a) { int v[3]; f1(a); return 0; }
int t2(int a) { volatile int v[3]; f1(a); return 0; }
int t3(int a) { int v[3]; f1(a); return v[0]; }
int t4(int a) { int v[3]; if (a == 0x7fffffff) f1((int)v); f1(a); return 0; }
struct S3 { int a, b, c; };
extern struct S3 g3(int);
int t5(int a) { struct S3 s; s = g3(a); f1(a); return 0; }
extern void f7(int,int,int,int,int,int,int);
int t6(int a) { f7(a,a,a,a,a,a,a); return 0; }
