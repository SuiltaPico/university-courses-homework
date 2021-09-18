def 斐波那契递归(n):
    if n<=2:
        return n
    else:
        return 斐波那契递归(n-1) + 斐波那契递归(n-2)
      
def 斐波那契递推(n):
    n1 = 0
    n2 = 1

    for x in range(0, n):
        cache = n1
        n1 = n2
        n2 += cache
    return n2

def 阶乘递归(n):
    if n==1:
        return 1
    else:
        return n*阶乘递归(n-1)
    
def 阶乘递推(n):
    result = 1
    for x in range(1, n+1):
        result *= x 
    return result
