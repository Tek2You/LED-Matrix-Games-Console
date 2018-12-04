#pragma once
#include "operators.h"
template<class t>
class List
{
private:
    template<class td>
    class Data;
public:
    List();
    void append(const t &item);
    void insert(const unsigned int &i, const t &item);
    inline void prepent(const t &item);
    void remove(const unsigned int &i);

    inline bool &isEmpty() const {return size() == 0;}
    int &size() const {return size_;}

    t &itemAt(const unsigned int &i) const;
    t &first() const;
    t &last() const;
private:
    template <class td>
    struct Data{
        td data_;
        Data * next_ = nullptr;
    };
    Data<t> *dataAt(const unsigned int &i) const;
    Data<t> *lastData() const;
    Data<t> * first_ = nullptr;
    unsigned int size_;
};

template<class t>
void List<t>::append(const t &item)
{
    Data<t> * d = lastData();
    if(!d)
        return;
    d->next_ = new Data<t>;
    d->next_->data_ = item;
}

template<class t>
void List<t>::insert(const unsigned int &i,const t &item)
{
    if(i == size()){
        append(i);
        return;
    }
    if(i > size())
        return;
    Data<t> * data = dataAt(i-1);
    if(!data)
        return;
    Data<t> * new_data = new Data<t>;
    new_data->next_ = data;
    data->data_ = item;
    data = new_data;
}

template<class t>
void List<t>::prepent(const t &item)
{
    insert(0,item);
}

template<class t>
void List<t>::remove(const unsigned int &i)
{
    Data<t> * data = dataAt(i);
    if(!data)
        return;
    Data<t> * next = data->next_;
    delete data;
    data = next;
}

template<class t>
t& List<t>::itemAt(const unsigned int &i) const
{
    Data<t> * d = dataAt(i);
    if(d){
        return d->data_;
    }
    return nullptr;
}

template<class t>
t &List<t>::first() const
{
    if(first_){
        return first_->data_;
    }
    return nullptr;
}

template<class t>
t &List<t>::last() const
{
    Data<t> *d = lastData();
    if(d){
        return d;
    }
    return nullptr;
}

template<class t>
List<t>::Data<t> *List<t>::dataAt(const unsigned int &i) const
{
    if(i >= size() - 1)
        return nullptr;
    int count = 0;
    Data<t> *d = first_;
    while(count++ < i && d){
        d = d->next_;
    }
    return d;
}

template<class t>
List<t>::Data<t> *List<t>::lastData() const
{
    return dataAt(size()-1);
}
