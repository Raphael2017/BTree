#ifndef BTREE_H
#define BTREE_H

#include <stddef.h>
#include <mcheck.h>

#define BTREE_N 5

#define BROTHER_ALGO1

namespace storage {

    typedef void Record;

    struct BTNode {
        //BTNode* left_brother_{nullptr};
        //BTNode* right_brother_{nullptr};
        BTNode *get_left_brother();

        BTNode *get_right_brother();

        BTNode *parent_{nullptr};
        int child_index_{-1};
        int keys_[BTREE_N];
        int used_links_count_{0};

        bool check() { return used_links_count_ >= ((BTREE_N + 1) / 2); }

        bool check_if_erase() { return (used_links_count_ - 1) >= ((BTREE_N + 1) / 2); }

        bool full() { return used_links_count_ >= 1 + BTREE_N; }

        int key_count() { return used_links_count_ - 1; }

        virtual bool is_leaf_node() = 0;

        virtual BTNode *get_child(int idx) = 0;
    };

    struct BTLeafNode : public BTNode {
        Record *record_links_[BTREE_N]{0};
        BTLeafNode *next_{0};

        bool search_record(int key, Record *&out_record, int &out_index);

        BTLeafNode *insert_(int key, Record *record, bool &out);

        BTLeafNode *split(int key, Record *record);

        int erase_(int key, Record *&out_record, bool &out);

        virtual bool is_leaf_node() override { return true; }

        virtual BTNode *get_child(int idx) override {
            return nullptr;
        }
    };

    struct BTInnerNode : public BTNode {
        void set_link(int idx, BTNode *l);

        void resolve_brother();

        BTNode *links_[BTREE_N + 1]{0};

        bool search_node(int key, BTNode *&out_record);

        BTInnerNode *insert_(int key, BTNode *link, int &outkey);

        BTInnerNode *split(int key, BTNode *link, int &out_key);

        virtual bool is_leaf_node() override { return false; }

        virtual BTNode *get_child(int idx) override {
            if (0 <= idx && idx < used_links_count_) {
                return links_[idx];
            }
            return nullptr;
        }

        BTInnerNode *erase_(int idx, int &out_idx);
    };

    struct BTree {
    public:
        bool find(int key, Record *&out);

        /*
         * todo range find
         * */

        bool insert(int key, Record *record);

        bool erase(int key, Record *&out_record);

        size_t size();

        bool check();

        void clear();

    private:
        bool _search_(int key, Record *&out);

        bool _insert_(int key, Record *record);

        bool _erase_(int key, Record *&out_record);
        size_t _size_();
        BTNode *root_{nullptr};
        int size_{0};

        static bool check_(BTNode *nd);
        static void clear_(BTNode* node);
    };

}
#endif