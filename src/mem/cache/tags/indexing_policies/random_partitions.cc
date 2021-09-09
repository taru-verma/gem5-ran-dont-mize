/**
 * @file
 * Definitions of an indexing policy for randomized caches with partitions.
 */

#include "mem/cache/tags/indexing_policies/random_partitions.hh"

#include "mem/cache/replacement_policies/replaceable_entry.hh"

#include "mem/cache/tags/indexing_policies/speck_initiate.h"

#include <math.h>

#define NUM_PARTITIONS 1 

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
    return (tag << tagShift) | (orig_set << setShift);
}

std::vector<ReplaceableEntry*>
RandomPartitions::getPossibleEntries(const Addr addr) const
{
    std::vector<ReplaceableEntry*> entries;
    int i = 0, partition_size = assoc/NUM_PARTITIONS;

    // Pick a partition at random and get the encrypted address for that partition
    // Don't use extractSet() since you encrypt the tag+set portion of the address
    int random_partition = rand() % NUM_PARTITIONS;
    Addr encrypted_addr = speck_encrypt_wrapper(addr >> setShift, random_partition);

    // Get all ways for the set in that partition, replacement policy chooses where to put.
    for (i = 0; i < partition_size; ++i)
        entries.push_back(sets[extractSet(encrypted_addr)][random_partition*partition_size + i]);

    return entries;
}

std::vector<ReplaceableEntry*>
RandomPartitions::getAllPossibleEntries(const Addr addr) const
{
    std::vector<ReplaceableEntry*> entries;
    std::vector<Addr> encrypted_addresses;
    int i = 0, j = 0;

    // Generate encrypted addresses for all partitions since 
    // while writing partition was chosen randomly
    for (i = 0; i < NUM_PARTITIONS; ++i)
        encrypted_addresses.push_back(speck_encrypt_wrapper(addr >> setShift, i));

    // Return all ways for the set in each partition
    for (i = 0; i < NUM_PARTITIONS; ++i)
        for (j = 0; j < assoc/NUM_PARTITIONS; ++j)
            entries.push_back(sets[extractSet(encrypted_addresses[i])][j+i*assoc/NUM_PARTITIONS]);

    return entries;
}

Addr RandomPartitions::extractOrigSet(const Addr addr) const
{
    return extractSet(addr);
}