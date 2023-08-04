#include <iostream>
#include <cstring>
#include <arm_neon.h>


using namespace std;
// AES¼ÓÃÜº¯Êı
void aes_encrypt(const uint8_t* in, const uint8_t* key, uint8_t* out) {
    const uint8_t* rk = key;
    uint8x16_t state = vld1q_u8(in);
    uint8x16_t roundkey;

    roundkey = vld1q_u8(rk);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = vaeseq_u8(state, roundkey);
    rk += 16;

    roundkey = vld1q_u8(rk);
    state = vaesmcq_u8(state);
    state = veorq_u8(state, roundkey);

    vst1q_u8(out, state);
}

int main() {
    uint8_t key[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
    uint8_t in[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
    uint8_t out[16];

    aes_encrypt(in, key, out);
    for (int i = 0; i < 16; i++) {printf("%02x ", out[i]);}

    return 0;
}
