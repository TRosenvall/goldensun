extern void _Func_8019000(int win, int tile, int col, int row, int z);

void Func_80a8cc0(int win, int col, int row, int idx)
{
	int t1;
	int t0;

	t1 = 0xf281 + idx * 2;
	_Func_8019000(win, t1 | 0x400, col, row, 0);
	t0 = 0xf280 + idx * 2;
	_Func_8019000(win, t0, col + 1, row, 0);
	_Func_8019000(win, t1, col + 2, row, 0);
}
