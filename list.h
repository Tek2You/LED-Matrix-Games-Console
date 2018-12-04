#pragma once
#include "operators.h"
template<class t>
class List
{
private:
	 template<class td>
	 class ListNote;
public:
	 List();
	 ~List();
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
	 template <class TN>
	 struct ListNote{
		  TN data_;
		  ListNote<TN> * next_ = nullptr;
	 };
	 ListNote<t> *dataAt(const unsigned int &i) const;
	 ListNote<t> *lastData() const;
	 ListNote<t> * root_ = nullptr;
	 unsigned int size_;
};

template<class t>
List<t>::List()
{
	root_ = nullptr;
	size_ = 0;
}

template<class t>
List<t>::~List()
{
	while(size_){
		remove(size_-1);
	}
}

template<class t>
void List<t>::append(const t &item)
{
	 ListNote<t> * d = lastData();
	 if(!d)
		  return;
	 d->next_ = new ListNote<t>;
	 d->next_->data_ = item;
	 size_++;
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
	 ListNote<t> * data = dataAt(i-1);
	 if(!data)
		  return;
	 ListNote<t> * new_data = new ListNote<t>;
	 new_data->next_ = data;
	 data->data_ = item;
	 data = new_data;
	 size_++;
}

template<class t>
void List<t>::prepent(const t &item)
{
	 insert(0,item);
}

template<class t>
void List<t>::remove(const unsigned int &i)
{
	 ListNote<t> * data = dataAt(i);
	 if(!data)
		  return;
	 ListNote<t> * next = data->next_;
	 delete data;
	 data = next;
	 size_--;
}

template<class t>
t& List<t>::itemAt(const unsigned int &i) const
{
	 ListNote<t> * d = dataAt(i);
	 if(d){
		  return d->data_;
	 }
	 return nullptr;
}

template<class t>
t &List<t>::first() const
{
	 if(root_){
		  return root_->data_;
	 }
	 return nullptr;
}

template<class t>
t &List<t>::last() const
{
	 ListNote<t> *d = lastData();
	 if(d){
		  return d;
	 }
	 return nullptr;
}

template<class t>
List<t>::ListNote<t> *List<t>::dataAt(const unsigned int &i) const
{
	 if(i >= size() - 1)
		  return nullptr;
	 int count = 0;
	 ListNote<t> *d = root_;
	 while(count++ < i && d){
		  d = d->next_;
	 }
	 return d;
}

template<class t>
List<t>::ListNote<t> *List<t>::lastData() const
{
	 return dataAt(size()-1);
}
