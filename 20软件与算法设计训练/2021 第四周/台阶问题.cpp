#include <cstdlib>
#include <iostream>

using namespace std;

size_t 台阶问题递归(size_t n){
    if (n <= 2)
        return n;
    else
        return 台阶问题递归(n-1) + 台阶问题递归(n-2);
}

// O(n)
size_t 台阶问题递推(size_t n){
    size_t 
      n1 = 0,
      n2 = 1;
    // 循环 n 次
    for (size_t x = 0; x < n; x++){
        size_t cache = n1;
        n1 = n2;
        n2 += cache;
    }
    return n2;
}

int main(){
  cout << 台阶问题递推(10) << endl;
  return 0;
}
