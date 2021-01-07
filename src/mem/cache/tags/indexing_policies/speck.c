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

#include "speck.h"
#include <stdbool.h>
#include <stdlib.h>
#include "speck_private.h"

#define LANE_NUM 1

static inline void speck_round(uint32_t *x, uint32_t *y, const uint64_t *k) {
    *x = (*x >> 8) | (*x << (8 * sizeof(*x) - 8));  // x = ROTR(x, 8)
    *x += *y;
    *x ^= *k;
    *y = (*y << 3) | (*y >> (8 * sizeof(*y) - 3));  // y = ROTL(y, 3)
    *y ^= *x;
}

static inline void speck_back(uint32_t *x, uint32_t *y, const uint64_t *k) {
    *y ^= *x;
    *y = (*y >> 3) | (*y << (8 * sizeof(*y) - 3));  // y = ROTR(y, 3)
    *x ^= *k;
    *x -= *y;
    *x = (*x << 8) | (*x >> (8 * sizeof(*x) - 8));  // x = ROTL(x, 8)
}

void speck_encrypt(speck_ctx_t *ctx, const uint32_t plaintext[2], uint32_t ciphertext[2]) {
    ciphertext[0] = plaintext[0];
    ciphertext[1] = plaintext[1];
    for (int i = 0; i < ctx->round; i++) {
        speck_round(&ciphertext[1], &ciphertext[0], &ctx->key_schedule[i]);
    }
}

void speck_decrypt(speck_ctx_t *ctx, const uint32_t ciphertext[2], uint32_t decrypted[2]) {
    decrypted[0] = ciphertext[0];
    decrypted[1] = ciphertext[1];
    for (int i = ctx->round; i > 0; i--) {
        speck_back(&decrypted[1], &decrypted[0], &ctx->key_schedule[i - 1]);
    }
}

speck_ctx_t *speck_init(enum speck_encrypt_type type, const uint8_t *key, int key_len) {
    if (key == NULL) return NULL;
    if (!is_validate_key_len(type, key_len)) return NULL;

    speck_ctx_t *ctx = (speck_ctx_t *)calloc(1, sizeof(speck_ctx_t));
    if (!ctx) return NULL;
    ctx->type = type;
    ctx->round = get_round_num(type);

    ctx->key_schedule = calloc(1, ctx->round * sizeof(uint64_t));
    if (!ctx->key_schedule) return NULL;

    // calc key schedule
    uint32_t b;
    uint32_t a;
    uint64_t k;
    int key_words_num = get_key_words_num(ctx->type);
    uint64_t keys[MAX_KEY_WORDS];
    for (int i = 0; i < key_words_num; i++) {
        cast_uint8_array_to_uint64(&keys[i], key + (WORDS * i));
    }
    ctx->key_schedule[0] = keys[0];
    for (int i = 0; i < ctx->round - 1; i++) {
        b = keys[0];
        a = keys[1];
        k = (uint64_t)i;
        speck_round(&a, &b, &k);
        keys[0] = b;

        if (key_words_num != 2) {
            for (int j = 1; j < (key_words_num - 1); j++) {
                keys[j] = keys[j + 1];
            }
        }
        keys[key_words_num - 1] = a;

        ctx->key_schedule[i + 1] = keys[0];
    }

    return ctx;
}

void speck_finish(speck_ctx_t **ctx) {
    if (!ctx) return;
    free((*ctx)->key_schedule);
    free(*ctx);
}
