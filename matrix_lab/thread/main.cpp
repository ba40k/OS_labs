#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
using namespace std;
using Matrix = vector<vector<int>>; 
using Row = vector<int>;
mutex global_mutex;
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
struct BlockNumber{
    int i,j;
    BlockNumber(int _i, int _j){
        this->i = _i;
        this->j = _j;
    }
};
void multiplicate_blocks(BlockNumber a_block, BlockNumber b_block,int blockSize, Matrix &a, Matrix &b, Matrix &res){
    int n = a.size();
    for (int i = a_block.i * blockSize;i < min(a_block.i*blockSize + blockSize, n);i++){
        for (int j = b_block.j * blockSize;j<min(b_block.j * blockSize + blockSize, n);j++){
            int sum = 0;
            for (int k = a_block.j*blockSize;k<min(n,a_block.j*blockSize + blockSize);k++){
               
                sum += a[i][k] * b[k][j];
               
            }
            global_mutex.lock();
            res[i][j] += sum;
            global_mutex.unlock();
        }
    }
    
}

Matrix multiThreadblockMultiplication(Matrix &a, Matrix &b, int blockSize, int &threadsUsed){
    int n = a.size();
    Matrix res(n, Row(n, 0));
    int blocks = n/blockSize + (n%blockSize!=0);
    vector<thread*> threads(blocks * blocks * blocks);
    for (int i = 0;i<blocks;i++){
        for (int j = 0;j<blocks;j++){
            for (int k = 0;k<blocks;k++){
                BlockNumber a_pos(i,k);
                BlockNumber b_pos(k,j);
                threads[threadsUsed] = new thread(multiplicate_blocks,(a_pos), (b_pos),blockSize, ref(a), ref(b), ref(res));
                threadsUsed++;
            }
        }
    }
    for (auto &t : threads){
        t->join();
        delete t;
    }
    return res;
}
void solve(Matrix &a, Matrix &b,Matrix &correctRes, int blockSize){
     cout<<"Block size: "<<blockSize<<'\n';
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Single thread stupid algorithm time: " << duration.count() << " ms" << std::endl;
    int threadsUsed = 0;
    start = std::chrono::high_resolution_clock::now();
    Matrix multiThreadblockMultiplicationRes = multiThreadblockMultiplication(a,b,blockSize,threadsUsed);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Multi thread block algorithm time: " << duration.count() << " ms" << std::endl;
    if (correctRes == multiThreadblockMultiplicationRes){
        cout<<"Multi thread block multiplication returned correct matrix\n";
    } else{
        cout<<"Multi thread block multiplication returned incorrect matrix\n";
    }
    cout<<"Number of used thread in multi thread: "<<threadsUsed<<"\n======================\n";
}
int main(){
    freopen("out.txt", "a", stdout);
    int n, blockSize;
    cin>>n>>blockSize;

    cout<<"Matrix size: "<<n<<" * "<<n<<'\n';
    Matrix a = genereateMatrix(n);
    Matrix b = genereateMatrix(n);
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    solve(a,b,correctRes,blockSize);
    
}