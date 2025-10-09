#include <vector>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <fstream>
using namespace std;

using Matrix = vector<vector<int>>;
using Row = vector<int>;

CRITICAL_SECTION cs;

Matrix generateMatrix(int n) {
    int limit = 6;
    srand(time(0));
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

void printMatrix(Matrix& a) {
    for (auto& row : a) {
        for (auto& x : row)
            cout << x << ' ';
        cout << '\n';
    }
    cout << "_____________________\n";
}

struct BlockArgs {
    int i, j, blockSize;
    Matrix* a;
    Matrix* b;
    Matrix* res;
};

DWORD WINAPI addBlockMultiplication(LPVOID param) {
    BlockArgs* args = (BlockArgs*)param;
    int n = args->a->size();
    for (int row = args->i * args->blockSize; row < min(n, (args->i + 1) * args->blockSize); row++) {
        for (int col = args->j * args->blockSize; col < min(n, (args->j + 1) * args->blockSize); col++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += (*args->a)[row][k] * (*args->b)[k][col];
            }
            (*args->res)[row][col] += sum;
        }
    }
    delete args;
    return 0;
}

Matrix singleThreadblockMultiplication(Matrix& a, Matrix& b, int blockSize) {
    int n = a.size();
    Matrix res(n, Row(n));
    int blocks = n / blockSize + (n % blockSize != 0);
    for (int i = 0; i < blocks; i++)
        for (int j = 0; j < blocks; j++)
            for (int row = i * blockSize; row < min(n, (i + 1) * blockSize); row++)
                for (int col = j * blockSize; col < min(n, (j + 1) * blockSize); col++)
                    for (int k = 0; k < n; k++)
                        res[row][col] += a[row][k] * b[k][col];
    return res;
}

Matrix multiThreadblockMultiplication(Matrix& a, Matrix& b, int blockSize, int& threadsUsed) {
    int n = a.size();
    Matrix res(n, Row(n));
    int blocks = n / blockSize + (n % blockSize != 0);
    vector<HANDLE> handles;
    InitializeCriticalSection(&cs);

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            BlockArgs* args = new BlockArgs{ i, j, blockSize, &a, &b, &res };
            HANDLE hThread = CreateThread(nullptr, 0, addBlockMultiplication, args, 0, nullptr);
            if (hThread) {
                handles.push_back(hThread);
                threadsUsed++;
            }
        }
    }

    WaitForMultipleObjects(handles.size(), handles.data(), TRUE, INFINITE);
    for (HANDLE h : handles) CloseHandle(h);
    DeleteCriticalSection(&cs);
    return res;
}

int main() {
    fstream out("output.txt");
    int n, blockSize;
    cin >> n >> blockSize;
    out << "Matrix size: " << n << " * " << n << '\n';
    out << "Block size: " << blockSize << '\n';

    Matrix a = generateMatrix(n);
    Matrix b = generateMatrix(n);

    auto start = chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    out << "Single thread stupid algorithm time: " << duration.count() << " ms\n";

    start = chrono::high_resolution_clock::now();
    Matrix singleThreadRes = singleThreadblockMultiplication(a, b, blockSize);
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    out << "Single thread block algorithm time: " << duration.count() << " ms\n";

    int threadsUsed = 0;
    start = chrono::high_resolution_clock::now();
    Matrix multiThreadRes = multiThreadblockMultiplication(a, b, blockSize, threadsUsed);
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    out << "Multi thread block algorithm time: " << duration.count() << " ms\n";

    out << (correctRes == singleThreadRes ? "Single thread block multiplication returned correct matrix\n"
        : "Single thread block multiplication returned incorrect matrix\n");
    out << (correctRes == multiThreadRes ? "Multi thread block multiplication returned correct matrix\n"
        : "Multi thread block multiplication returned incorrect matrix\n");
    out << "Number of used threads in multi thread: " << threadsUsed << '\n';
    out.close();
}
