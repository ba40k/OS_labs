#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

using Matrix = vector<vector<int>>;
using Row = vector<int>;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

Matrix generateMatrix(int n) {
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

struct BlockNumber {
    int i, j;
    BlockNumber(int _i, int _j) : i(_i), j(_j) {}
};

struct ThreadArgs {
    BlockNumber a_block;
    BlockNumber b_block;
    int blockSize;
    Matrix *a;
    Matrix *b;
    Matrix *res;
};

void* multiplicate_blocks(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int n = args->a->size();
    for (int i = args->a_block.i * args->blockSize; i < min((args->a_block.i + 1) * args->blockSize, n); i++) {
        for (int j = args->b_block.j * args->blockSize; j < min((args->b_block.j + 1) * args->blockSize, n); j++) {
            int sum = 0;
            for (int k = args->a_block.j * args->blockSize; k < min((args->a_block.j + 1) * args->blockSize, n); k++) {
                sum += (*args->a)[i][k] * (*args->b)[k][j];
            }
            pthread_mutex_lock(&global_mutex);
            (*args->res)[i][j] += sum;
            pthread_mutex_unlock(&global_mutex);
        }
    }
    delete args;
    return nullptr;
}

Matrix multiThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize, int &threadsUsed) {
    int n = a.size();
    Matrix res(n, Row(n, 0));
    int blocks = (n + blockSize - 1) / blockSize;
    vector<pthread_t> threads;

    for (int i = 0; i < blocks; i++) {
        for (int j = 0; j < blocks; j++) {
            for (int k = 0; k < blocks; k++) {
                ThreadArgs* args = new ThreadArgs{
                    BlockNumber(i, k),
                    BlockNumber(k, j),
                    blockSize,
                    &a,
                    &b,
                    &res
                };
                pthread_t tid;
                pthread_create(&tid, nullptr, multiplicate_blocks, args);
                threads.push_back(tid);
                threadsUsed++;
            }
        }
    }

    for (auto &tid : threads) {
        pthread_join(tid, nullptr);
    }

    return res;
}

void solve(Matrix &a, Matrix &b, Matrix &correctRes, int blockSize) {
    cout << "Block size: " << blockSize << '\n';
    int threadsUsed = 0;
    auto start = chrono::high_resolution_clock::now();
    Matrix multiThreadblockMultiplicationRes = multiThreadblockMultiplication(a, b, blockSize, threadsUsed);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Multi thread block algorithm time: " << duration.count() << " ms" << endl;
    if (correctRes == multiThreadblockMultiplicationRes) {
        cout << "Multi thread block multiplication returned correct matrix\n";
    } else {
        cout << "Multi thread block multiplication returned incorrect matrix\n";
    }
    cout << "Number of used threads: " << threadsUsed << "\n======================\n";
}

int main() {
    freopen("out.txt", "a", stdout);
    int n, blockSize;
    cin >> n >> blockSize;
    cout << "Matrix size: " << n << " * " << n << '\n';
    Matrix a = generateMatrix(n);
    Matrix b = generateMatrix(n);
    auto start = std::chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Single thread stupid algorithm time: " << duration.count() << " ms" << std::endl;
    solve(a, b, correctRes, blockSize);
    
}
