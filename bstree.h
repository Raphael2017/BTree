#ifndef BSTREE_H
#define BSTREE_H

#include <stdlib.h>

namespace search
{
    typedef void Record;
    struct BSTNode
    {
        int key_;
        Record * record_{nullptr};
        BSTNode* left_{nullptr};
        BSTNode* right_{nullptr};
        BSTNode* parent_{nullptr};
        BSTNode* forerunner();
        BSTNode* successor();
        BSTNode* max();
        BSTNode* min();
        bool insert(int key, Record* r);
        bool erase(int key, Record*& r);
        BSTNode* search(int key);
    };

    struct BSTree
    {
    public:
        bool insert(int key, Record* r);
        bool erase(int key, Record*& r);
        bool find(int key, Record*& r);
        size_t size();
        void clear();
    private:
        bool _insert_(int key, Record* r);
        bool _erase_(int key, Record*& r);
        static void _clear_(BSTNode* node);
        BSTNode* root_{nullptr};
        size_t size_{0};
    };
}

#endif