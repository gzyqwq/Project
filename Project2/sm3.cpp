#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;
const int BLOCK_SIZE = 64;
typedef uint32_t WORD;

inline WORD rotl(WORD x, int n) {
	return (x << n) | (x >> (32 - n));
}

inline WORD P1(WORD x) {
	return x ^ rotl(x, 15) ^ rotl(x, 23);
}

inline WORD P0(WORD x) {
	return x ^ rotl(x, 9) ^ rotl(x, 17);
}

inline WORD FF(WORD x, WORD y, WORD z, WORD j) {
	if (j >= 0 && j <= 15) { return x ^ y ^ z; }
	else { return (x & y) | (x & z) | (y & z); }
}

inline WORD GG(WORD x, WORD y, WORD z, WORD j) {
	if (j >= 0 && j <= 15) { return x ^ y ^ z; }
	else { return (x & y) | ((~x) & z); }
}



void CF(WORD* V, const WORD* B_) {
	WORD A = V[0], B = V[1], C = V[2], D = V[3], E = V[4], F = V[5], G = V[6], H = V[7], SS1, SS2, TT1, TT2;
	WORD W[68];
	WORD W_[64];
	WORD T[64];
	for (int i = 0; i <= 15; ++i) {
		T[i] = 0x79cc4519;
	}
	for (int i = 16; i <= 63; ++i) {
		T[i] = 0x7a879d8a;
	}
	for (int i = 0; i < 16; ++i) W[i] = B_[i];
	for (int j = 16; j < 68; j++) {
		W[j] = P1(W[j - 16] ^ W[j - 9] ^ rotl(W[j - 3], 15)) ^ rotl(W[j - 13], 7) ^ W[j - 6];
	}
	for (int i = 0; i <= 63; ++i) {
		W_[i] = W[i] ^ W[i + 4];
	}
	for (int i = 0; i <= 63; ++i) {
		SS1 = rotl((rotl(A, 12) + E + rotl(T[i], i)), 7);
		SS2 = SS1 ^ rotl(A, 12);
		TT1 = FF(A, B, C, i) + D + SS2 + W_[i];
		TT2 = GG(E, F, G, i) + H + SS1 + W[i];
		D = C;
		C = rotl(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = rotl(F, 19);
		F = E;
		E = P0(TT2);
	}
	WORD tmp[8] = { A,B,C,D,E,F,G,H };
	for (int i = 0; i <= 7; ++i) {
		V[i] = tmp[i] ^ V[i];
	}
	return;
}

void padding(const uint8_t* message, size_t message_len, uint8_t* padded_message, size_t& padded_message_len) {
	size_t remainder = message_len % BLOCK_SIZE;
	size_t padding_len = (remainder < 56) ? (56 - remainder) : (120 - remainder);
	padded_message_len = message_len + padding_len + 8;

	memcpy(padded_message, message, message_len);
	padded_message[message_len] = 0x80;
	memset(padded_message + message_len + 1, 0, padding_len - 1);

	uint64_t message_bits = (uint64_t)message_len * 8;
	padded_message[padded_message_len - 8] = (message_bits >> 56) & 0xff;
	padded_message[padded_message_len - 7] = (message_bits >> 48) & 0xff;
	padded_message[padded_message_len - 6] = (message_bits >> 40) & 0xff;
	padded_message[padded_message_len - 5] = (message_bits >> 32) & 0xff;
	padded_message[padded_message_len - 4] = (message_bits >> 24) & 0xff;
	padded_message[padded_message_len - 3] = (message_bits >> 16) & 0xff;
	padded_message[padded_message_len - 2] = (message_bits >> 8) & 0xff;
	padded_message[padded_message_len - 1] = message_bits & 0xff;
}

void SM3(const uint8_t* message, uint32_t* digest) {
	uint8_t padding_message[BLOCK_SIZE * 100];
	size_t padding_message_len;
	size_t message_len = strlen((const char*)message);
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
		CF(V, B);
	}
	for (int i = 0; i <= 7; ++i) {
		digest[i] = V[i];
	}
	return;
}

