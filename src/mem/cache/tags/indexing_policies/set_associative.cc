/*
 * Copyright (c) 2018 Inria
 * Copyright (c) 2012-2014,2017 ARM Limited
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2003-2005,2014 The Regents of The University of Michigan
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

/**
 * @file
 * Definitions of a set associative indexing policy.
 */

#include "mem/cache/tags/indexing_policies/set_associative.hh"

#include "mem/cache/replacement_policies/replaceable_entry.hh"

#include "speck_initiate.h"

#include <math.h>

SetAssociative::SetAssociative(const Params &p)
    : BaseIndexingPolicy(p)
{
}

uint32_t
SetAssociative::extractSet(const Addr addr) const
{
    ////speck_dummy_landing();

    printf("Address: %" PRIu64 ", %" PRIx64 "\n", addr, addr);

    //uint32_t extracted_set = (addr >> setShift) & setMask;
    //extracted_set = (extracted_set + 9) % 64;
    //printf("\textracted_set: %u\n", extracted_set);
    //return extracted_set;

    //printf("\textracted_set: %lu\n", (addr >> setShift) & setMask);

    size_t siz = 16;
    uint8_t *plain_text = (uint8_t*) calloc(1, siz);
    uint8_t *encrypted_text = (uint8_t*) calloc(1, siz);
    //uint8_t *decrypted_text = (uint8_t*) calloc(1, siz);

    plain_text = speck_prepare_address(addr);
    speck_show_array("\tplain text     :", plain_text, siz);

    encrypted_text = speck_addr_encrypt(plain_text, siz);
    speck_show_array("\tencrypted text :", encrypted_text, siz);
    uint64_t new_addr = 0;
    for (int i = 0; i < 7; ++i)
        new_addr += encrypted_text[i] * pow(256, i);
    printf("\tFinal Addr: %" PRIx64 ", Set: %" PRIu64 "\n", new_addr, (new_addr >> 6) & 63);

    //decrypted_text = speck_addr_decrypt(encrypted_text, siz);
    //speck_show_array("\tdecrypted text :", decrypted_text, siz);

    return (new_addr >> setShift) & setMask;
    //return (addr >> setShift) & setMask;
}

Addr
SetAssociative::regenerateAddr(const Addr tag, const ReplaceableEntry* entry)
                                                                        const
{
    //return (tag << tagShift) | (((entry->getSet() - 9) % 64) << setShift);
    printf("\t\tTag: %" PRIu64 ", ShiftedTag: %" PRIu64 ", set: %" PRIu32 " \n", tag, (tag << tagShift), entry->getSet());
    printf("\t\tAddress returned is: %" PRIu64 "\n", (tag << tagShift) | (entry->getSet() << setShift));
    return (tag << tagShift) | (entry->getSet() << setShift);
}

std::vector<ReplaceableEntry*>
SetAssociative::getPossibleEntries(const Addr addr) const
{
    return sets[extractSet(addr)];
}
