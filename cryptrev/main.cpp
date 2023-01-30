#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <thread>
#include <utility>
#include <mutex>
#include <chrono>
#include <cstdlib>

/*
 * Тестовый вектор: size 64
 * вход:  {0x84218421, 0x48124812, 0x24812481, 0x12481248}
 * выход: {0x12d630a1, 0x13bf02fb, 0x0568724a, 0x04014010}
 *
 * Тестовый вектор: size 16
 * вход:  0x84218421
 * выход: 0x40100401
*/

using namespace std;

#define SIZE_A 16

typedef unsigned int UINT;

UINT* a;
UINT* b;

int MAGIC_NUMBERS = 130;

std::mutex mtx;

inline UINT rez(UINT mz) {

    int size = SIZE_A;
    unsigned int A1,A2,A3;
    UINT bvar[SIZE_A] = {0};
    UINT aa[SIZE_A];

    aa[0] = mz;

    // size - битовый размер вектора 256 bit max
    //        входной массив uint равен size/16, 16 uint max
    //        так что битовый размер массива вдвое больше, чем size
    //        и содержит size/16 типов uint 32 бита

    // Внешний цикл по входной битовой длине, 16-32 раза по каждому uint

    int offs = size / 32;

    for (int i = 0; i < size; i++) {

        // i( 0 -  31) -> A1[0] (size/32)
        // i(32 -  63) -> A1[1]
        // i(64 -  95) -> A1[2]
        // i(96 - 127) -> A1[3]
        //A1 = a[i / 32] >> (i % 32);
        A1 = aa[i >> 5] >> (i & 0x1f);
        // Внутренний цикл по входной битовой длине

        for (int j = 0; j < size; j++) {

            // ? - buffer aliasing.
            //
            // offs---+--+-----+
            //        |  |     |
            //        v  v     v
            //  0-31 [0]    ?
            // 32-47    [1][x] ?
            // 48-63    [1][2][x] ?
            // 64-79       [2][3][x]
            //
            //                       16-31 32-63 64-95 96-127   <- size
            // j( 0 -  31) -> A2[0  +  0]                       A1[      0]; A2[      0]
            // j(32 -  63) -> A2[1  +        1]                 A1[    0,1]; A2[    1,2]
            // j(64 -  95) -> A2[2  +              2]           A1[  0,1,2]; A2[  2,3,4]
            // j(96 - 127) -> A2[3  +                    3]     A1[0,1,2,3]; A2[3,4,5,6]
            //
            //
            //
            //
            //
            // j >> 5 + offs -> A2[256]

            A2 = aa[j >> 5 + offs] >> (j & 0x1f);

            // i=j=0, i=j=16 -> A3 = 0
            //
            // j(0-255) 15x(0...31)

            // + i 15 16 17
            // j
            // 15  30 31 32
            // 16  31 32 33
            // 17  32 33 34
            //
            // (i + j) % 32 -> A3[256][256]
            // (i + j) / 32 -> ij[512]
            //
            //A3 = (i + j) & 0x1f;
            A3 = (i + j) & 0x1f;

            bvar[(i + j) >> 5] ^= ( A1 & A2 & 1 ) << A3;   // Magic centaurian operation
       }
    }

    return bvar[0];

}

void f1(UINT mm, UINT mend, int thread_num, int nums) {

    int size = SIZE_A;
    unsigned int c[nums];
    int head = 0;
    UINT total = 0;
    UINT bvar;

    for (; mm < mend; mm++) {
        bvar = rez(mm);
        if (bvar == 0x40100401) {
            c[head] = mm;
            head++;
        }
        total++;
        if (head == nums) { break; }
    }

    mtx.lock();

    cout << dec << "Thread: " << thread_num << ", nums: " << nums << ", Numbers checked: "  << total << endl;
/*
    for (int i = 0; i < head; i++) {

        cout << setfill('0') << setw(8) << hex << c[i] << " ";
        if ((i + 1) % 12 == 0) { cout << endl; }

    }
    cout << dec << endl << endl;
*/
    mtx.unlock();

    return;
}

int main(int argc, char* argv[])
{

    if (argc > 1) {
        MAGIC_NUMBERS = atoi(argv[1]);
    }

    static int THREADS_NUM = thread::hardware_concurrency();
    THREADS_NUM = THREADS_NUM < MAGIC_NUMBERS ? THREADS_NUM : MAGIC_NUMBERS;

    int size = SIZE_A;

    unsigned int step = 0xFFFFFFFF / THREADS_NUM;
    unsigned int start = 0x0;
    int nm = 1;

    cout << dec << "Calculating " << MAGIC_NUMBERS << " magic numbers in " << THREADS_NUM << " thread(s)" << endl << endl;

    std::thread mt[THREADS_NUM];

    auto st = std::chrono::steady_clock::now();

    int totnums = MAGIC_NUMBERS, nums;

    for (int ii = 0; ii < THREADS_NUM; ii++) {
        nums = (int)(totnums / (THREADS_NUM - ii));
        totnums -= nums;
        mt[ii] = std::thread(f1, start, start+step, nm, nums);
        start += step;
        nm++;
    }

    for (int ii = 0; ii < THREADS_NUM; ii++) {
        mt[ii].join();
    }

    auto tend = std::chrono::steady_clock::now();
    std::chrono::duration<double> el = tend - st;
    cout << "time: " << el.count() << "s\n";

    return 0;

}

