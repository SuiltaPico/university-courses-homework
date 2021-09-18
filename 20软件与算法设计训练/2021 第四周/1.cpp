#include <stdlib.h>

size_t 斐波那契递归(size_t n){
    if (n <= 2)
        return n;
    else
        return 斐波那契递归(n-1) + 斐波那契递归(n-2);
}
      
size_t 斐波那契递推(size_t n){
    size_t 
      n1 = 0,
      n2 = 1;

    for (size_t x = 0; x < n; x++){
        size_t cache = n1;
        n1 = n2;
        n2 += cache;
    }
    return n2;
}

size_t 阶乘递归(size_t n){
    if (n == 1)
        return 1;
    else
        return n*阶乘递归(n-1);
}
    
size_t 阶乘递推(size_t n){
    size_t result = 1;
    for (size_t x = 1; x < n-1; x++)
        result *= x;
    return result;
}
