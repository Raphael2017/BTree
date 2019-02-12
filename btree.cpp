#include "btree.h"
#include "assert.h"

namespace storage {

    bool cc(int *arr, int cnt) {
        int cur = arr[0];
        bool ret = true;
        for (int i = 1; i < cnt; ++i) {
            if (arr[i] > cur)
                cur = arr[i];
            else
                return false;
        }
        return ret;
    }

    bool ll(BTNode **links, int cnt) {
        for (int i = 0; i < cnt; ++i) {
            assert(links[i]);
            if (links[i]->child_index_ != i)
                return false;
        }
        return true;
    }

#ifdef BROTHER_ALGO1

    BTNode *BTNode::get_left_brother() {
        if (child_index_ == -1) {
            return nullptr;
        } else {
            assert(parent_);
            return parent_->get_child(child_index_ - 1);
        }
    }

    BTNode *BTNode::get_right_brother() {
        if (child_index_ == -1) {
            return nullptr;
        } else {
            assert(parent_);
            return parent_->get_child(child_index_ + 1);
        }
    }

#else
    BTNode* BTNode::get_left_brother()
    {
        if (child_index_ == -1)
        {
            assert(parent_ == nullptr);
            assert(left_brother_ == nullptr);
        }
        else
        {
            assert(left_brother_ == parent_->get_child(child_index_ - 1));
        }
        return left_brother_;
    }

    BTNode* BTNode::get_right_brother()
    {
        if (child_index_ == -1)
        {
            assert(parent_ == nullptr);
            assert(right_brother_ == nullptr);
        }
        else
        {
            assert(right_brother_ == parent_->get_child(child_index_ + 1));
        }
        return right_brother_;
    }
#endif

    bool BTLeafNode::search_record(int key, Record *&out_record, int &out_index) {
        int L = 0, R = used_links_count_ - 2;
        while (L <= R) {
            int M = L + ((R - L) >> 1);
            if (keys_[M] == key) {
                out_record = record_links_[M];
                out_index = M;
                return true;
            } else if (keys_[M] > key) {
                R = M - 1;
            } else {
                L = M + 1;
            }
        }
        return false;
    }

    bool BTInnerNode::search_node(int key, BTNode *&out_record) {
        int L = 0, R = used_links_count_ - 1;
        while (L < R) {
            int M = L + ((R - L) >> 1);   // avoid (L+R)/2 overflow
            if (keys_[M] == key) {
                out_record = links_[M + 1];
                return true;
            } else if (keys_[M] > key)
                R = M;
            else
                L = 1 + M;
        }
        out_record = links_[L];
        return true;
    }

    bool BTree::find(int key, Record *&out) {
        return _search_(key, out);
    }

    bool BTree::insert(int key, Record *record) {
        bool ret = _insert_(key, record);
        if (ret)
            size_++;
        return ret;
    }

    bool BTree::erase(int key, Record *&out_record) {
        bool ret = _erase_(key, out_record);
        if (ret)
            size_--;
        return ret;
    }

    bool BTree::_search_(int key, Record *&out) {
        if (!root_)
            return false;
        BTNode *node = root_;
        while (!node->is_leaf_node()) {
            dynamic_cast<BTInnerNode *>(node)->search_node(key, node);
        }
        int index = -1;
        return dynamic_cast<BTLeafNode *>(node)->search_record(key, out, index);
    }

    bool BTree::_insert_(int key, Record *record) {
        if (!root_) {
            auto t = new BTLeafNode;
            root_ = t;
            t->used_links_count_ = 2;
            t->keys_[0] = key;
            t->record_links_[0] = record;
            t->next_ = nullptr;
            return true;
        }
        BTNode *node = root_;
        while (!node->is_leaf_node()) {
            dynamic_cast<BTInnerNode *>(node)->search_node(key, node);
        }
        bool err = false;
        BTNode *M = dynamic_cast<BTLeafNode *>(node)->insert_(key, record, err);
        if (!err) return false; // exists

        int kk = 0;
        if (M) kk = M->keys_[0];
        while (M) {
            BTInnerNode *parent = dynamic_cast<BTInnerNode *>(node->parent_);
            if (!parent) {
                BTInnerNode *rt = new BTInnerNode;
                rt->used_links_count_ = 2;
                rt->keys_[0] = kk;
                if (node->keys_[0] < M->keys_[0]) {
                    rt->set_link(0, node);//rt->links_[0] = node;
                    rt->set_link(1, M);//rt->links_[1] = M;
                } else {
                    rt->set_link(0, M);//rt->links_[0] = M;
                    rt->set_link(1, node);//rt->links_[1] = node;
                }
                root_ = rt;
                rt->resolve_brother();
                break;
            } else {
                int newk = 0;
                M = parent->insert_(kk, M, newk);
                kk = newk;
                node = parent;
            }
        }
        return true;
    }

    bool BTree::_erase_(int key, Record *&out_record) {
        if (!root_) {
            return false;   /* EMPTY */
        }
        BTNode *node = root_;
        while (!node->is_leaf_node()) {
            dynamic_cast<BTInnerNode *>(node)->search_node(key, node);
        }
        if (node == root_) {
            auto leaf = dynamic_cast<BTLeafNode *>(node);
            if (leaf->key_count() > 1) {
                bool err = false;
                leaf->erase_(key, out_record, err);
                if (!err) {
                    assert(out_record == nullptr);
                    return false; // not exists
                } else
                    return true;

            } else if (leaf->key_count() == 1) {
                out_record = leaf->record_links_[0];
                delete root_;
                root_ = nullptr;
                return true;
            } else {
                assert(false);
            }
        }
        bool err = false;
        BTInnerNode *parent = dynamic_cast<BTInnerNode *>(node->parent_);
        int idx = dynamic_cast<BTLeafNode *>(node)->erase_(key, out_record, err);
        if (!err) {
            assert(out_record == nullptr);
            return false; // not exists
        }

        BTInnerNode *cur = nullptr;
        while (idx != -1) {
            if (parent) {
                auto tt = parent->links_[idx + 1];
                BTInnerNode *nd = parent->erase_(idx, idx);
                if (!nd && idx != -1) {
                    assert(parent == root_);
                    cur = parent;
                    break;
                } else {
                    delete tt;
                    parent = nd;
                }
            }
        }
        if (cur) {
            if (cur->used_links_count_ > 2) {
                for (int i = idx + 1; i < cur->used_links_count_ - 1; ++i) {
                    cur->keys_[i - 1] = cur->keys_[i];
                }
                delete cur->links_[idx + 1];
                for (int i = idx + 2; i < cur->used_links_count_; ++i) {
                    cur->set_link(i - 1, cur->links_[i]);
                }
                cur->used_links_count_--;
                assert(ll(cur->links_, cur->used_links_count_));
            } else if (cur->used_links_count_ == 2) {
                delete cur->links_[1];
                root_ = cur->links_[0];
                root_->parent_ = nullptr;
                root_->child_index_ = -1;
                delete cur;
            } else {
                assert(false);
            }
        }
        return true;
    }

    size_t BTree::size() {
        return size_;
    }

    size_t BTree::_size_()
    {
        if (!root_)
            return 0;
        BTNode *node = root_;
        while (!node->is_leaf_node()) {
            node = dynamic_cast<BTInnerNode *>(node)->links_[0];
        }
        assert(node && node->is_leaf_node());
        auto leaf = dynamic_cast<BTLeafNode *>(node);
        assert(leaf);
        size_t ret = 0;
        while (leaf) {
            ret += leaf->used_links_count_ - 1;
            leaf = leaf->next_;
        }
        return ret;
    }

    bool BTree::check() {
        if (!root_) return true;
        return check_(root_);
    }

    bool BTree::check_(BTNode *nd) {
        nd->get_right_brother();
        nd->get_left_brother();
        assert(cc(nd->keys_, nd->key_count()));
        BTNode *node = nd;
        if (!node->parent_) {
            if (node->used_links_count_ < 2)
                return false;
        } else {
            if (!node->check())
                return false;
        }
        if (!node->is_leaf_node()) {
            auto inner_node = dynamic_cast<BTInnerNode *>(node);
            for (int i = 0; i < inner_node->used_links_count_; ++i) {
                if (!check_(inner_node->links_[i]))
                    return false;
            }
        }
        return true;
    }

    BTLeafNode *BTLeafNode::insert_(int key, Record *record, bool &out) {
        out = true;
        Record *a = nullptr;
        int index = -1;
        if (!search_record(key, a, index)) {
            if (!full()) {
                int i = 0;
                for (i = used_links_count_ - 2; i >= 0; --i) {
                    if (keys_[i] > key) {
                        keys_[i + 1] = keys_[i];
                        record_links_[i + 1] = record_links_[i];
                    } else {
                        break;
                    }
                }
                keys_[i + 1] = key;
                record_links_[i + 1] = record;
                used_links_count_++;
            } else {
                return split(key, record);
            }
        } else {
            out = false;
        }
        return nullptr;
    }

    BTLeafNode *BTLeafNode::split(int key, Record *record) {
        BTLeafNode *M = new BTLeafNode;
        M->next_ = this->next_;
        this->next_ = M;
        int SN = (key_count() + 1) / 2;
        int SM = (key_count() + 1) - SN;

        int SN1 = SN, SM1 = SM;

        bool flag = false;  // first
        for (int i = key_count() - 1; i >= 0; --i) {
            int k = keys_[i];
            Record *r = record_links_[i];
            if (keys_[i] < key && !flag) {
                i++;
                k = key;
                r = record;
                flag = true;
            }
            if (SM > 0) {
                M->keys_[SM - 1] = k;
                M->record_links_[SM - 1] = r;
                SM--;
            } else {
                keys_[SN - 1] = k;
                record_links_[SN - 1] = r;
                SN--;
            }
        }
        if (SN > 0) {
            keys_[SN - 1] = key;
            record_links_[SN - 1] = record;
        }
        M->parent_ = parent_;
        this->used_links_count_ = 1 + SN1;
        M->used_links_count_ = 1 + SM1;

        assert(cc(this->keys_, this->key_count()));
        assert(cc(M->keys_, M->key_count()));

        return M;
    }

    int BTLeafNode::erase_(int key, Record *&out_record, bool &out) {
        out = true;
        int index = -1;
        if (search_record(key, out_record, index)) {
            if (check_if_erase() || (parent_ == nullptr))               //only need erase
            {
                for (int i = index + 1; i < used_links_count_ - 1; ++i) {
                    keys_[i - 1] = keys_[i];
                    record_links_[i - 1] = record_links_[i];
                }
                used_links_count_--;
            } else {
                BTLeafNode *l = dynamic_cast<BTLeafNode *>(get_left_brother());
                BTLeafNode *r = dynamic_cast<BTLeafNode *>(get_right_brother());
                if (l && l->check_if_erase()) {
                    // move one from left brother
                    int &key_infl = parent_->keys_[child_index_ - 1];

                    int key_move = l->keys_[l->used_links_count_ - 2];
                    Record *record_move = l->record_links_[l->used_links_count_ - 2];

                    bool a = false;
                    insert_(key_move, record_move, a);
                    assert(a);
                    l->erase_(key_move, out_record, a);
                    assert(a);
                    erase_(key, out_record, out);
                    key_infl = key_move;
                } else if (r && r->check_if_erase()) {
                    // move one from right brother
                    int &key_infl = parent_->keys_[child_index_];

                    int key_move = r->keys_[0];
                    Record *record_move = r->record_links_[0];
                    Record *rec = nullptr;
                    bool a = false;
                    r->erase_(key_move, rec, a);
                    assert(a);

                    insert_(key_move, record_move, a);
                    assert(a);
                    erase_(key, out_record, out);
                    key_infl = r->keys_[0];
                } else {
                    if (l) {
                        int idx = child_index_ - 1;
                        for (int i = 0; i < used_links_count_ - 1; ++i) {
                            if (keys_[i] == key)
                                continue;
                            bool a = false;
                            l->insert_(keys_[i], record_links_[i], a);
                            assert(a);
                        }
                        l->next_ = this->next_;
                        return idx;
                    } else if (r) {
                        bool a = false;
                        int idx = child_index_;
                        insert_(r->keys_[0], r->record_links_[0], a);
                        assert(a);
                        erase_(key, out_record, out);
                        for (int i = 1; i < r->used_links_count_ - 1; ++i) {
                            insert_(r->keys_[i], r->record_links_[i], a);
                            assert(a);
                        }
                        this->next_ = r->next_;
                        return idx;
                    } else {
                        // this must be a root
                        assert(parent_ == nullptr);
                    }
                }
            }
        } else {
            out = false;
        }
        return -1;
    }

    BTInnerNode *BTInnerNode::insert_(int key, BTNode *link, int &out_key) {
        if (!full()) {
            {
                int i = 0;
                for (i = key_count() - 1; i >= 0; --i) {
                    if (keys_[i] > key)
                        keys_[i + 1] = keys_[i];
                    else
                        break;
                }
                keys_[i + 1] = key;
            }
            {
                int i = 0;
                int used_links_count1 = used_links_count_;
                used_links_count_++;
                for (i = used_links_count1 - 1; i >= 0; --i) {
                    if (links_[i]->keys_[0] > link->keys_[0])
                        set_link(i + 1, links_[i]);//links_[i+1] = links_[i];
                    else
                        break;
                }
                set_link(i + 1, link);//links_[i+1] = link;
                resolve_brother();
            }
        } else {
            return split(key, link, out_key);
        }
        return nullptr;
    }

    BTInnerNode *BTInnerNode::split(int key, BTNode *link, int &out_key) {
        assert(cc(this->keys_, this->key_count()));

        BTInnerNode *M = new BTInnerNode;
        M->parent_ = parent_;
        int SN = (used_links_count_ - 1) / 2;       // 1
        int SM = used_links_count_ - 1 - SN;        // 2

        int MID = SN;

        int cur = used_links_count_ - 1;
        int SN1 = SN, SM1 = SM;
        bool flag1 = false;  // first
        for (int i = key_count() - 1; i >= 0; --i, --cur) {
            int k = keys_[i];
            if (keys_[i] < key && !flag1) {
                i++;
                k = key;
                flag1 = true;
            }
            if (cur > MID) {
                M->keys_[SM1 - 1] = k;
                SM1--;
            } else if (cur < MID) {
                keys_[SN1 - 1] = k;
                SN1--;
            } else {
                out_key = k;
                continue;
            }
        }
        if (SN1 > 0) {
            assert(SN1 == 1);
            keys_[SN1 - 1] = key;
        }


        int SN2 = SN + 1, SM2 = SM + 1;
        bool flag2 = false;
        int used_links_count1 = used_links_count_;
        M->used_links_count_ = SM + 1;
        used_links_count_ = SN + 1;
        for (int i = used_links_count1 - 1; i >= 0; --i) {
            BTNode *n = links_[i];
            if (links_[i]->keys_[0] < link->keys_[0] && !flag2) {
                i++;
                n = link;
                flag2 = true;
            }

            if (SM2 > 0) {
                M->set_link(SM2 - 1, n);//M->links_[SM2-1] = n;
                SM2--;
            } else {
                set_link(SN2 - 1, n);//links_[SN2-1] = n;
                SN2--;
            }
        }
        if (SN2 > 0) {
            set_link(SN2 - 1, link);//links_[SN2-1] = link;
        }

        this->resolve_brother();
        M->resolve_brother();
        assert(cc(this->keys_, this->key_count()));
        assert(cc(M->keys_, M->key_count()));
        return M;
    }

    BTInnerNode *BTInnerNode::erase_(int idx, int &out_idx) {
        BTInnerNode *parent = dynamic_cast<BTInnerNode *>(parent_);
        out_idx = -1;
        assert(idx != -1);
        if (check_if_erase()) {
            for (int i = idx + 1; i < used_links_count_ - 1; ++i) {
                keys_[i - 1] = keys_[i];
            }
            for (int i = idx + 2; i < used_links_count_; ++i) {
                set_link(i - 1, links_[i]);
            }
            used_links_count_--;
            assert(ll(links_, used_links_count_));
            assert(ll(this->links_, used_links_count_));
        } else {
            assert(true);
            BTInnerNode *l = dynamic_cast<BTInnerNode *>(get_left_brother());
            BTInnerNode *r = dynamic_cast<BTInnerNode *>(get_right_brother());
            if (l && l->check_if_erase()) {
                assert(ll(this->links_, used_links_count_));
                assert(ll(l->links_, l->used_links_count_));
                int move_key = l->keys_[l->used_links_count_ - 2];
                int upper_key = l->parent_->keys_[l->child_index_];
                l->parent_->keys_[l->child_index_] = move_key;
                BTNode *move_link = l->links_[l->used_links_count_ - 1];

                int tmp = -2;
                l->erase_(l->used_links_count_ - 2, tmp);

                for (int i = used_links_count_ - 2; i >= 0; --i) {
                    keys_[i + 1] = keys_[i];
                }
                for (int i = used_links_count_ - 1; i >= 0; --i) {
                    set_link(i + 1, links_[i]);
                }
                used_links_count_++;
                keys_[0] = upper_key;
                set_link(0, move_link);
                int aaaa = -2;
                erase_(1 + idx, aaaa);
                assert(aaaa == -1);
                assert(cc(this->keys_, this->key_count()));
                assert(cc(l->keys_, l->key_count()));
                assert(l->keys_[l->key_count() - 1] < l->parent_->keys_[l->child_index_]);
                assert(this->keys_[0] >= l->parent_->keys_[l->child_index_]);
                assert(ll(this->links_, used_links_count_));
                assert(ll(l->links_, l->used_links_count_));
            } else if (r && r->check_if_erase()) {
                assert(ll(this->links_, used_links_count_));
                assert(ll(r->links_, r->used_links_count_));
                int move_key = r->keys_[0];
                int lower_key = r->parent_->keys_[r->child_index_ - 1];
                r->parent_->keys_[r->child_index_ - 1] = move_key;
                BTNode *move_link = r->links_[0];
                int tmp = -2;

                for (int i = 1; i < r->used_links_count_ - 1; ++i) {
                    r->keys_[i - 1] = r->keys_[i];
                }
                for (int i = 1; i < r->used_links_count_; ++i) {
                    r->set_link(i - 1, r->links_[i]);
                }
                r->used_links_count_--;

                keys_[used_links_count_ - 1] = lower_key;
                set_link(used_links_count_, move_link);
                used_links_count_++;
                int aaaa = -2;
                erase_(idx, aaaa);
                assert(aaaa == -1);
                assert(cc(this->keys_, this->key_count()));
                assert(cc(r->keys_, r->key_count()));
                assert(this->keys_[this->key_count() - 1] < r->parent_->keys_[r->child_index_ - 1]);
                assert(r->keys_[0] >= r->parent_->keys_[r->child_index_ - 1]);
                assert(ll(this->links_, used_links_count_));
                assert(ll(r->links_, r->used_links_count_));
            } else {
                if (l) {
                    assert(ll(l->links_, l->used_links_count_));
                    assert(cc(l->keys_, l->key_count()));
                    assert(ll(this->links_, this->used_links_count_));
                    assert(cc(this->keys_, this->key_count()));

                    int lower_key = parent->keys_[l->child_index_];
                    int aa = l->used_links_count_ - 1;
                    l->keys_[aa++] = lower_key;
                    for (int i = 0; i < key_count(); ++i) {
                        if (i == idx)
                            continue;
                        l->keys_[aa++] = keys_[i];
                    }
                    aa = l->used_links_count_;
                    for (int i = 0; i < used_links_count_; ++i) {
                        if (i == idx + 1)
                            continue;
                        l->set_link(aa++, links_[i]);
                    }
                    l->used_links_count_ = used_links_count_ - 1 + l->used_links_count_;
                    out_idx = l->child_index_;
                    assert(ll(l->links_, l->used_links_count_));
                    assert(cc(l->keys_, l->key_count()));
                    return parent;
                } else if (r) {
                    int upper_key = parent->keys_[this->child_index_];
                    for (int i = idx + 1; i < used_links_count_ - 1; ++i) {
                        keys_[i - 1] = keys_[i];
                    }
                    for (int i = idx + 2; i < used_links_count_; ++i) {
                        set_link(i - 1, links_[i]);
                    }
                    used_links_count_--;
                    assert(ll(this->links_, this->used_links_count_));
                    assert(cc(this->keys_, this->key_count()));
                    int aa = used_links_count_ - 1;
                    keys_[aa++] = upper_key;
                    for (int i = 0; i < r->key_count(); ++i) {
                        keys_[aa++] = r->keys_[i];
                    }
                    aa = used_links_count_;
                    for (int i = 0; i < r->used_links_count_; ++i) {
                        set_link(aa++, r->links_[i]);
                    }
                    used_links_count_ = used_links_count_ + r->used_links_count_;
                    out_idx = child_index_;
                    assert(ll(this->links_, this->used_links_count_));
                    assert(cc(this->keys_, this->key_count()));
                    return parent;
                } else {
                    assert(parent_ == nullptr);
                    out_idx = idx;
                }
            }
        }
        return parent;
    }

    void BTInnerNode::set_link(int idx, BTNode *link) {
        //assert(0 <= idx && idx < used_links_count_);

        links_[idx] = link;
        link->parent_ = this;
        link->child_index_ = idx;
    }


#ifdef BROTHER_ALGO1

    void BTInnerNode::resolve_brother() {
        /* do nothing */
    }

#else
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
#endif

}
