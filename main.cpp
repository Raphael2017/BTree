#include <iostream>
#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <time.h>

#include "btree.h"


size_t total = 10000000;
/*
 * [L,R)
 * */
size_t binary_search(int* keys_arr, size_t keys_size, int target);

int main() {
    BTree tree;
    Record* record = nullptr;

    std::map<int, Record*> tmp;
    std::vector<int> src;

    for (size_t i = 0; i < total; ++i)
        src.push_back(rand());

    clock_t start, finish;
    double duration;
    {
        start = clock();
        for (auto it : src)
            tmp.insert(std::make_pair(it, nullptr));
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "STD::MAP::INSERT: %f seconds\n", duration );
        start = clock();
        for (auto it : src)
        {
            assert(tmp.find(it) != tmp.end());
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "STD::MAP::FIND: %f seconds\n", duration );
    }
    {
        start = clock();
        for (auto it : src)
        {
            bool t = tree.insert(it, nullptr);
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::INSERT: %f seconds\n", duration );
        start = clock();
        for (auto it : src)
        {
            assert(tree.search(it, record));
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::FIND: %f seconds\n", duration );
    }

    {
        std::vector<int> others;
        for (size_t i = 0; i < 1000000; ++i)
            others.push_back(rand());
        for (auto it : others)
        {
            bool r1 = (tmp.find(it) != tmp.end());
            bool r2 = tree.search(it, record);
            assert(r1 == r2);
        }
    }

    return 0;
}

size_t binary_search(int* keys_arr, size_t keys_size, int target)
{
    assert(keys_arr != nullptr);
    assert(keys_size > 0);
    int L = 0, R = keys_size;
    while (L < R)
    {
        int M = L + ((R-L)>>1);   // avoid (L+R)/2 overflow
        if (keys_arr[M] == target)
            return M+1;
        else if (keys_arr[M] > target)
            R = M;
        else
            L = 1 + M;
    }
    return L;
}

