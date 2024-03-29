/*
 * Copyright (c) 2016-2017 Naruto TAKAHASHI <tnaruto@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//#include "speck_initiate.h"
#include "speck.h"
#include <stdio.h>
#include <byteswap.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#define NUM_BLOCKS 8    // 8 * (8 bits = 64 bits address
#define ADDR_SIZE 16    // Address from gem5 of type uint64_t, 16 * 4bits in each char

static const uint8_t keys[32][16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x02, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x03, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x04, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x07, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x09, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0a, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0c, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0d, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0e, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x11, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x12, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x13, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x14, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x15, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x16, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x17, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x18, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1a, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1c, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1d, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1e, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x1f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

uint32_t plain_text[NUM_BLOCKS];
uint32_t cipher_text[2] = {0x0, 0x0};
uint32_t decrypted_text[2] = {0x0, 0x0};

uint32_t * speck_prepare_address(const uint64_t addr) {
    memset(plain_text, 0x0, NUM_BLOCKS * sizeof(uint32_t));
 
    char addr_hex[ADDR_SIZE + 1];
    sprintf(addr_hex, "%016lx", addr);
    addr_hex[ADDR_SIZE] = 0;
    //printf("\tEncrypting address: %s\n", addr_hex);

    for (int j = 0; j < ADDR_SIZE;  j += 2)
    {
        char value1 = *(addr_hex + j), value2 = *(addr_hex + j + 1);
        int num1 = 0, num2 = 0;
        if (value1 >= '0' && value1 <= '9')
            num1 = value1 - '0';
        else if (value1 >= 'a' && value1 <= 'f')
            num1 = 10 + (value1 - 'a');
        if (value2 >= '0' && value2 <= '9')
            num2 = value2 - '0';
        else if (value2 >= 'a' && value2 <= 'f')
            num2 = 10 + (value2 - 'a');

        plain_text[(NUM_BLOCKS - 1) - j/2] = 16 * num1 + num2;
    }

    return plain_text;
}

uint32_t * speck_addr_encrypt(uint32_t plain_text[2], int partition) {

    uint8_t s_key_stream[16];
    memcpy(s_key_stream, keys[partition], sizeof(s_key_stream));

    speck_ctx_t *ctx = speck_init(SPECK_ENCRYPT_TYPE_128_128, s_key_stream, sizeof(s_key_stream));
    if(!ctx) return NULL;

    speck_encrypt(ctx, plain_text, cipher_text);
    
    speck_finish(&ctx);
    return cipher_text;
}

uint32_t * speck_addr_decrypt(uint32_t cipher_text[2], int partition) {
    uint8_t s_key_stream[16];
    memcpy(s_key_stream, keys[partition], sizeof(s_key_stream));

    speck_ctx_t *ctx = speck_init(SPECK_ENCRYPT_TYPE_128_128, s_key_stream, sizeof(s_key_stream));
    if(!ctx) return NULL;

    speck_decrypt(ctx, cipher_text, decrypted_text);

    speck_finish(&ctx);
    return decrypted_text;
}

uint64_t speck_encrypt_wrapper(const uint64_t addr, int partition) {
    uint32_t *ptx_address; 
    uint32_t *ptx = (uint32_t*) calloc(2, sizeof(uint32_t));
    uint32_t *ctx = (uint32_t*) calloc(2, sizeof(uint32_t));
    ptx[1] = 0x0; ptx[0] = 0x0;
    ctx[1] = 0x0; ctx[0] = 0x0;

    ptx_address = speck_prepare_address(addr);
    for (int i = NUM_BLOCKS - 1; i >= 0; i--)
        ptx[i/(NUM_BLOCKS/2)] += ptx_address[i] * pow(256, (i%(NUM_BLOCKS/2)));

    //printf("\tEncrypting: \n");
    ctx = speck_addr_encrypt(ptx, partition);
    
    uint64_t encrypted_addr = 0;
    encrypted_addr = ctx[1] * (uint64_t)pow(256, 4) + ctx[0];
    //printf("\tEncrypted Address: %" PRIx64 ", Set: %d\n", encrypted_addr, (int)(encrypted_addr >> 6) & 63);
    //printf("\n");

    return encrypted_addr;
}

uint64_t speck_decrypt_wrapper(const uint64_t addr, int partition) {
    uint32_t *ctx_address; 
    uint32_t *ctx = (uint32_t*) calloc(2, sizeof(uint32_t));
    uint32_t *dtx = (uint32_t*) calloc(2, sizeof(uint32_t));
    ctx[1] = 0x0; ctx[0] = 0x0;
    dtx[1] = 0x0; dtx[0] = 0x0;

    ctx_address = speck_prepare_address(addr);
    for (int i = NUM_BLOCKS - 1; i >= 0; i--)
        ctx[i/(NUM_BLOCKS/2)] += ctx_address[i] * pow(256, (i%(NUM_BLOCKS/2)));
 
    //printf("\tDecrypting: \n");
    dtx = speck_addr_decrypt(ctx, partition);

    uint64_t decrypted_addr = 0;
    decrypted_addr = dtx[1] * (uint64_t)pow(256, 4) + dtx[0];
    //printf("\tDecrypted Address: %" PRIx64 ", Set: %d\n", decrypted_addr, (int)(decrypted_addr >> 6) & 63);
    //printf("\n");

    return decrypted_addr;    
}

void speck_dummy_landing() {
    printf("Hello World from Speck!\n");
}

/*
int main() {
    uint64_t addr = 0x9700c;
    uint64_t getaddr = speck_encrypt_wrapper(0x9700c, 0);
    uint64_t retaddr = speck_decrypt_wrapper(getaddr, 0);

    printf("\t%s -> addr: %" PRIx64 ", enc: %" PRIx64 ", dec: %" PRIx64 "\n", addr == retaddr ? "Successful" : "Unsuccessful", addr, getaddr, retaddr);

    int num_partitions = 4, assoc = 16;
    int partition_size = assoc/num_partitions;
    int i = 0, j = 0;

    srand(42);
    int random_partition = 0;
    for (i = 0; i < 10; ++i)
    {
        random_partition = rand() % num_partitions;
        printf("Partition: %d -> ways: ", random_partition);
        for (j = 0; j < partition_size; ++j)
            printf("%d ", random_partition*partition_size+j);
        printf("\n");
    }

    uint64_t encrypted_addresses[num_partitions];
    // Generate encrypted addresses
    for (i = 0; i < num_partitions; ++i)
        encrypted_addresses[i] = speck_encrypt_wrapper(addr, i);

    for (i = 0; i < num_partitions; ++i)
        for (j = 0; j < assoc/num_partitions; ++j)
            printf("Partition: %d: addr: %" PRIx64 ", way: %d\n", i, encrypted_addresses[i], j+i*assoc/num_partitions);

    return 0;
}
*/
