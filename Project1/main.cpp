#include <iostream>
#include <osrng.h>
#include <hex.h>
#include <cstring>
#include <map>
#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <mutex>
using namespace CryptoPP;
using namespace std;
using namespace std::chrono;
const int BLOCK_SIZE = 64;

void SM3(const uint8_t* message, size_t message_len, uint32_t* digest);
void reduced_SM3(const uint8_t* message, size_t message_len, uint32_t* digest);

bool operator<(const array<uint32_t, 8>& a, const array<uint32_t, 8>& b) {
    return memcmp(&a, &b, sizeof(a)) < 0;
}
std::mutex mtx;

map<array<uint32_t, 8>, array<uint8_t, BLOCK_SIZE>> myMap;
void thread_func(int id, int N) {
    CryptoPP::AutoSeededRandomPool rng;
    uint8_t message[BLOCK_SIZE];
    uint32_t digest[8];

    for (int i = 0; i < N; ++i) {
        rng.GenerateBlock(message, sizeof(message));
        SM3(message, BLOCK_SIZE, digest);
        mtx.lock();
        if (myMap.find(*(reinterpret_cast<array<uint32_t, 8>*>(digest))) != myMap.end()) {
            cout << "find a collision: " << endl;
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                printf("%02x", message[i]);
            }
            puts("");
            uint8_t tmp[BLOCK_SIZE];
            std::copy(myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))].begin(), myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))].end(), tmp);
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                printf("%02x", tmp[i]);
            }
            cout << endl;
        }
        myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))] = *(reinterpret_cast<array<uint8_t, BLOCK_SIZE>*>(message));
        mtx.unlock();
    }
}



int main() {
    auto start = high_resolution_clock::now();
    /*
    const int T = 1000000;
    const int N = T / 16;
    std::thread threads[16];
    for (int i = 0; i < 16; ++i) {
        threads[i] = std::thread(thread_func, i, N);
    }
    for (int i = 0; i < 16; ++i) {
        threads[i].join();
    }*/
    
    CryptoPP::AutoSeededRandomPool rng;
    uint8_t message[BLOCK_SIZE];
    uint32_t digest[8];
    int T = 100000000;
    for (int i = 0; i < T; ++i) {
        rng.GenerateBlock(message, sizeof(message));
        reduced_SM3(message, BLOCK_SIZE, digest);
        if (myMap.find(*(reinterpret_cast<array<uint32_t, 8>*>(digest))) != myMap.end()) {
            cout << "find a collision: " << endl;
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                printf("%02x", message[i]);
            }
            puts("");
            uint8_t tmp[BLOCK_SIZE];
            std::copy(myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))].begin(), myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))].end(), tmp);
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                printf("%02x", tmp[i]);
            }
            cout << endl;
        }
        myMap[*(reinterpret_cast<array<uint32_t, 8>*>(digest))] = *(reinterpret_cast<array<uint8_t, BLOCK_SIZE>*>(message));
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Ö´ÐÐÊ±¼ä£º " << 1.0 * duration.count() / 1000000 << " Ãë " << endl;
    return 0;
}





//code : test for sm3

/*
#include <iostream>
#include <cstdio>
#include <cstring>
#include <random>
using namespace std;
const int BLOCK_SIZE = 64;

void SM3(const uint8_t* message, size_t message_len, uint32_t* digest);
void reduced_SM3(const uint8_t* message, size_t message_len, uint32_t* digest);

int main() {
    const uint8_t message[] = { "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd" };
    uint32_t digest[8];
    int len = strlen((const char*)message);
    reduced_SM3(message, len, digest);
    puts("");
    for (int i = 0; i < 8; ++i) cout << hex << digest[i] << ' ';
}
*/
