#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;
const int BLOCK_SIZE = 64;
typedef uint32_t WORD;
WORD rotl(WORD x, int n);
WORD P1(WORD x);
WORD P0(WORD x);
WORD FF(WORD x, WORD y, WORD z, WORD j);
WORD GG(WORD x, WORD y, WORD z, WORD j);
void padding(const uint8_t* message, size_t message_len, uint8_t* padded_message, size_t& padded_message_len);

void reduced_CF(WORD* V, WORD* B) {
    WORD A = V[0], B_ = V[1], C = V[2], D = V[3], E = V[4], F = V[5], G = V[6], H = V[7];
    WORD TT1, TT2;

    for (int j = 0; j < 16; j++) {
        TT1 = A + FF(B_, C, D, j) + rotl(E, 12) + B[j];
        TT2 = G + GG(H, A, B_, j) + rotl(F, 9) + rotl(B[j], 7);

        E = D;
        D = rotl(C, 9);
        C = B_;
        B_ = TT1;
        H = G;
        G = rotl(F, 19);
        F = A;
        A = P0(TT2);
    }

    V[0] ^= A;
    V[1] ^= B_;
    V[2] ^= C;
    V[3] ^= D;
    V[4] ^= E;
    V[5] ^= F;
    V[6] ^= G;
    V[7] ^= H;
}

void reduced_SM3(const uint8_t* message, size_t message_len, uint32_t* digest) {
	uint8_t padding_message[BLOCK_SIZE * 100];
	size_t padding_message_len;
	WORD B[16];
	WORD V[8] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e };
	padding(message, message_len, padding_message, padding_message_len);
	for (int i = 0; i < padding_message_len; i += BLOCK_SIZE) {
		for (int j = 0; j < 16; ++j) {
			B[j] = 0;
			for (int k = 0; k < 4; ++k) {
				B[j] += padding_message[i + j * 4 + k] << 8 * (3 - k);
			}
		}
		reduced_CF(V, B);
	}
	for (int i = 0; i <= 7; ++i) {
		digest[i] = V[i];
	}
	return;
}

