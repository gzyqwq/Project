#include <iostream>
#include <cstdio>
#include <cstring>
#include <chrono>
using namespace std;
using namespace std::chrono;
const int BLOCK_SIZE = 64;
void len_ex_attack_SM3(const uint8_t* message, uint32_t* digest, uint32_t* hash, size_t m);
/*参数分别为增加的消息x,输出哈希值digest,消息m的哈希值,长度|m|*/

int main() {
	auto start = high_resolution_clock::now();
	int T = 100000000;
	while (T--) {
		const uint8_t message[] = { "abc" };
		uint32_t digest[8], hash[8] = { 0x01,0x02,0x01,0x01,0x01,0x01,0x01,0x01 };
		size_t m = 128;
		len_ex_attack_SM3(message, digest, hash, m);
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "执行时间： " << 1.0 * duration.count() / 1000000 << "秒 " << endl;
	return 0;
}
//4.4
//43.98s