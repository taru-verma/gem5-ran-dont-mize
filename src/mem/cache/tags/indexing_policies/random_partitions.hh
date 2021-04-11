/**
 * @file
 * Declaration of an indexing policy for randomized caches with partitions
 */

#ifndef __MEM_CACHE_INDEXING_POLICIES_RANDOMIZED_PARTITIONS_HH__
#define __MEM_CACHE_INDEXING_POLICIES_RANDOMIZED_PARTITIONS_HH__

#include <vector>

#include "mem/cache/tags/indexing_policies/base.hh"
#include "params/RandomPartitions.hh"

class ReplaceableEntry;

class RandomPartitions : public BaseIndexingPolicy
{
  protected:
    /**
     * Apply a hash function to calculate address set.
     *
     * @param addr The address to calculate the set for.
     * @return The set index for given combination of address and way.
     */
    virtual uint32_t extractSet(const Addr addr) const;

  public:
    /**
     * Convenience typedef.
     */
    typedef RandomPartitionsParams Params;

    /**
     * Construct and initialize this policy.
     */
    RandomPartitions(const Params &p);

    /**
     * Destructor.
     */
    ~RandomPartitions() {};

    /**
     * Find all possible entries for insertion and replacement of an address.
     * Should be called immediately before ReplacementPolicy's findVictim()
     * not to break cache resizing.
     * Returns entries in all ways belonging to the set of the address.
     *
     * @param addr The addr to a find possible entries for.
     * @return The possible entries.
     */
    std::vector<ReplaceableEntry*> getPossibleEntries(const Addr addr) const
                                                                     override;

    /**
     * Find all possible entries for finding the block with randomized caches.
     * Should be called immediately before ReplacementPolicy's findVictim()
     * not to break cache resizing.
     * Returns entries in all ways belonging to the set of the address.
     *
     * @param addr The addr to a find possible entries for.
     * @return The possible entries.
     */
    std::vector<ReplaceableEntry*> getAllPossibleEntries(const Addr addr) const
                                                                     override;

    /**
     * Regenerate an entry's address from its tag and assigned set and way.
     *
     * @param tag The tag bits.
     * @param orig_set The original set.
     * @param entry The entry.
     * @return the entry's original addr value.
     */
    Addr regenerateAddr(const Addr tag, const Addr orig_set, const ReplaceableEntry* entry) const
                                                                   override;

    /**
     * Wrapper function to protected extractSet() - required for reconstructing unencrypted address
     *
     * @param addr The address to extract set from.
     * @return The set index for given address.
     */
    Addr extractOrigSet(const Addr addr) const 
                                       override;
};

#endif //__MEM_CACHE_INDEXING_POLICIES_SET_ASSOCIATIVE_HH__
