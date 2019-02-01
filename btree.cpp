#include "btree.h"
#include "assert.h"

bool cc(int* arr, int cnt)
{
    int cur = arr[0];
    bool ret = true;
    for (int i = 1; i < cnt; ++i)
    {
        if (arr[i] > cur)
            cur = arr[i];
        else
            return false;
    }
    return ret;
}

bool BTLeafNode::search_record(int key, Record*& out_record)
{
    int L = 0, R = used_links_count_-2;
    while (L <= R)
    {
        int M = L + ((R - L) >> 1);
        if (keys_[M] == key)
        {
            out_record = record_links_[M];
            return true;
        }
        else if (keys_[M] > key)
        {
            R = M - 1;
        }
        else
        {
            L = M + 1;
        }
    }
    return false;
}

bool BTInnerNode::search_node(int key, BTNode*& out_record)
{
    int L = 0, R = used_links_count_ - 1;
    while (L < R)
    {
        int M = L + ((R-L)>>1);   // avoid (L+R)/2 overflow
        if (keys_[M] == key)
        {
            out_record = links_[M+1];
            return true;
        }
        else if (keys_[M] > key)
            R = M;
        else
            L = 1 + M;
    }
    out_record = links_[L];
    return true;
}

bool BTree::search(int key, Record*& out)
{
    BTNode* node = root_;
    while (!node->is_leaf_node())
    {
        dynamic_cast<BTInnerNode*>(node)->search_node(key, node);
    }
    return dynamic_cast<BTLeafNode*>(node)->search_record(key, out);
}

bool BTree::insert(int key, Record* record)
{
    if (!root_)
    {
        auto t = new BTLeafNode;
        root_ = t;
        t->used_links_count_ = 2;
        t->keys_[0] = key;
        t->record_links_[0] = record;
        t->next_ = nullptr;
        return true;
    }
    BTNode* node = root_;
    while (!node->is_leaf_node())
    {
        dynamic_cast<BTInnerNode*>(node)->search_node(key, node);
    }
    bool err = false;
    BTNode* M = dynamic_cast<BTLeafNode*>(node)->insert_(key, record, err);
    if (!err) return false; // exists

    int kk = 0;
    if (M)  kk = M->keys_[0];
    while (M)
    {
        BTInnerNode* parent = dynamic_cast<BTInnerNode*>(node->parent_);
        if (!parent)
        {
            BTInnerNode* rt = new BTInnerNode;
            rt->used_links_count_ = 2;
            rt->keys_[0] = kk;
            if (node->keys_[0] < M->keys_[0])
            {
                rt->set_link(0, node);//rt->links_[0] = node;
                rt->set_link(1, M);//rt->links_[1] = M;
            }
            else
            {
                rt->set_link(0, M);//rt->links_[0] = M;
                rt->set_link(1, node);//rt->links_[1] = node;
            }
            root_ = rt;
            rt->resolve_brother();
            break;
        }
        else
        {
            int newk = 0;
            M = parent->insert_(kk, M, newk);
            kk = newk;
            node = parent;
        }
    }
    return true;
}

bool BTree::erase(int key, Record*& out_record)
{
    if (!root_)
    {
        return false;   /* EMPTY */
    }
    BTNode* node = root_;
    while (!node->is_leaf_node())
    {
        dynamic_cast<BTInnerNode*>(node)->search_node(key, node);
    }
    bool err = false;
    BTNode* M = dynamic_cast<BTLeafNode*>(node)->erase_(key, out_record, err);
    if (!err) return false; // not exists
}

BTLeafNode* BTLeafNode::insert_(int key, Record* record, bool& out)
{
    out = true;
    Record* a = nullptr;
    if (!search_record(key, a))
    {
        if (!full())
        {
            int i = 0;
            for (i = used_links_count_ - 2; i >= 0; --i)
            {
                if (keys_[i] > key)
                {
                    keys_[i+1] = keys_[i];
                    record_links_[i+1] = record_links_[i];
                }
                else
                {
                    break;
                }
            }
            keys_[i+1] = key;
            record_links_[i+1] = record;
            used_links_count_++;
        }
        else
        {
            return split(key, record);
        }
    }
    else
    {
        out = false;
    }
    return nullptr;
}

BTLeafNode* BTLeafNode::split(int key, Record* record)
{
    BTLeafNode* M = new BTLeafNode;
    M->next_ = this->next_;
    this->next_ = M;
    int SN = (key_count() + 1) / 2;
    int SM = (key_count() + 1) - SN;

    int SN1 = SN,SM1 = SM;

    bool flag = false;  // first
    for (int i = key_count() - 1; i >= 0; --i)
    {
        int k = keys_[i];
        Record* r = record_links_[i];
        if (keys_[i] < key && !flag)
        {
            i++;
            k = key;
            r = record;
            flag = true;
        }
        if (SM > 0)
        {
            M->keys_[SM-1] = k;
            M->record_links_[SM-1] = r;
            SM--;
        }
        else
        {
            keys_[SN-1] = k;
            record_links_[SN-1] = r;
            SN--;
        }
    }
    if (SN > 0)
    {
        keys_[SN-1] = key;
        record_links_[SN-1] = record;
    }
    M->parent_ = parent_;
    this->used_links_count_ = 1 + SN1;
    M->used_links_count_ = 1 + SM1;

    assert(cc(this->keys_, this->key_count()));
    assert(cc(M->keys_, M->key_count()));

    return M;
}

BTLeafNode* BTLeafNode::erase_(int key, Record*& out_record, bool& out)
{
    out = true;
    if (search_record(key, out_record))
    {
        if (check_if_erase())
        {
            //only need erase
        }
        else
        {

        }
    }
    else
    {
        out = false;
    }
    return nullptr;
}

BTInnerNode* BTInnerNode::insert_(int key, BTNode* link, int& out_key)
{
    if (!full())
    {
        {
            int i = 0;
            for (i = key_count() - 1; i >= 0; --i)
            {
                if (keys_[i] > key)
                    keys_[i+1] = keys_[i];
                else
                    break;
            }
            keys_[i+1] = key;
        }
        {
            int  i = 0;
            int used_links_count1 = used_links_count_;
            used_links_count_++;
            for (i = used_links_count1-1; i >= 0; --i)
            {
                if (links_[i]->keys_[0] > link->keys_[0])
                    set_link(i+1, links_[i]);//links_[i+1] = links_[i];
                else
                    break;
            }
            set_link(i+1, link);//links_[i+1] = link;
            resolve_brother();
        }
    }
    else
    {
        return split(key, link, out_key);
    }
    return nullptr;
}

BTInnerNode* BTInnerNode::split(int key, BTNode* link, int& out_key)
{
    assert(cc(this->keys_, this->key_count()));

    BTInnerNode* M = new BTInnerNode;
    M->parent_ = parent_;
    int SN = (used_links_count_ - 1) / 2 ;       // 1
    int SM = used_links_count_-1 - SN;        // 2

    int MID = SN;

    int cur = used_links_count_ - 1;
    int SN1 = SN, SM1 = SM;
    bool flag1 = false;  // first
    for (int i = key_count() - 1; i >= 0; --i,--cur)
    {
        int k = keys_[i];
        if (keys_[i] < key && !flag1)
        {
            i++;
            k = key;
            flag1 = true;
        }
        if (cur > MID)
        {
            M->keys_[SM1-1] = k;
            SM1--;
        }
        else if (cur < MID)
        {
            keys_[SN1-1] = k;
            SN1--;
        }
        else
        {
            out_key = k;
            continue;
        }
    }
    if (SN1 > 0)
    {
        assert(SN1 == 1);
        keys_[SN1-1] = key;
    }


    int SN2 = SN+1,SM2 = SM+1;
    bool flag2 = false;
    int used_links_count1 = used_links_count_;
    M->used_links_count_ = SM+1;
    used_links_count_ = SN+1;
    for (int i = used_links_count1 - 1; i >= 0; --i)
    {
        BTNode* n = links_[i];
        if (links_[i]->keys_[0] < link->keys_[0] && !flag2)
        {
            i++;
            n = link;
            flag2 = true;
        }

        if (SM2 > 0)
        {
            M->set_link(SM2-1, n);//M->links_[SM2-1] = n;
            SM2--;
        }
        else
        {
            set_link(SN2-1, n);//links_[SN2-1] = n;
            SN2--;
        }
    }
    if (SN2 > 0)
    {
        set_link(SN2-1, link);//links_[SN2-1] = link;
    }

    this->resolve_brother();
    M->resolve_brother();
    assert(cc(this->keys_, this->key_count()));
    assert(cc(M->keys_, M->key_count()));
    return M;
}

void BTInnerNode::set_link(int idx, BTNode* link)
{
    assert(0 <= idx && idx < used_links_count_);

    links_[idx] = link;
    link->parent_ = this;
}

void BTInnerNode::resolve_brother()
{
    BTNode* cur = nullptr, *next = nullptr,*prev = nullptr;
    for (int i = 0; i < used_links_count_-1; ++i)
    {
        cur = links_[i];
        next = links_[1+i];
        cur->right_brother_ = next;
        cur->left_brother_ = prev;
        prev = cur;
    }
    links_[used_links_count_-1]->right_brother_ = nullptr;
    links_[used_links_count_-1]->left_brother_ = cur;
}
