#include <bits/stdc++.h>
using namespace std;
using Matrix = vector<vector<int>>; 
using Row = vector<int>;
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
}
Matrix parallelMultiplication(Matrix &a, Matrix &b){
    
}
int main(){
    freopen("out.txt", "w", stdout);
    int n;
    cin>>n;
    Matrix a = genereateMatrix(n);
    Matrix b = genereateMatrix(n);
    Matrix correctRes = stupidMatrixMultiplication(a, b);
    

}