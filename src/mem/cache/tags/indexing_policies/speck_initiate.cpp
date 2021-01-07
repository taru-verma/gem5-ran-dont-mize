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

#include "speck_initiate.h"
#include "speck.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <random>
#include <bits/byteswap.h>
#include <inttypes.h>

static const uint8_t s_key_128256_stream[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

uint8_t ptx_stream[16];

void speck_show_array(const char *explain, const uint8_t *array, size_t len) {
    printf("%20s ", explain);
    for(int i=len-1; i >= 0; i--) {
        printf("%02x ", array[i]);
    }
    printf("\n");
}

uint8_t * speck_prepare_address(const uint64_t addr) {
    uint64_t swapped = __bswap_constant_64(addr);
    printf("\tReceived address (big endian): %" PRIu64 ", \
        Swapped address (little endian): %" PRIu64 "\n", addr, swapped);

    char addr_hex[17], swapped_hex[17];
    sprintf(addr_hex, "%016lx", addr);
    addr_hex[16] = 0;
    sprintf(swapped_hex, "%016lx", swapped);
    swapped_hex[16] = 0;
    printf("\tReceived address in hex: %s, \
        Swapped address in hex: %s\n", addr_hex, swapped_hex);

    for (int j = 0; *(swapped_hex + j) != '\0'; j += 2)
    {
        char value1 = *(swapped_hex + j), value2 = *(swapped_hex + j+1);
        int num1 = 0, num2 = 0;
        if (value1 >= '0' && value1 <= '9')
            num1 = value1 - '0';
        else if (value1 >= 'a' && value1 <= 'f')
            num1 = 10 + (value1 - 'a');
        if (value2 >= '0' && value2 <= '9')
            num2 = value2 - '0';
        else if (value2 >= 'a' && value2 <= 'f')
            num2 = 10 + (value2 - 'a');
        ptx_stream[7 - j/2] = 16 * num1 + num2;
    }
    for (int i = 8; i < 16; ++i)
        ptx_stream[i] = 0;

    return ptx_stream;
}

uint8_t * speck_addr_encrypt(uint8_t *ptx_stream, size_t siz) {
    speck_ctx_t *ctx = speck_init(SPECK_ENCRYPT_TYPE_128_256, s_key_128256_stream, sizeof(s_key_128256_stream));
    if(!ctx)
    {
        printf("\t*** ERROR: ctx not initialized\n");
        return NULL;
    }

    uint8_t *plain_text = (uint8_t*) calloc(1, siz);
    uint8_t *encrypted_text = (uint8_t*) calloc(1, siz);
    memcpy(plain_text, ptx_stream, siz);
    speck_ecb_encrypt(ctx, plain_text, encrypted_text, siz);

    free(plain_text);
    speck_finish(&ctx);
    return encrypted_text;
}

uint8_t * speck_addr_decrypt(uint8_t *ctx_stream, size_t siz) {
    speck_ctx_t *ctx = speck_init(SPECK_ENCRYPT_TYPE_128_256, s_key_128256_stream, sizeof(s_key_128256_stream));
    if(!ctx)
    {
        printf("\t*** ERROR: ctx not initialized\n");
        return NULL;
    }

    uint8_t *decrypted_text = (uint8_t*) calloc(1, siz);
    speck_ecb_decrypt(ctx, ctx_stream, decrypted_text, siz);

    speck_finish(&ctx);
    return decrypted_text;
}

void speck_dummy_landing() {
    printf("Hello World from Speck\n");
}

/*
int main() {
    printf("Speck Encryption\n");
    size_t siz = 16;
    uint8_t *plain_text = (uint8_t*) calloc(1, siz);
    uint8_t *encrypted_text = (uint8_t*) calloc(1, siz);
    uint8_t *decrypted_text = (uint8_t*) calloc(1, siz);

    plain_text = speck_prepare_address(618508);
    speck_show_array("\tplain text     :", plain_text, siz);

    encrypted_text = speck_addr_encrypt(plain_text, siz);
    speck_show_array("\tencrypted text :", encrypted_text, siz);

    uint64_t addr = 0;
    for (int i = 0; i < 7; ++i)
        addr += encrypted_text[i] * pow(256, i);
    printf("\tFinal Addr: %" PRIx64 ", Set: %" PRIu64 "\n", addr, (addr >> 6) & 63);

    decrypted_text = speck_addr_decrypt(encrypted_text, siz);
    speck_show_array("\tdecrypted text :", decrypted_text, siz);

    for (int i = 0; i < siz; i++)
        if (plain_text[i] != decrypted_text[i])
            printf("\tdecrypted error idx:%d  0x%02x != 0x%02x\n", i, plain_text[i], decrypted_text[i]);

    free(decrypted_text);
    free(encrypted_text);

    return 0;
}
*/
