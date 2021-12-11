# 查找IP地址所属的区域
## 代码展示
[在线编译](https://gcc.godbolt.org/z/K487vbdz8)
### `main.cpp`
```cpp
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

struct IPLocation
{
    uint32_t start;
    uint32_t end;
    string location;
    IPLocation(const string &, const string &, const string &);
};

uint32_t ipv4_string_to_value(string const &ipString);
string find_IP_location(const string &, const IPLocation *&, size_t &&);
void sort_IPLocations(IPLocation *, const size_t &&);
bool ascending_sort_by_ip_end(IPLocation &a, IPLocation &b);
int binary_search_first_larger_equal(const uint32_t target, IPLocation *IP_locations, const size_t IP_locations_size);

IPLocation::IPLocation(const string &ip_start, const string &ip_end, const string &location)
{
    this->start = ipv4_string_to_value(ip_start);
    this->end = ipv4_string_to_value(ip_end);
    this->location = location;
}

// IP 地址字符串转 unsigned int32
uint32_t ipv4_string_to_value(string const &ipString)
{
    uint32_t result = 0;
    for (size_t next_dot_pos = string::npos, last_pos = 0; last_pos != string::npos; last_pos = next_dot_pos)
    {
        if (next_dot_pos != string::npos)
            last_pos += 1;
        next_dot_pos = ipString.find(".", last_pos);
        uint32_t segmentValue = atoi(ipString.substr(last_pos, next_dot_pos - last_pos).c_str());
        result = (result << 8) + segmentValue;
    }
    return result;
}

void sort_IPLocations(IPLocation *IP_locations, const size_t &&IP_locations_size)
{
    sort(IP_locations, IP_locations + IP_locations_size, ascending_sort_by_ip_end);
}

bool ascending_sort_by_ip_end(IPLocation &a, IPLocation &b)
{
    return a.end < b.end;
}

// IPLocation 的二分查找
int binary_search_first_larger_equal(const uint32_t target, IPLocation *IP_locations, const size_t IP_locations_size)
{
    size_t left = 0, right = IP_locations_size - 1;
    while (left < right)
    {
        size_t mid = (left + right) / 2;
        if (target > IP_locations[mid].end)
            left = mid + 1;
        else if (mid != 0 && target < IP_locations[mid - 1].end)
            right = mid - 1;
        else if (target >= IP_locations[mid].start)
            return mid;
        else
            break;
    }

    if (target >= IP_locations[left].start && target <= IP_locations[left].end)
        return left;
    return -1;
}

void find_IP_location(const string &ip, IPLocation *IP_locations, const size_t &&IP_locations_size)
{
    int ip_pos = binary_search_first_larger_equal(ipv4_string_to_value(ip), IP_locations, IP_locations_size);

    if (ip_pos != -1)
        cout << ip << " is in " << IP_locations[ip_pos].location << endl;
    else
        cout << "Fail to find location of " << ip << endl;
}

int main()
{
    IPLocation IP_locations[] = {
        IPLocation("202.102.133.0", "202.102.133.255", "Shandong Dongying"),
        IPLocation("202.102.135.0", "202.102.136.255", "Shandong Yantai"),
        IPLocation("202.102.156.34", "202.102.157.255", "Shandong Qingdao"),
        IPLocation("202.102.48.0", "202.102.48.255", "Jiangsu Suqian"),
        IPLocation("202.102.49.15", "202.102.51.251", "Jiangsu Taizhou"),
        IPLocation("202.102.56.0", "202.102.56.255", "Jiangsu Lianyungang"),
    };

    // 二分查找之前需要先排序
    sort_IPLocations(IP_locations, sizeof(IP_locations) / sizeof(IPLocation));

    string input_ip = "";

    while (cin >> input_ip && input_ip != "exit")
    {
        find_IP_location(input_ip, IP_locations, sizeof(IP_locations) / sizeof(IPLocation));
    }

    return 0;
}
```

## 运行结果
```cmd
> d:\classTask\算法训练\build\查找IP地址所属的区域.exe
202.102.133.254
202.102.133.254 is in Shandong Dongying
202.102.156.32 
Fail to find location of 202.102.156.32
202.102.156.38
202.102.156.38 is in Shandong Qingdao
202.102.56.0  
202.102.56.0 is in Jiangsu Lianyungang
202.102.48.255
202.102.48.255 is in Jiangsu Suqian
```