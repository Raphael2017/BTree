#include <iostream>
#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <time.h>

#include "btree.h"


struct Record
{
    int data_;
};

size_t total = 10000000;

void test1(const std::map<int, Record*>& tmp, storage::BTree* tree)
{
    assert(tree->check());
    storage::Record * r = nullptr;
    for (auto it : tmp)
    {
        assert(tree->find(it.first, r));
    }
}

int main() {
    storage::BTree tree;
    storage::Record* record = nullptr;

    std::map<int, Record*> tmp;
    std::vector<int> src;

    for (size_t i = 0; i < total; ++i)
        src.push_back(rand());

    clock_t start, finish;
    double duration;
    {
        start = clock();
        for (auto it : src)
        {
            auto r = new Record{it};
            auto ret = tmp.insert(std::make_pair(it, r));
            if (!ret.second)
            {
                delete(r);
            }
        }

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
            auto r = new Record{it};
            bool t = tree.insert(it, r);
            if (!t)
            {
                delete(r);
            }

        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::INSERT: %f seconds\n", duration );
        start = clock();
        for (auto it : src)
        {
            assert(tree.find(it, record));
            assert(((Record*)record)->data_ == it);
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
            bool r2 = tree.find(it, record);
            assert(r1 == r2);
        }
    }
    auto a = tmp.size();
    auto b = tree.size();
    assert(tmp.size() == tree.size());
    assert(tree.check());

    {
        start = clock();
        for (auto it : src)
        {
            auto kk = tmp.find(it);
            if (kk != tmp.end())
                delete(kk->second);
            tmp.erase(it);
            assert(tmp.find(it) == tmp.end());
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "STD::MAP::ERASE: %f seconds\n", duration );
    }
    {
        start = clock();
        for (auto it : src)
        {
            record = nullptr;
            bool t = tree.erase(it, record);
            if (t)
            {
                assert(((Record*)record)->data_ == it);
                delete(((Record*)record));
            }
            else
            {
                assert(record == nullptr);
            }
            //tmp.erase(it);
            //test1(tmp, &tree);
            assert(!tree.find(it, record));
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::ERASE: %f seconds\n", duration );
    }
    assert(tree.size() == 0);
    return 0;
}


