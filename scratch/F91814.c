extern int _GetFlag(int member);
extern int _HasMove(int member, int ability);

int Func_8091814(int req)
{
    int member;
    int ability;

    member = (req >> 10) & 0xf;
    ability = req & 0x3ff;
    if (member > 7)
        return -1;
    if (_GetFlag(member) == 0)
        return -2;
    if (_HasMove(member, ability) == 0)
        return -3;
    return 0;
}
