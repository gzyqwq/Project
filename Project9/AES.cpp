#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib> 
#include <windows.h>
using namespace std;
#define int long long

int plaintext[100][256][16];
int key[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
int C[16], w[44];
int S_box[16][16];
const int Mat[8][8] = {{1,0,0,0,1,1,1,1},{1,1,0,0,0,1,1,1},{1,1,1,0,0,0,1,1},{1,1,1,1,0,0,0,1},{1,1,1,1,1,0,0,0},{0,1,1,1,1,1,0,0},{0,0,1,1,1,1,1,0},{0,0,0,1,1,1,1,1}} ;
const int c[8] = {1,1,0,0,0,1,1,0};
const int _Mat[4][4] = {{2,3,1,1},{1,2,3,1},{1,1,2,3},{3,1,1,2}};
const int Rcon[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36 };

void SubBytes();
void ShiftRows(); 
void MixColunmns();
void AddRoundkey();

int gcd(int a,int b) {return !b ? a : gcd(b,a%b);}

int x2time(int x) {
	if (x & 0x80) {
		return (((x << 1) ^ 0x1B) & 0xFF);
	}
	return x << 1;
}
int x3time(int x) {return (x2time(x) ^ x);}

int poly_Mul(int a, int b) {//多项式乘法
    int tmp[8] = {0};
    int i;
    for(i = 0;i < 8;i ++) {
        tmp[i] = (a << i) * ((b >> i) & 0x1);
    }
    tmp[0] = tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3] ^ tmp[4] ^ tmp[5] ^ tmp[6] ^ tmp[7];

    return tmp[0];
}
int Find_hbit(int val) {//比特最高位 
    int i = 0;
    while(val) {
        i ++;
        val = val >> 1;
    }
    return i;
}

int Gf_div(int div_ed, int div, int *remainder) {//GF多项式除 
    int r0 = 0; 
    int qn = 0;
    int bitCnt = 0;

    r0 = div_ed;

    bitCnt = Find_hbit(r0) - Find_hbit(div);
    while(bitCnt >= 0) {
        qn = qn | (1 << bitCnt);
        r0 = r0 ^ (div << bitCnt);
        bitCnt = Find_hbit(r0) - Find_hbit(div);
    }
    *remainder = r0;
    return qn;
}

int Inv_S_box(int a, int m) {//找逆元 
    int r0, r1, r2;
    int  qn, v0, v1, v2, w0, w1, w2;
    r0 = m; r1 = a;
    v0 = 1; v1 = 0;
    w0 = 0; w1 = 1;

    while(r1 != 1) {
        qn = Gf_div(r0, r1, &r2);
        v2 = v0 ^ poly_Mul(qn, v1);
        w2 = w0 ^ poly_Mul(qn, w1);
        r0 = r1; r1 = r2;
        v0 = v1; v1 = v2;
        w0 = w1; w1 = w2;
    }
    return w1;
}

int Mul(int x) {//矩阵乘法 
	int b[8],m[8];
	for(int i = 0;i < 8;++ i) b[i] = m[i] = 0;
	for(int i = 0;i < 8;++ i) {
		b[i] = x & 1;
		x /= 2;
	}
	for(int i = 0;i < 8;++ i) {
		for(int j = 0;j < 8;++ j) {
			m[i] += Mat[i][j] * b[j];
		}
		m[i] += c[i];
		m[i] %= 2;
	}
	int ans = 0;
	for(int i = 0;i < 8;++ i) {
		ans += (1 << i) * m[i];
	}
	return ans;
} 
void Pre() {//done 产生key 
	srand(time(0));
	for(int i = 0;i < 100;++ i) {
		for(int j = 0;j < 256;++ j) {
			plaintext[i][j][0] = j;
			for(int k = 1;k < 16;++ k) {
				plaintext[i][j][k] = rand() % 256;
			}
		}
	}
	//Debug();
	return;
}
void S_box_pre() {//模数0x11B 
	int tot = 0;
	for(int i = 0;i < 16;++ i) {
		for(int j = 0;j < 16;++ j) {
			S_box[i][j] = tot ++;
		}
	}
	for(int i = 0;i < 16;++ i) {
		for(int j = 0;j < 16;++ j) {
			if(S_box[i][j]) {
				S_box[i][j] = Inv_S_box(S_box[i][j],0x11B); 
			}
		}
	}
	for(int i = 0;i < 16;++ i) {
		for(int j = 0;j < 16;++ j) {
			S_box[i][j] = Mul(S_box[i][j]);
		} 
	}
	return ;
} 

int Rotword(int x) { //字循环 
	int bin[4] = {0};
	for(int i = 0;i < 4;++ i) {
		bin[i] = x & 255;
		x /= 256;
	}
	int tmp[4] = {0};
	for(int i = 0;i < 4;++ i) {
		tmp[i] = bin[(i + 1) % 4];
	}
	x = 0;
	for(int i = 0;i < 4;++ i) {
		x += tmp[i] << (8 * i);
	}
	return 	x;
}
int Subword(int x) {//字代替
	int bin[4] = {0,0,0,0};
	for(int i = 0;i < 4;++ i) {
		bin[i] = x & 255;
		x >>= 8;
	}
	int _rank,_row;
	for(int i = 0;i < 4;++ i) {
		_rank =  (bin[i] / 16) & 15;
		_row =  bin[i] & 15;
		bin[i] = S_box[_rank][_row];
	}
	x = 0;
	for(int i = 0;i < 4;++ i) {
		x += bin[i] << (8 * i);
	}
	return x;
}
void Key_Round_pre() {
	int tot = 0;
	for(int i = 0;i < 4;++ i) {
		w[i] = 0;
		for(int j = 0;j < 4;++ j) {
			w[i] += key[tot] << (8*j);
			tot ++;
		} 
	}
	for(int i = 4;i < 44;++ i) {
		int tmp = w[i - 1];
		if(i % 4 ==  0) {
			tmp = Subword(Rotword(tmp));
			tmp = tmp ^ Rcon[i / 4 - 1];
		}
		w[i] = w[i - 4] ^ tmp;
	}

}


void SubBytes() {
	int _rank = 0,_row = 0;
	for(int i = 0;i < 16;++ i) {
		_rank =  (C[i] / 16) & 15;
		_row =  C[i] & 15;
		C[i] = S_box[_rank][_row];
	} 
	return;
}
void ShiftRows() {
	int tmp[4] , tmp2[4], tot;
	for(int i = 0;i < 4;++ i) {
		tot = 0;
		for(int j = i;j <= 15;j += 4) {
			tmp[tot ++] = C[j];
		}
		for(int j = 0;j < 4;++ j) {
			tmp2[j] = tmp[(j + i) % 4];
		}
		tot = 0;
		for(int j = i;j <= 15;j += 4) {
			C[j] = tmp2[tot ++];
		//	printf("%d %d %d\n",i,j,C[j]);
		}
	}
	return;
}
void MixColunmns() {
	int tot = 0,tmp[4][4],ans[4][4];
	for(int j = 0;j < 4;++ j) {
		for(int i = 0;i < 4;++ i) {
			tmp[i][j] = C[tot];
			tot ++;
		}
	}//转化成矩阵 
	for(int i = 0;i < 4;++ i) {
		for(int j = 0;j < 4;++ j) {
			ans[i][j] = 0;
			for(int k = 0;k < 4;++ k) {
				if(_Mat[i][k] == 1) ans[i][j] = ans[i][j] ^ tmp[k][j]; 
				if(_Mat[i][k] == 2) ans[i][j] = ans[i][j] ^ x2time(tmp[k][j]);
				if(_Mat[i][k] == 3) ans[i][j] = ans[i][j] ^ x3time(tmp[k][j]);
//				printf("wrong");
			}
		} 
	}
	tot = 0;
	for(int j = 0;j < 4;++ j) {
		for(int i = 0;i < 4;++ i) {
			C[tot] = ans[i][j];
			tot ++;
		}
	}
	return;
}
void AddRoundkey(int round) {
	int tmp[4] = {0};
	int tot = 0;
	for(int i = round * 4 - 4;i < round * 4;++ i) {
		tmp[tot] = w[i];
		tot ++;
	} 
	for(int i = 0;i < 4;++ i) {
		for(int j = 0;j < 4;++ j) {
			key[4 * i + j] = tmp[i] & 255; 
			tmp[i] /= 256; 
		}
	}
	for(int i = 0;i < 16;++ i) C[i] = C[i] ^ key[i];
	return;	
}


void init_round() {
	AddRoundkey(1);
	return;
}
void mid_round(int round) {//9
	SubBytes();
	//	puts("\n");
//cout << "ROUND:" <<round; 
/*	for(int i = 0;i < 16;++ i) {
		if(i % 4 == 0 && i) {
			puts("\n");
		}
		cout << hex << C[i] << ' ';
	}
	puts("\n");
*/	ShiftRows();
	MixColunmns();
	AddRoundkey(round);

	return;
}
void final_round() {
	SubBytes();
	ShiftRows();
	AddRoundkey(11);
	return;
}


void Enc(int group,int round) {
	for(int i = 0;i < 16;++ i) {
		C[i] = rand();
	} 
	init_round();
	for(int i = 2;i <= 10;++ i) mid_round(i);
	final_round();
	return;
}


main() {
	//freopen("1.in.txt","r",stdin);
	Pre();
	S_box_pre();
	Key_Round_pre();
	LARGE_INTEGER t1,t2,tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
	for(int i = 0;i < 100000000;++ i) {
		Enc(i,1);
	}
	QueryPerformanceCounter(&t2);
    double time=(double)(t2.QuadPart-t1.QuadPart)/(double)tc.QuadPart; 
	printf("%fs",time);
	return 0; 
}
/*
0 31323032
1 36343232
2 32373130
3 0




*/
