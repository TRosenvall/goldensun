extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetAnimSpeed(int slot, int n);

void OvlFunc_945_200c7cc(int slot)
{
    switch (slot) {
    case 0x13:
        __MapActor_SetAnim(slot, 6);
        __MapActor_SetAnimSpeed(slot, 8);
        break;
    case 0x12:
    case 0x14:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 0x10);
        break;
    case 0x16:
    case 0x17:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 0x14);
        break;
    case 0x18:
        __MapActor_SetAnim(slot, 0xa);
        __MapActor_SetAnimSpeed(slot, 8);
        break;
    case 0x15:
    case 0x19:
        __MapActor_SetAnim(slot, 5);
        __MapActor_SetAnimSpeed(slot, 4);
        break;
    case 0x1a:
        __MapActor_SetAnim(slot, 9);
        __MapActor_SetAnimSpeed(slot, 4);
        break;
    }
}
