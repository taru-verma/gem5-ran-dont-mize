#ifndef SPECK_TEST_H
#define SPECK_TEST_H

#include "params/SetAssociative.hh"

void speck_show_array(const char *, const uint8_t *, size_t);
uint8_t * speck_prepare_address(const Addr);
uint8_t * speck_addr_encrypt(uint8_t *, size_t);
uint8_t * speck_addr_decrypt(uint8_t *, size_t);
void speck_dummy_landing();

#endif
