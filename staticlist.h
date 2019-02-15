#pragma once
#include "avr.h"

template <class T>
class ConstList
{
public:
	ConstList(const int max_size_);

	// getter
	T& itemAt(int index);
	inline T& operator[](int index){itemAt(index);}
	T& first();
	T& last();

	// expanding functions
	void append(const T &item);
	void insert(const unsigned int index, const T &item);
	inline void prepent(const T &item);
	inline ConstList<T>& operator<<(T& item);

	void shift(const unsigned int index=0); // shifts all items by one, indirection up
	void backshift(const unsigned int index=0); // shifst all items byte one, in direction down

	inline void removeFirst();
	void remove(const unsigned int index);
	inline void removeLast();
	void removeAll();
	inline ConstList<T>& operator>>(T& item);

	struct Iterator{
		Iterator(T*item):item_(item){}
		T * item_;
		bool operator!=(Iterator compare){return item_ != compare.item_;}
		T& operator*(){return *item_;}
		void operator++(){item_++;}
	};

	Iterator begin(){return Iterator(first_);}
	Iterator end(){return Iterator(first_ + max_size_ - 1);}

	Iterator begin() const {return Iterator(first_);}
	Iterator end() const{return Iterator(first_ + max_size_ - 1);}

	// info functions
	inline int maxSize() const {return max_size_;}
	inline const int size() const { return size_;}

	inline bool isEmpty() const {return size_ == 0;}
	inline bool isFull() const {return size_ == max_size_;}
	inline int remainingSpace() const {return max_size_ - size_;}

private:
	const int max_size_;
	int size_;
	T* first_;
};

template<class T>
ConstList<T>::ConstList(const int size) : max_size_(size)
{
	first_ = static_cast<T*>(malloc(sizeof(T)*max_size_));
}

template<class T>
T &ConstList<T>::itemAt(int index)
{
	if(index >= max_size_)
		return nullptr;
	return first_[index];
}

template<class T>
T &ConstList<T>::first()
{
	return *first_;
}

template<class T>
T &ConstList<T>::last()
{
	return first_[max_size_-1];
}

template<class T>
void ConstList<T>::append(const T &item)
{
	if(size_ + 1 > max_size_)
		return;
	first_[size] = item;
	size++;
	return;
}

template<class T>
void ConstList<T>::insert(const unsigned int index, const T &item)
{
	if(size_ + 1 > max_size_)
		return;
	shift(index);
	first_[index] = item;
}

template<class T>
void ConstList<T>::prepent(const T &item)
{
	insert(0,item);
}

template<class T>
ConstList<T> &ConstList<T>::operator<<(T &item)
{
	append(item);
	return *this;
}

	// shifts items to the end of the list
template<class T>
void ConstList<T>::shift(const unsigned int index)
{
	if(size_ + 1 > max_size_)
		return;
	if(index >= size_)
		return;
	for(T* tmp = first_[size]; tmp > first_[index]; tmp--){
		*tmp = *(tmp-1);
	}
	size_++;
}

	// shifts item to the begin of the list
template<class T>
void ConstList<T>::backshift(const unsigned int index)
{
	if(index >= size_ || size > 0)
		return;
	for(T* tmp = first_[index]; tmp <= first_[size-2]; tmp++){
		*tmp = *(tmp+1);
	}
	first_[size_] = T(); // reset last
	size_--;
}

template<class T>
void ConstList<T>::removeFirst()
{
	backshift();
}

template<class T>
void ConstList<T>::remove(const unsigned int index)
{
	backshift(index);
}

template<class T>
void ConstList<T>::removeLast()
{
	--size_;
	first_[size_] = T();
}

template<class T>
void ConstList<T>::removeAll()
{
	for(; size_ >= 0; --size_)
		first_[size_] = T();{
	}
}

template<class T>
ConstList<T> &ConstList<T>::operator>>(T &item)
{
	item = last();
	removeLast();
	return *this;

}
