#include <iostream>
#include <cstdio>
#include <cstring>
#include <chrono>
using namespace std;
using namespace std::chrono;
const int BLOCK_SIZE = 64;
void len_ex_attack_SM3(const uint8_t* message, uint32_t* digest, uint32_t* hash, size_t m);
/*�����ֱ�Ϊ���ӵ���Ϣx,�����ϣֵdigest,��Ϣm�Ĺ�ϣֵ,����|m|*/

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
	cout << "ִ��ʱ�䣺 " << 1.0 * duration.count() / 1000000 << "�� " << endl;
	return 0;
}
//4.4
//43.98s