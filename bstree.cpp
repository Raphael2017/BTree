#include <term.h>
#include "bstree.h"
#include <assert.h>

namespace search
{
    BSTNode* BSTNode::forerunner()
    {
        if (left_)
            return left_->max();
        else
        {
            BSTNode* node = this;
            BSTNode* p = parent_;
            while (p && p->left_ == node)
            {
                // this is min of node
                node = p;
                p = node->parent_;
            }
            return p;
        }
    }

    BSTNode* BSTNode::successor()
    {
        if (right_)
            return right_->min();
        else
        {
            BSTNode* node = this;
            BSTNode* p = parent_;
            while (p && p->right_ == node)
            {
                node = p;
                p = node->parent_;
            }
        }
    }

    BSTNode* BSTNode::max()
    {
        BSTNode* node = this;
        while (node->right_)
        {
            node = node->right_;
        }
        return node;
    }

    BSTNode* BSTNode::min()
    {
        BSTNode* node = this;
        while (node->left_)
        {
            node = node->left_;
        }
        return node;
    }

    bool BSTNode::insert(int key, Record* r)
    {
        BSTNode* cur = this;
        BSTNode* p = cur->parent_;
        while (true)
        {
            if (!cur)
            {
                assert(p->key_ != key);
                BSTNode* nd = new BSTNode;
                nd->parent_ = p;
                nd->key_ = key;
                nd->record_ = r;
                if (p->key_ < key)
                {
                    p->right_ = nd;
                }
                else
                {
                    p->left_  = nd;
                }
                return true;
            }
            else
            {
                if (cur->key_ == key)
                    return false;
                else if (cur->key_ < key)
                {
                    p = cur;
                    cur = cur->right_;
                }

                else
                {
                    p = cur;
                    cur = cur->left_;
                }
            }
        }
    }

    bool BSTNode::erase(int key, Record*& r)
    {
        BSTNode* nd = search(key);
        if (!nd)
            return false;
        r = nd->record_;
        if (!nd->left_ && !nd->right_)
        {

            if (nd->key_ > nd->parent_->key_)
                nd->parent_->right_ = nullptr;
            else
                nd->parent_->left_ = nullptr;
            delete nd;
        }
        else if (!nd->left_ && nd->right_)
        {
            if (nd->key_ > nd->parent_->key_)
                nd->parent_->right_ = nd->right_;
            else
                nd->parent_->left_ = nd->right_;
            nd->right_->parent_ = nd->parent_;
            delete nd;
        }
        else if (nd->left_ && !nd->right_)
        {
            if (nd->key_ > nd->parent_->key_)
                nd->parent_->right_ = nd->left_;
            else
                nd->parent_->left_ = nd->left_;
            nd->left_->parent_ = nd->parent_;
            delete nd;
        }
        else
        {
            auto su = nd->successor();
            Record* r1 = nullptr;
            int kk = su->key_;
            nd->erase(su->key_, r1);
            nd->key_ = kk;
            nd->record_ = r1;
        }
        return true;
    }

    BSTNode* BSTNode::search(int key)
    {
        BSTNode* cur = this;
        while (true)
        {
            if (!cur)
            {
                return nullptr;
            }
            else
            {
                if (cur->key_ == key)
                    return cur;
                else if (cur->key_ < key)
                    cur = cur->right_;
                else
                    cur = cur->left_;
            }
        }
    }

    bool BSTree::insert(int key, Record* r)
    {
        auto ret = _insert_(key, r);
        if (ret)
            size_++;
        return ret;
    }
    bool BSTree::erase(int key, Record*& r)
    {
        auto ret = _erase_(key, r);
        if (ret)
            size_--;
        return ret;
    }

    bool BSTree::_insert_(int key, Record* r)
    {
        if (!root_)
        {
            root_ = new BSTNode;
            root_->key_ = key;
            root_->record_ = r;
            return true;
        }
        else
        {
            return root_->insert(key, r);
        }
    }

    bool BSTree::_erase_(int key, Record*& r)
    {
        BSTNode* nd = root_->search(key);
        if (!nd)
            return false;
        if (nd == root_)
        {
            r = nd->record_;
            if (!nd->left_ && !nd->right_)
            {
                delete root_;
                root_ = nullptr;
                return true;
            }
            else if (!nd->left_ && nd->right_)
            {
                root_ = nd->right_;
                delete nd;
                return true;
            }
            else if (nd->left_ && !nd->right_)
            {
                root_ = nd->left_;
                delete nd;
                return true;
            }
        }
        return root_->erase(key, r);
    }

    bool BSTree::find(int key, Record*& r)
    {
        if (!root_)
            return false;
        auto ret =  root_->search(key);
        if (ret)
        {
            r = ret;
            return true;
        }
        else
            return false;
    }

    size_t BSTree::size()
    {
        return size_;
    }

    void BSTree::clear()
    {
        _clear_(root_);
        root_ = nullptr;
        size_ = 0;
    }

    void BSTree::_clear_(BSTNode* node)
    {
        if (node->left_)
            _clear_(node->left_);
        if (node->right_)
            _clear_(node->right_);
        delete node;
    }

}