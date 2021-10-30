# 在O(n)的时间复杂度下求得无序数组经排序后的第K位数据
这里使用了快速选择算法。
## 代码展示
```cpp
#include <cstdio>
#include <ctime>
#include <cstdlib>

/* 快速选择的原理
 * 随机选择一个位置的值 x 作为中心值，把小于与大于它的分成左右两块：
 * |小于x|中心值|大于x|
 * 我们都知道，第 Top K 个在已排序数组的第k位上：
 * |有序且小于第k个|第k个|有序且大于第k个|
 * 上面的步骤完成后，其实中心值已经在已排序数组的对应位置上了，只不过中心值左右还是乱序的而已：
 * |无序且小于x|中心值|无序且大于x| （现在的）
 * |有序且小于x|中心值|有序且大于x| （通过排序得到的）
 * 那问题就变得简单起来了，我们可以判断 x 是否位于 k 的位置上。如果不是，那 k 必定在其左右。
 * 如果 k小于/大于x的位置，就继续对左边/右边那块进行一开始的步骤，直到 k等于x的位置。
 */
int getNumberOfTopK(int *arr, size_t arr_size, size_t k)
{
    if (arr_size < 2)
        return arr[0];

    srand(time(nullptr));

    // 随机生成中心点
    size_t pivot = rand() % arr_size;
    int pivot_value = arr[pivot];

    // 左中右分块
    arr[pivot] = arr[0];

    size_t
        left_check_point = 1,
        mid{},
        mid_left{},
        right_check_point = arr_size - 1;

    while (left_check_point < right_check_point)
    {
        while (arr[right_check_point] > pivot_value && left_check_point != right_check_point)
        {
            right_check_point--;
            if (left_check_point == right_check_point)
            {
                arr[0] = pivot_value;
                mid = 0;
                goto end_of_loop;
            }
        }
        while (arr[left_check_point] <= pivot_value && left_check_point != right_check_point)
        {
            left_check_point++;
            if (left_check_point == right_check_point)
            {
                arr[0] = arr[left_check_point];
                arr[left_check_point] = pivot_value;
                mid = left_check_point;
                goto end_of_loop;
            }
        }

        int right_temp = arr[right_check_point];
        arr[right_check_point] = arr[left_check_point];
        arr[left_check_point] = right_temp;
    }

end_of_loop:

    // 针对与中间点相同的数进行优化
    mid_left = mid;

    // 构建中间块
    for (long long i = mid_left - 1; i >= 0; i--)
    {
        if (arr[i] == pivot_value)
        {
            int temp = arr[mid_left - 1];
            arr[mid_left - 1] = arr[i];
            arr[i] = temp;
            mid_left--;
        }
    }
    if (k < mid_left)
        return getNumberOfTopK(arr, mid_left, k);
    else if (k > mid)
        return getNumberOfTopK(arr + mid + 1, arr_size - mid - 1, k - mid - 1);
    else
        return pivot_value;

    return 0;
}

int main()
{

    // 随机生成数组
    srand((unsigned)time(NULL));

    const size_t size = rand() % 4 + 5;
    int *arr = new int[size];

    for (size_t i = 0; i < size; i++)
        arr[i] = rand() % 10;

    printf("array: ");
    for (size_t i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // 输出第三位
    printf("Top-3: %d", getNumberOfTopK(arr, size, 2));

    delete[] arr;
    return 0;
}
```
## 运行结果
```cmd
> d:\classTask\算法训练\堆\build\快速选择
array: 1 7 1 2 3 
Top-3: 2
```
