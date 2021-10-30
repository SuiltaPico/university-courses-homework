```cpp
#include <cstdio>
#include <ctime>
#include <cstdlib>

// 构建最小堆的函数，由构建最大堆的函数简单推理出来的
bool buildMinimumHeap(int *arr, size_t arr_size)
{
    if (arr_size < 2)
        return false;
    for (int i = arr_size / 2 + 1; i >= 0; i--)
    {
        size_t current = i;
        while (true)
        {
            size_t min = 2 * (current + 1) - 1;
            if (min >= arr_size)
                break;
            if (min + 1 < arr_size)
                min = arr[min] <= arr[min + 1] ? min : min + 1;
            if (arr[min] < arr[current])
            {
                int temp = arr[current];
                arr[current] = arr[min];
                arr[min] = temp;
            }
            current = min;
        }
    }
    return true;
}

struct IntArray
{
    size_t size;
    int *arr;
};

// 维护一个最小堆，大于堆顶的放入，小于的取走。每次放入则丢弃最小堆根节点。
IntArray getTopK(int *arr, size_t arr_size, size_t k = 2)
{
    int *minimum_heap{}, *result{};
    size_t minimum_heap_size{}, result_size{};
    minimum_heap = new int[k]{};

    // 放入 min(arr_size,k) 个元素到堆内并构建堆
    for (size_t i = 0; i < k && i < arr_size; i++)
        minimum_heap[minimum_heap_size++] = arr[i];

    buildMinimumHeap(minimum_heap, minimum_heap_size);

    // 当 arr_size <= k 那就返回这个长度的数组
    if (arr_size <= k)
        result_size = minimum_heap_size;
    // 否则限制最大堆大小为 k
    else
    {
        result_size = k;
        // 从 k 开始，到 arr_size，如果 arr[i] 大于根节点则把根节点换成 arr[i]，然后再构建一次。
        for (size_t i = k; i < arr_size; i++)
            if (arr[i] > minimum_heap[0])
            {
                minimum_heap[0] = arr[i];
                buildMinimumHeap(minimum_heap, minimum_heap_size);
            }
    }

    result = new int[result_size]{};

    // 像堆排序一样把堆转换为数组
    for (size_t i = 0; i < result_size; i++)
    {
        result[result_size - i - 1] = minimum_heap[0];
        minimum_heap[0] = minimum_heap[(minimum_heap_size--) - 1];
        buildMinimumHeap(minimum_heap, minimum_heap_size);
    }

    delete[] minimum_heap;
    return {result_size, result};
}

int main()
{

    // 随机生成数组
    srand((unsigned)time(NULL));

    const size_t size = rand() % 6 + 5;
    const size_t k = size - 4;
    int *arr = new int[size];

    for (size_t i = 0; i < size; i++)
        arr[i] = rand() % 10;

    // 输出数组
    printf("array: ");
    for (size_t i = 0; i < size; i++)
        printf("%4d ", arr[i]);
    printf("\n");

    IntArray result = getTopK(arr, size, k);

    // 输出前k位数
    printf("Top%zu: ", result.size);
    for (size_t i = 0; i < result.size; i++)
        printf("%4d ", result.arr[i]);
    printf("\n");

    delete[] result.arr;
    delete[] arr;
    return 0;
}
```
