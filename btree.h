#ifndef BTREE_H
#define BTREE_H

#include <stddef.h>
#define BTREE_N 128

struct Record
{
    int data_;
};

struct BTNode
{
    BTNode* left_brother_{nullptr};
    BTNode* right_brother_{nullptr};
    BTNode* get_left_brother() { return left_brother_; }
    BTNode* get_right_brother(){ return right_brother_; }
    BTNode* parent_{nullptr};
    int keys_[BTREE_N];
    int used_links_count_{0};
    bool check() { return used_links_count_ >= ((BTREE_N+1)/2); }
    bool check_if_erase() { return (used_links_count_-1) >= ((BTREE_N+1)/2); }
    bool full() { return used_links_count_ >= 1 + BTREE_N; }
    int key_count() { return used_links_count_ - 1; }
    virtual bool is_leaf_node() = 0;
};

struct BTLeafNode : public BTNode
{
    Record* record_links_[BTREE_N]{0};
    BTLeafNode* next_{0};
    bool search_record(int key, Record*& out_record);
    BTLeafNode* insert_(int key, Record* record, bool& out);
    BTLeafNode* split(int key, Record* record);
    BTLeafNode* erase_(int key, Record*& out_record, bool& out);
    virtual bool is_leaf_node() override { return true; }
};

struct BTInnerNode : public BTNode
{
    void set_link(int idx, BTNode* l);
    void resolve_brother();
    BTNode* links_[BTREE_N+1]{0};
    bool search_node(int key, BTNode*& out_record);
    BTInnerNode* insert_(int key, BTNode* link, int& outkey);
    BTInnerNode* split(int key, BTNode* link, int& out_key);
    virtual bool is_leaf_node() override { return false; }
};

struct BTree
{
    BTNode* root_{nullptr};
    bool search(int key, Record*& out);
    bool insert(int key, Record* record);
    bool erase(int key, Record*& out_record);
    size_t size();
};


#endif