# include <stdint.h>
# include <stdio.h>
# include <inttypes.h>

unsigned lfsr3(void)
{
    uint16_t start_state = 0xACE1;  /* Any nonzero start state will work. */
    uint16_t lfsr = start_state;
    unsigned period = 0;

    do
    {
        lfsr ^= lfsr >> 7;
        lfsr ^= lfsr << 9;
        lfsr ^= lfsr >> 1;
        //printf("%" PRIx16 "\n", lfsr);
        //printf("\t%" PRIx16 "\n", lfsr << 6);
        //printf("\t\t%" PRIx16 "\n", (lfsr << 6) & 0x7fff);
        uint16_t randomizer = (lfsr << 6) & 0x7fc0;
        uint32_t address = 0xf04010 + randomizer;
        printf("%" PRIx32 "\n", address);
        printf("\tTag: %x, Set: %x\n", ((address >> 12) & 7), ((address >> 6) & 63));
        ++period;
    }
    while (lfsr != start_state);

    return period;
}

int main()
{
    printf("%d\n", lfsr3());
    return 0;
}