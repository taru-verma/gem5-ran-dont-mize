#ifndef SPECK_TEST_H
#define SPECK_TEST_H

#include "params/SetAssociative.hh"

uint8_t * speck_prepare_address(const Addr);
uint8_t * speck_addr_encrypt(uint32_t []);
uint8_t * speck_addr_decrypt(uint32_t []);
Addr speck_encrypt_wrapper(const Addr);
Addr speck_decrypt_wrapper(const Addr);
void speck_dummy_landing();

#endif
