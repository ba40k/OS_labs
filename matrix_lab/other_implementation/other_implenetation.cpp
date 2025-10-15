#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")


using namespace std;
using Matrix = vector<vector<int>>; 
using Row = vector<int>;
mutex globalMutex;
Matrix genereateMatrix(int n){
    int limit = 6;
    srand(time(0));
    Matrix matrix(n, Row(n));
    for (int i =0;i<n;i++){
        for (int j =0;j<n;j++){
            matrix[i][j] = rand() % limit;
        }
    }
    return matrix;
}
Matrix stupidMatrixMultiplication(Matrix &a, Matrix &b){
    int n = a.size();
    Matrix res(n, Row(n, 0));
    for (int i =0;i<n;i++){
        for (int j = 0;j<n;j++){
            for (int k = 0;k<n;k++){
                res[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return res;
}
void printMatrix(Matrix &a){
    for (auto &row : a){
        for (auto &x : row){
            cout<<x<<' ';
        }
        cout<<'\n';
    }
    cout<<"_____________________\n";
}
void addBlockMultiplication(int i, int j,int blockSize, Matrix &a, Matrix &b, Matrix &res){
    int n = a.size();
    for (int row = i * blockSize;row < min(n, (i+1) * blockSize );row++){
        for (int col = j * blockSize;col < min(n, (j+1)*blockSize);col++){
            for (int k = 0;k<n;k++){
                res[row][col] += a[row][k] * b[k][col];
            }
        }
    }
}
Matrix singleThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize){
    int n = a.size();
    Matrix res(n, Row(n));
    int blocks = n/blockSize + (n%blockSize!=0);
    for (int i = 0;i<blocks;i++){
        for (int j = 0;j<blocks;j++){
            for (int row = i * blockSize;row < min(n, (i+1) * blockSize );row++){
                for (int col = j * blockSize;col < min(n, (j+1)*blockSize);col++){
                    for (int k = 0;k<n;k++){
                        res[row][col] += a[row][k] * b[k][col];
            }
        }
    }
        }
    }
    return res;
}
Matrix multiThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize, int &treadsUsed){
    int n = a.size();
    Matrix res(n, Row(n));
    int blocks = n/blockSize + (n%blockSize!=0);
    vector<thread*> threads(blocks * blocks);
    int curThread = 0;
    for (int i = 0;i<blocks;i++){
        for (int j = 0;j<blocks;j++){
            threads[curThread] = new thread (addBlockMultiplication, i,j,blockSize, std::ref(a),std::ref(b), std::ref(res));
            curThread++;
            treadsUsed++;
            //addBlockMultiplication(i ,j, blockSize, a,b,res);
        }
    }
    for (auto &tp : threads){
        tp->join();
        delete tp;
    }
    return res;
}
int main(){
    freopen("out.txt", "w", stdout);
    int n, blockSize;
    cin>>n>>blockSize;
    cout<<"Matrix size: "<<n<<" * "<<n<<'\n';
    cout<<"Block size: "<<blockSize<<'\n';
    Matrix a = genereateMatrix(n);
    Matrix b = genereateMatrix(n);
    auto start = std::chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Single thread stupid algorithm time: " << duration.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    Matrix singleThreadblockMultiplicationRes = singleThreadblockMultiplication(a, b, blockSize);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Single thread block algorithm time: " << duration.count() << " ms" << std::endl;
    int threadsUsed = 0;
    start = std::chrono::high_resolution_clock::now();
    Matrix multiThreadblockMultiplicationRes = multiThreadblockMultiplication(a,b,blockSize,threadsUsed);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Multi thread block algorithm time: " << duration.count() << " ms" << std::endl;
    if (correctRes == singleThreadblockMultiplicationRes){
        cout<<"Single thread block multiplication returned correct matrix\n";
    } else{
        cout<<"Single thread block multiplication returned incorrect matrix\n";
    }
    if (correctRes == multiThreadblockMultiplicationRes){
        cout<<"Multi thread block multiplication returned correct matrix\n";
    } else{
        cout<<"Multi thread block multiplication returned incorrect matrix\n";
    }
    cout<<"Number of used thread in multi thread: "<<threadsUsed;
}