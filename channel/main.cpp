#include <bits/stdc++.h>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
using namespace std;

using Matrix = vector<vector<int>>;
using Row = vector<int>;
mutex global_mutex;

template<typename T>
class buffered_channel {
public:
    explicit buffered_channel(size_t buffer_size = 0)
        : capacity(buffer_size), closed(false) {}

    
    void send(const T &value) {
        unique_lock<mutex> lk(mtx);
        cv_not_full.wait(lk, [&]{ return q.size() < capacity || closed; });
        if (closed) throw runtime_error("send on closed channel");
        q.push_back(value);
        cv_not_empty.notify_one();
    }

    
    void send(T &&value) {
        unique_lock<mutex> lk(mtx);
        cv_not_full.wait(lk, [&]{ return q.size() < capacity || closed; });
        if (closed) throw runtime_error("send on closed channel");
        q.push_back(std::move(value));
        cv_not_empty.notify_one();
    }

    
    pair<T, bool> recv() {
        unique_lock<mutex> lk(mtx);
        cv_not_empty.wait(lk, [&]{ return !q.empty() || closed; });
        if (!q.empty()) {
            T val = std::move(q.front());
            q.pop_front();
            cv_not_full.notify_one();
            return { std::move(val), true };
        }
        
        return { T(), false };
    }

    
    void close() {
        {
            unique_lock<mutex> lk(mtx);
            closed = true;
        }
        cv_not_empty.notify_all();
        cv_not_full.notify_all();
    }

    bool is_closed() const {
        unique_lock<mutex> lk(mtx);
        return closed;
    }

private:
    size_t capacity;
    mutable mutex mtx;
    condition_variable cv_not_empty;
    condition_variable cv_not_full;
    deque<T> q;
    bool closed;
};

Matrix genereateMatrix(int n){
    int limit = 6;
    static std::mt19937 rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(0, limit-1);
    Matrix matrix(n, Row(n));
    for (int i =0;i<n;i++){
        for (int j =0;j<n;j++){
            matrix[i][j] = dist(rng);
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


struct BlockTask {
    int i_block;
    int j_block;
    int k_block;
    BlockTask(int i=0,int j=0,int k=0):i_block(i),j_block(j),k_block(k){}
};


void multiplicate_block_task(const BlockTask &task, int blockSize, const Matrix &a, const Matrix &b, Matrix &res){
    int n = a.size();
    int i_start = task.i_block * blockSize;
    int j_start = task.j_block * blockSize;
    int k_start = task.k_block * blockSize;
    int i_end = min(i_start + blockSize, n);
    int j_end = min(j_start + blockSize, n);
    int k_end = min(k_start + blockSize, n);

    for (int i = i_start; i < i_end; ++i){
        for (int j = j_start; j < j_end; ++j){
            int sum = 0;
            for (int k = k_start; k < k_end; ++k){
                sum += a[i][k] * b[k][j];
            }
            
            unique_lock<mutex> lk(global_mutex);
            res[i][j] += sum;
        }
    }
}


Matrix multiThreadblockMultiplicationBuffered(Matrix &a, Matrix &b, int blockSize, int &threadsUsed, size_t bufferCapacity = 128){
    int n = a.size();
    Matrix res(n, Row(n, 0));
    int blocks = n / blockSize + (n % blockSize != 0);

    buffered_channel<BlockTask> chan(bufferCapacity);

    unsigned hw = thread::hardware_concurrency();
    if (hw == 0) hw = 4;
    int workers = static_cast<int>(hw);
    threadsUsed = workers;

    auto worker = [&](int){
        while (true) {
            auto pr = chan.recv();
            if (!pr.second) break; 
            multiplicate_block_task(pr.first, blockSize, a, b, res);
        }
    };

    vector<thread> pool;
    pool.reserve(workers);
    for (int w = 0; w < workers; ++w) pool.emplace_back(worker, w);

    for (int i = 0; i < blocks; ++i){
        for (int j = 0; j < blocks; ++j){
            for (int k = 0; k < blocks; ++k){
                chan.send(BlockTask(i, j, k));
            }
        }
    }
    chan.close();

    for (auto &t : pool) if (t.joinable()) t.join();

    return res;
}

void solve(Matrix &a, Matrix &b, Matrix &correctRes, int blockSize){
    cout<<"Block size: "<<blockSize<<'\n';
    int threadsUsed = 0;
    auto start = chrono::high_resolution_clock::now();
    Matrix multiThreadblockMultiplicationRes = multiThreadblockMultiplicationBuffered(a,b,blockSize,threadsUsed,128);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Multi thread block algorithm time: " << duration.count() << " ms" << endl;
    if (correctRes == multiThreadblockMultiplicationRes){
        cout<<"Multi thread block multiplication returned correct matrix\n";
    } else{
        cout<<"Multi thread block multiplication returned incorrect matrix\n";
    }
    cout<<"Number of used threads in multi thread: "<<threadsUsed<<"\n======================\n";
}

int main(){
    freopen("out.txt", "a", stdout);
    int n, blockSize;
    cin>>n>>blockSize;

    cout<<"Matrix size: "<<n<<" * "<<n<<'\n';
    Matrix a = genereateMatrix(n);
    Matrix b = genereateMatrix(n);

    auto start = chrono::high_resolution_clock::now();
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Single thread stupid algorithm time: " << duration.count() << " ms" << endl;

    solve(a,b,correctRes,blockSize);

    return 0;
}
