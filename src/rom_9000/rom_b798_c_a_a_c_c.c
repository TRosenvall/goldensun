extern void DeleteSpriteLayer(void *layer);

void Sprite_DeleteLayerIndex(char *a, unsigned int i)
{
    unsigned int j;
    int count;
    int off;
    void *layer;

    if (a == 0)
        return;
    if (i > 3)
        return;
    off = i * 4 + 0x28;
    layer = *(void **)(a + off);
    if (layer == 0)
        return;
    DeleteSpriteLayer(layer);
    *(void **)(a + off) = 0;
    count = 0;
    for (j = i + 1; j <= 3; j++) {
        if (*(void **)(a + 0x28 + j * 4) != 0)
            count++;
    }
    if (count == 0)
        *(unsigned char *)(a + 0x27) = i;
}
