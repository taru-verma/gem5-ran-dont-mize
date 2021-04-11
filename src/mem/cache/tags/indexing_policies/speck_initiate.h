#ifndef SPECK_TEST_H
#define SPECK_TEST_H

//#include "params/SetAssociative.hh"

uint8_t * speck_prepare_address(const Addr);
uint8_t * speck_addr_encrypt(uint32_t [], int);
uint8_t * speck_addr_decrypt(uint32_t [], int);
Addr speck_encrypt_wrapper(const Addr, int);
Addr speck_decrypt_wrapper(const Addr, int);
void speck_dummy_landing();

#endif
