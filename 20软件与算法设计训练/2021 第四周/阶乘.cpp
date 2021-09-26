#include <cstdlib>
#include <iostream>

size_t 阶乘递归(size_t n){
    if (n == 1)
        return 1;
    else
        return n*阶乘递归(n-1);
}
    
// O(n)
size_t 阶乘递推(size_t n){
    size_t result = 1;
    for (size_t x = n; x > 0; x++)
        result *= x;
    return result;
}

int main(){
  cout << 阶乘递推(10) << endl;
  return 0;
}
