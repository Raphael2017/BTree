#ifndef ARR_UTILS_H
#define ARR_UTILS_H

#include <assert.h>

struct ArrUtils
{
    template <typename T>
    void insert_at(T* src, size_t sz, T ele, size_t pos)
    {
        assert(0 <= pos && pos < sz);
        size_t i = 0;
        for (i = sz - 1; i >= 0; --i)
        {
            if (i >= pos)
                src[i+1] = src[i];
            else
                break;
        }
        src[pos] = ele;
    }

    template <typename T>
    void delete_at(T* src, size_t sz, size_t pos, T& out_ele)
    {
        assert(0 <= pos && pos < sz);
        out_ele = src[pos];
        for (size_t i = pos + 1; i < sz; ++i)
            src[i-1] = src[i];
    }

    template <typename T>
    bool insert_if(T* src, size_t sz, T ele, bool(*i_f)(T t, size_t index))
    {
        size_t i = 0;
        bool flag = false;
        for (i = sz - 1; sz >= 0; --i)
        {
            if (!i_f(src[i], i))
                src[i+1] = src[i];
            else
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            src[i+1] = ele;
        }
        return flag;
    }

    template <typename T>
    bool delete_if(T* src, size_t sz, bool(*i_f)(T t, size_t index), T& out_ele)
    {
        size_t i = 0;
        bool flag = false;
        for (i = 0; i < sz; ++i)
        {
            if (i_f(src[i], i))
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            out_ele = src[i];
            for (size_t j = i+1; j < sz; ++j)
                src[j-1] = src[j];
        }
        return flag;
    }

};


#endif