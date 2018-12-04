#pragma once
#include "operators.h"
template<class T>
class List
{
private:
	template<class Td>
	class ListNode;
public:
	List();
	~List();
	void append(const T &item);
	void insert(const unsigned int &index, const T &item);
	void prepent(const T &item);

	T &removeFirst();
	T &remove(const unsigned int &index);
	T &removeLast();

	inline bool &isEmpty() const {return size() == 0;}
	inline int &size() const {return size_;}

	T &itemAt(const unsigned int &index) const;
	T &first() const;
	T &last() const;
	T &operator[](const unsigned int &index);
private:
	template <class TN>
	struct ListNode{
		TN data_;
		ListNode<TN> * next_ = nullptr;
	};

	ListNode<T> *nodeAt(const unsigned int &index) const;
	ListNode<T> * root_ = nullptr;
	ListNode<T> * last_ = nullptr;
	unsigned int size_;
};

template<class T>
List<T>::List()
{
	root_ = nullptr;
	size_ = 0;
}

template<class T>
List<T>::~List()
{
	while(size_){
		remove(size_-1);
	}
}

template<class T>
void List<T>::append(const T &item)
{
	ListNode<T> *tmp = new ListNode<T>();
	tmp->data_ = item;
	tmp->next_ = nullptr;

	if(root_){
		// Already have elements inserted
		last_->next_ = tmp;
		last_ = tmp;
	}else{
		// First element being inserted
		root_ = tmp;
		last_ = tmp;
	}

	size_++;
}

template<class T>
void List<T>::insert(const unsigned int &index,const T &item)
{
	if(index >= size_)
		return append(item);

	if(index == 0)
		return prepent(item);

	ListNode<T> *tmp = new ListNode<T>(), *prev = nodeAt(index-1);
	tmp->data_ = item;
	tmp->next_ = prev->next_;
	prev->next_ = tmp;

	size_++;
}

template<class T>
void List<T>::prepent(const T &item)
{
	if(size_ == 0)
		return append(item);

	ListNode<T> *tmp = new ListNode<T>();
	tmp->next_ = root_;
	tmp->data_ = item;
	root_ = tmp;

	size_++;
}

template<class T>
T &List<T>::removeFirst(){
	if(size_ == 0)
		return T();

	if(size_ > 1){
		ListNode<T> *next = root_->next_;
		T ret = root_->data_;
		delete(root_);
		root_ = next;
		size_--;

		return ret;
	}else{
		return removeLast();
	}
}

template<typename T>
T &List<T>::removeLast(){
	if(size_ <= 0)
		return T();

	if(size_ > 1){
		ListNode<T> *tmp = nodeAt(size_ - 2);
		T ret = tmp->next_->data_;
		delete(tmp->next_);
		tmp->next_ = nullptr;
		last_ = tmp;
		size_--;
		return ret;
	}else{
		// Only one element left on the list
		T ret = root_->data_;
		delete(root_);
		root_ = nullptr;
		last_ = nullptr;
		size_ = 0;
		return ret;
	}
}

template<class T>
T &List<T>::remove(const unsigned int &index)
{
	if (index < 0 || index >= size_)
		return T();

	if(index == 0)
		return removeFirst();
	if (index == size_-1)
		return removeLast();

	ListNode<T> *tmp = nodeAt(index - 1);
	ListNode<T> *toDelete = tmp->next_;
	T ret = toDelete->data_;
	tmp->next_ = tmp->next_->next_;
	delete(toDelete);
	size_--;
	return ret;
}

template<class T>
T &List<T>::itemAt(const unsigned int &index) const
{
	ListNode<T> * d = nodeAt(index);
	return (d ? d : T());
}

template<class T>
T &List<T>::operator[](const unsigned int &index)
{
	return itemAt(index);
}

template<class T>
T &List<T>::first() const
{
	return itemAt(0);
}

template<class T>
T &List<T>::last() const
{
	ListNode<T> tmp = last_;
	return (tmp ? tmp.data_ : T());
}

template<class T>
List<T>::ListNode<T> *List<T>::nodeAt(const unsigned int &index) const
{
	int pos = 0;
	ListNode<T>* current = root_;

	while(pos < index && current){
		current = current->next_;
		pos++;
	}

	if(pos == index){
		return current;
	}
	return nullptr;
}
