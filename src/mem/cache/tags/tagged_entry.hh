/**
 * Copyright (c) 2020 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CACHE_TAGGED_ENTRY_HH__
#define __CACHE_TAGGED_ENTRY_HH__

#include <cassert>

#include "base/cprintf.hh"
#include "base/types.hh"
#include "mem/cache/replacement_policies/replaceable_entry.hh"

/**
 * A tagged entry is an entry containing a tag. Each tag is accompanied by a
 * secure bit, which informs whether it belongs to a secure address space.
 * A tagged entry's contents are only relevant if it is marked as valid.
 */
class TaggedEntry : public ReplaceableEntry
{
  public:
    TaggedEntry() : _valid(false), _secure(false), _tag(MaxAddr), _orig_set(MaxAddr) {}
    ~TaggedEntry() = default;

    /**
     * Checks if the entry is valid.
     *
     * @return True if the entry is valid.
     */
    virtual bool isValid() const { return _valid; }

    /**
     * Check if this block holds data from the secure memory space.
     *
     * @return True if the block holds data from the secure memory space.
     */
    bool isSecure() const { return _secure; }

    /**
     * Get tag associated to this block.
     *
     * @return The tag value.
     */
    virtual Addr getTag() const { return _tag; }

    /**
     * Get original set associated to this block.
     *
     * @return The original set value.
     */
    virtual Addr getOrigSet() const { return _orig_set; }

    /**
     * Checks if the given tag information corresponds to this entry's.
     *
     * @param tag The tag value to compare to.
     * @param is_secure Whether secure bit is set.
     * @return True if the tag information match this entry's.
     */
    virtual bool
    matchTag(Addr tag, bool is_secure) const
    {
        return isValid() && (getTag() == tag) && (isSecure() == is_secure);
    }

    /**
     * Checks if the given set information corresponds to this entry's unencrypted set
     *
     * @param set The set value to compare to.
     * @return True if the given set information matches this entry's unencrypted set
     */
    virtual bool
    matchOrigSet(Addr set) const
    {
        return isValid() && (getOrigSet() == set);
    }

    /**
     * Insert the block by assigning it a tag and marking it valid. Touches
     * block if it hadn't been touched previously.
     *
     * @param tag The tag value.
     */
    virtual void
    insert(const Addr tag, const bool is_secure)
    {
        setValid();
        setTag(tag);
        if (is_secure) {
            setSecure();
        }
    }

    /**
     * Update the original set of the inserted block 
     *
     * @param orig_set The original set.
     */
    virtual void
    update_set(const Addr orig_set)
    {
        setOrigSet(orig_set);
    }

    /** Invalidate the block. Its contents are no longer valid. */
    virtual void invalidate()
    {
        _valid = false;
        setTag(MaxAddr);
        setOrigSet(MaxAddr);
        clearSecure();
    }

    std::string
    print() const override
    {
        return csprintf("tag: %#x orig_set: %#x secure: %d valid: %d | %s", getTag(), getOrigSet(),
            isSecure(), isValid(), ReplaceableEntry::print());
    }

  protected:
    /**
     * Set tag associated to this block.
     *
     * @param tag The tag value.
     */
    virtual void setTag(Addr tag) { _tag = tag; }

    /**
     * Set original set associated to this block.
     *
     * @param orig_set The original set value.
     */
    virtual void setOrigSet(Addr orig_set) { _orig_set = orig_set; }

    /** Set secure bit. */
    virtual void setSecure() { _secure = true; }

    /** Set valid bit. The block must be invalid beforehand. */
    virtual void
    setValid()
    {
        assert(!isValid());
        _valid = true;
    }

  private:
    /**
     * Valid bit. The contents of this entry are only valid if this bit is set.
     * @sa invalidate()
     * @sa insert()
     */
    bool _valid;

    /**
     * Secure bit. Marks whether this entry refers to an address in the secure
     * memory space. Must always be modified along with the tag.
     */
    bool _secure;

    /** The entry's tag. */
    Addr _tag;

    /** The entry's original set before encryption. */
    Addr _orig_set;

    /** Clear secure bit. Should be only used by the invalidation function. */
    void clearSecure() { _secure = false; }
};

#endif//__CACHE_TAGGED_ENTRY_HH__
