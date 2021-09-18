function 斐波那契递归(n){
    if (n <= 2)
        return n;
    else
        return 斐波那契递归(n-1) + 斐波那契递归(n-2);
}
      
function 斐波那契递推(n){
    let 
      n1 = 0,
      n2 = 1;

    for (let x = 0; x < n; x++){
        let cache = n1;
        n1 = n2;
        n2 += cache;
    }
    return n2;
}

function 阶乘递归(n){
    if (n == 1)
        return 1;
    else
        return n*阶乘递归(n-1);
}
    
function 阶乘递推(n){
    let result = 1;
    for (let x = 1; x < n-1; x++)
        result *= x;
    return result;
}
