#include <bits/stdc++.h>
#include <pthread.h>
#include <chrono>

#pragma GCC target("avx2")
#pragma GCC optimize("O3")

using namespace std;
using Matrix = vector<vector<int>>;
using Row = vector<int>;

struct ThreadArgs {
    int i, j, blockSize;
    Matrix *a, *b, *res;
};

Matrix genereateMatrix(int n) {
    int limit = 6;
    srand(time(0));
    Matrix matrix(n, Row(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = rand() % limit;
    return matrix;
}

Matrix stupidMatrixMultiplication(Matrix &a, Matrix &b) {
    int n = a.size();
    Matrix res(n, Row(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                res[i][j] += a[i][k] * b[k][j];
    return res;
}

void printMatrix(Matrix &a) {
    for (auto &row : a) {
        for (auto &x : row)
            cout << x << ' ';
        cout << '\n';
    }
    cout << "_____________________\n";
}

void* addBlockMultiplication(void* arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    int i = args->i, j = args->j, blockSize = args->blockSize;
    Matrix &a = *(args->a), &b = *(args->b), &res = *(args->res);
    int n = a.size();
    for (int row = i * blockSize; row < min(n, (i + 1) * blockSize); row++)
        for (int col = j * blockSize; col < min(n, (j + 1) * blockSize); col++)
            for (int k = 0; k < n; k++)
                res[row][col] += a[row][k] * b[k][col];
    return nullptr;
}

Matrix singleThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize) {
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

Matrix multiThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize, int &threadsUsed) {
    int n = a.size();
    Matrix res(n, Row(n));
    int blocks = n / blockSize + (n % blockSize != 0);
    int totalThreads = blocks * blocks;
    vector<pthread_t> threads(totalThreads);
    vector<ThreadArgs> args(totalThreads);
    int curThread = 0;

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            args[curThread] = {i, j, blockSize, &a, &b, &res};
            pthread_create(&threads[curThread], nullptr, addBlockMultiplication, &args[curThread]);
            curThread++;
            threadsUsed++;
        }
    }

    for (int i = 0; i < totalThreads; i++)
        pthread_join(threads[i], nullptr);

    return res;
}

int main() {
    freopen("out.txt", "w", stdout);
    int n, blockSize;
    cin >> n >> blockSize;
    cout << "Matrix size: " << n << " * " << n << '\n';
    cout << "Block size: " << blockSize << '\n';

    Matrix a = genereateMatrix(n);
    Matrix b = genereateMatrix(n);

    auto start = chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Single thread stupid algorithm time: " << duration.count() << " ms\n";

    start = chrono::high_resolution_clock::now();
    Matrix singleThreadblockMultiplicationRes = singleThreadblockMultiplication(a, b, blockSize);
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Single thread block algorithm time: " << duration.count() << " ms\n";

    int threadsUsed = 0;
    start = chrono::high_resolution_clock::now();
    Matrix multiThreadblockMultiplicationRes = multiThreadblockMultiplication(a, b, blockSize, threadsUsed);
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Multi thread block algorithm time: " << duration.count() << " ms\n";

    if (correctRes == singleThreadblockMultiplicationRes)
        cout << "Single thread block multiplication returned correct matrix\n";
    else
        cout << "Single thread block multiplication returned incorrect matrix\n";

    if (correctRes == multiThreadblockMultiplicationRes)
        cout << "Multi thread block multiplication returned correct matrix\n";
    else
        cout << "Multi thread block multiplication returned incorrect matrix\n";

    cout << "Number of used threads in multi thread: " << threadsUsed << '\n';
}
