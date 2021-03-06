#include <iostream>
#include <assert.h>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <time.h>

#include "btree.h"
#include "bstree.h"


struct Record
{
    int data_;
};

size_t total = 10000000;
#define switch_ true

void test1(const std::map<int, Record*>& tmp, storage::BTree* tree)
{
    assert(tree->check());
    storage::Record * r = nullptr;
    for (auto it : tmp)
    {
        assert(tree->find(it.first, r));
    }
}

int main0()
{
    storage::BTree tree;
    std::vector<int> src{18, 31, 12, 10, 15, 48, 45, 47, 50, 52, 23, 30, 20};
    for (auto it : src)
    {
        printf("\n\nINSERT %d:\n", it);
        tree.insert(it, nullptr);
        tree.print();
    }

    std::vector<int> del{15, 18, 23, 30, 31, 52, 50};
    for (auto it : src)
    {
        printf("\n\nERASE %d:\n", it);
        storage::Record *r = nullptr;
        tree.erase(it, r);
        tree.print();
    }
    return 0;
}


int main() {
    storage::BTree tree;
    storage::Record* record = nullptr;

    std::map<int, Record*> tmp;
    std::vector<int> src;

    clock_t start, finish;
    double duration;


    for (size_t i = 0; i < total; ++i)
        src.push_back(i);


    if (switch_)
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

    if (switch_)
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
    if (switch_)
        assert(tmp.size() == tree.size());
    assert(tree.check());

    if (switch_)
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
    if (true)
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
    else
    {
        std::map<Record*, bool > tttt;
        for (auto it : src)
        {
            record = nullptr;
            if (tree.find(it, record))
            {
                tttt[(Record*)record] = true;
            }
        }
        for (auto it : tttt)
            delete(it.first);
        start = clock();
        tree.clear();
        assert(tree.size() == 0);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::CLEAR: %f seconds\n", duration );
    }
    assert(tree.size() == 0);
    return 0;
}

int main1()
{
    search::BSTree tree;
    search::Record* record = nullptr;

    std::map<int, Record*> tmp;
    std::vector<int> src;

    clock_t start, finish;
    double duration;


    for (size_t i = 0; i < total; ++i)
        src.push_back(rand());


    if (switch_)
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

    if (switch_)
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
    if (switch_)
        assert(tmp.size() == tree.size());

    if (switch_)
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
    if (false)
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
    else
    {
        std::map<Record*, bool > tttt;
        for (auto it : src)
        {
            record = nullptr;
            if (tree.find(it, record))
            {
                tttt[(Record*)record] = true;
            }
        }
        for (auto it : tttt)
            delete(it.first);
        start = clock();
        tree.clear();
        assert(tree.size() == 0);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "BTREE::CLEAR: %f seconds\n", duration );
    }
    assert(tree.size() == 0);
    return 0;
}


