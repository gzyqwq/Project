#include <iostream>
#include <cstdio>
#include <osrng.h>
#include <hex.h>
using namespace CryptoPP;
using namespace std::chrono;
using namespace std;
const int BLOCK_SIZE = 64;

uint32_t hash[8];

void f(uint32_t* x,uint32_t *fx) {
	uint8_t message[32];
	for (int i = 0; i < 8; ++i) {
		message[4 * i] = x[i] & 0xff;
		message[4 * i + 1] = (x[i] >> 8) & 0xff;
		message[4 * i + 2] = (x[i] >> 16) & 0xff;
		message[4 * i + 3] = (x[i] >> 24) & 0xff;
	}
	uint32_t hash[8];
	reduced_SM3(message, hash);
	for (int i = 0; i < 8; ++i)
		fx[i] = hash[i];
}

bool is_com(uint32_t* x,uint32_t* y) {
	for (int i = 0; i < 8; ++i) {
		if (x[i] != y[i]) return false;
	}
	return true;
}

void rho_method(uint8_t *m) {
	auto start = high_resolution_clock::now();
	uint32_t x[8], y[8];
	reduced_SM3(m, x);
	f(x,y);
	uint32_t* last_x = x;
	uint32_t* last_y = y;
	int cnt = 1;
	while (is_com(x,y) == 0) {
		cnt++;
		if (cnt % 10000000 == 0) {
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);
			cout << "Ö´ÐÐÊ±¼ä£º " << 1.0 * duration.count() / 1000000 << " Ãë " << endl;
		}
		f(x,x);
		f(y, y);
		f(y, y);
	}
	reduced_SM3(m,x);
	while (is_com(x,y) == 0) {
		last_x = x;
		last_y = y;
		cnt++;
		
		f(x, x);
		f(y, y);
	}
	puts("find a collision");
	for (int i = 0; i < 8; ++i) cout << hex << last_x[i] << ' ';
	puts("");
	for (int i = 0; i < 8; ++i) cout << hex << last_y[i] << ' ';
	return;
}

int main() {
	CryptoPP::AutoSeededRandomPool rng;
	uint8_t message[BLOCK_SIZE];
	rng.GenerateBlock(message, sizeof(message));
	rho_method(message);
	return 0;
}