#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;
using Matrix = vector<vector<int>>;
using Row = vector<int>;

CRITICAL_SECTION cs; 

struct ThreadData {
    int blockSize;
    Matrix* a;
    Matrix* b;
    Matrix* res;
    int n;
    int ai, aj, bi, bj;
};

Matrix generateMatrix(int n) {
    int limit = 6;
    srand(static_cast<unsigned>(time(0)));
    Matrix matrix(n, Row(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = rand() % limit;
    return matrix;
}

Matrix stupidMatrixMultiplication(Matrix& a, Matrix& b) {
    int n = a.size();
    Matrix res(n, Row(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                res[i][j] += a[i][k] * b[k][j];
    return res;
}

DWORD WINAPI multiplicate_blocks(LPVOID param) {
    ThreadData* data = static_cast<ThreadData*>(param);
    int n = data->n;
    int blockSize = data->blockSize;
    Matrix& a = *(data->a);
    Matrix& b = *(data->b);
    Matrix& res = *(data->res);

    for (int i = data->ai * blockSize; i < min((data->ai + 1) * blockSize, n); i++) {
        for (int j = data->bj * blockSize; j < min((data->bj + 1) * blockSize, n); j++) {
            int sum = 0;
            for (int k = data->aj * blockSize; k < min((data->aj + 1) * blockSize, n); k++) {
                sum += a[i][k] * b[k][j];
            }
            EnterCriticalSection(&cs);
            res[i][j] += sum;
            LeaveCriticalSection(&cs);
        }
    }

    delete data;
    return 0;
}

Matrix multiThreadblockMultiplication(Matrix& a, Matrix& b, int blockSize, int& threadsUsed) {
    int n = a.size();
    Matrix res(n, Row(n, 0));
    int blocks = n / blockSize + (n % blockSize != 0);
    vector<HANDLE> handles;

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            for (int k = 0; k < blocks; k++) {
                ThreadData* data = new ThreadData{ blockSize, &a, &b, &res, n, i, k, k, j };
                HANDLE hThread = CreateThread(nullptr, 0, multiplicate_blocks, data, 0, nullptr);
                if (hThread) {
                    handles.push_back(hThread);
                    threadsUsed++;
                }
            }
        }
    }

    WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), TRUE, INFINITE);
    for (HANDLE h : handles) {
        CloseHandle(h);
    }

    return res;
}

void solve(Matrix& a, Matrix& b, Matrix& correctRes, int blockSize) {
    cout << "Block size: " << blockSize << '\n';
    int threadsUsed = 0;
    auto start = chrono::high_resolution_clock::now();
    Matrix result = multiThreadblockMultiplication(a, b, blockSize, threadsUsed);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Multi thread block algorithm time: " << duration.count() << " ms" << endl;

    if (result == correctRes)
        cout << "Multi thread block multiplication returned correct matrix\n";
    else
        cout << "Multi thread block multiplication returned incorrect matrix\n";

    cout << "Number of used threads in multi thread: " << threadsUsed << "\n======================\n";
}

int main() {
    InitializeCriticalSection(&cs); 

    ofstream out("out.txt", ios::app);
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    int n, blockSize;
    cin >> n >> blockSize;

    cout << "Matrix size: " << n << " * " << n << '\n';
    Matrix a = generateMatrix(n);
    Matrix b = generateMatrix(n);

    auto start = chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Single thread stupid algorithm time: " << duration.count() << " ms" << endl;

    solve(a, b, correctRes, blockSize);

    cout.rdbuf(coutbuf);
    DeleteCriticalSection(&cs); 
}
