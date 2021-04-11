/**
 * @file
 * Definitions of an indexing policy for randomized caches with partitions.
 */

#include "mem/cache/tags/indexing_policies/random_partitions.hh"

#include "mem/cache/replacement_policies/replaceable_entry.hh"

#include "mem/cache/tags/indexing_policies/speck_initiate.h"

#include <math.h>

RandomPartitions::RandomPartitions(const Params &p)
    : BaseIndexingPolicy(p)
{
}

uint32_t
RandomPartitions::extractSet(const Addr addr) const
{
    return (addr >> setShift) & setMask;
}

Addr
RandomPartitions::regenerateAddr(const Addr tag, const Addr orig_set, const ReplaceableEntry* entry)
                                                                        const
{
    // disable_randomization - change `orig_set` to `entry->getSet()`
    return (tag << tagShift) | (orig_set << setShift);
}

std::vector<ReplaceableEntry*>
RandomPartitions::getPossibleEntries(const Addr addr) const
{
    std::vector<ReplaceableEntry*> entries;
    Addr encrypted_addr0 = speck_encrypt_wrapper(addr >> 6, 0);
    Addr encrypted_addr1 = speck_encrypt_wrapper(addr >> 6, 1);
    Addr encrypted_addr2 = speck_encrypt_wrapper(addr >> 6, 2);
    Addr encrypted_addr3 = speck_encrypt_wrapper(addr >> 6, 3);
    int num_partitions = 4;
    int random_partition = rand() % num_partitions;
    if (random_partition == 0)
        for (uint32_t way = 0; way < 2; ++way)
            entries.push_back(sets[extractSet(encrypted_addr0)][way]);
    else if (random_partition == 1)
        for (uint32_t way = 2; way < 4; ++way)
            entries.push_back(sets[extractSet(encrypted_addr1)][way]);
    else if (random_partition == 2)
        for (uint32_t way = 4; way < 6; ++way)
            entries.push_back(sets[extractSet(encrypted_addr2)][way]);
    else
        for (uint32_t way = 6; way < 8; ++way)
            entries.push_back(sets[extractSet(encrypted_addr3)][way]);

    return entries;
}

std::vector<ReplaceableEntry*>
RandomPartitions::getAllPossibleEntries(const Addr addr) const
{
    std::vector<ReplaceableEntry*> entries;
    Addr encrypted_addr0 = speck_encrypt_wrapper(addr >> 6, 0);
    Addr encrypted_addr1 = speck_encrypt_wrapper(addr >> 6, 1);
    Addr encrypted_addr2 = speck_encrypt_wrapper(addr >> 6, 2);
    Addr encrypted_addr3 = speck_encrypt_wrapper(addr >> 6, 3);
    for (uint32_t way = 0; way < 2; ++way)
        entries.push_back(sets[extractSet(encrypted_addr0)][way]);
    for (uint32_t way = 2; way < 4; ++way)
        entries.push_back(sets[extractSet(encrypted_addr1)][way]);
    for (uint32_t way = 4; way < 6; ++way)
        entries.push_back(sets[extractSet(encrypted_addr2)][way]);
    for (uint32_t way = 6; way < 8; ++way)
        entries.push_back(sets[extractSet(encrypted_addr3)][way]);

    return entries;
}

Addr RandomPartitions::extractOrigSet(const Addr addr) const
{
    return extractSet(addr);
}
