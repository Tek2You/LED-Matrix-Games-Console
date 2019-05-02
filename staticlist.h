#pragma once
#include "avr.h"

template <class T>
class StaticList
{
public:
	StaticList(const int max_size_);
	~StaticList();

	// getter
	T &itemAt(const int index);
	T &first();
	T &last();

	inline T &operator[](const int index) { return itemAt(index); }

	inline T* toArray() const;

	// expanding functions
	void append(const T &item);
	void insert(const unsigned int index, const T &item);
	inline void prepent(const T &item);
	inline StaticList<T> &operator<<(const T &t);

	void shift(const unsigned int index = 0);     // shifts all items by one, indirection up
	void backshift(const unsigned int index = 0); // shifst all items byte one, in direction down

	inline void removeFirst();
	void remove(const unsigned int index);
	inline void removeLast();
	void removeAll();
	inline StaticList<T> &operator>>(T &item);

	// info functions
	inline int maxSize() const { return max_size_; }
	inline const int size() const { return size_; }

	inline bool isEmpty() const { return size_ == 0; }
	inline bool isFull() const { return size_ == max_size_; }
	inline int remainingSpace() const { return max_size_ - size_; }

	struct Iterator
	{
		Iterator(T *item) : item_(item) {}
		T *item_;
		bool operator!=(Iterator compare) { return item_ != compare.item_; }
		T &operator*() { return *item_; }
		void operator++() { item_++; }
	};

	Iterator begin() { return Iterator(first_); }
	Iterator end() { return Iterator(first_ + size_); }

	Iterator begin() const { return Iterator(first_); }
	Iterator end() const { return Iterator(first_ + size_); }

private:
	const int max_size_;
	int size_;
	T *first_;
};

template <class T>
StaticList<T>::StaticList(const int size) : max_size_(size), size_(0)
{
	first_ = static_cast<T *>(malloc(sizeof(T) * max_size_));
	for(int i = 0; i < max_size_; i++){
		first_[i] = T();
	}
}

template <class T>
StaticList<T>::~StaticList()
{
	removeAll();
}

template <class T>
T &StaticList<T>::itemAt(const int index)
{
	if (index < size_) return first_[index];
}

template <class T>
T &StaticList<T>::first()
{
	return *first_;
}

template<class T>
T *StaticList<T>::toArray() const
{
	return first_;
}

template <class T>
T &StaticList<T>::last()
{
	return first_[size_ - 1];
}

template <class T>
void StaticList<T>::append(const T &item)
{
	if (size_ + 1 > max_size_) return;
	first_[size_] = item;
	size_++;
	return;
}

template <class T>
void StaticList<T>::insert(const unsigned int index, const T &item)
{
	if (size_ + 1 > max_size_) return;
	shift(index);
	first_[index] = item;
}

template <class T>
void StaticList<T>::prepent(const T &item)
{
	insert(0, item);
}

template <class T>
StaticList<T> &StaticList<T>::operator<<(const T &t)
{
	append(t);
	return *this;
}

// shifts items to the end of the list
template <class T>
void StaticList<T>::shift(const unsigned int index)
{
	if (size_ + 1 > max_size_) return;
	if (index >= size_) return;
	for (T *tmp = first_ + size_; tmp > first_ + index; tmp--)
	{
		*tmp = *(tmp - 1);
	}
	size_++;
}

// shifts item to the begin of the list
template <class T>
void StaticList<T>::backshift(const unsigned int index)
{
	if (index >= size_ || size_ < 1) return;
	for (T *tmp = first_ + index; tmp <= first_ + size_ - 2; tmp++)
	{
		*tmp = *(tmp + 1);
	}
	first_[size_] = T(); // reset last
	size_--;
}

template <class T>
void StaticList<T>::removeFirst()
{
	remove(0);
}

template <class T>
void StaticList<T>::remove(const unsigned int index)
{
	backshift(index);
}

template <class T>
void StaticList<T>::removeLast()
{
	--size_;
	first_[size_] = T();
}

template <class T>
void StaticList<T>::removeAll()
{
	for (; size_ >= 0; --size_) first_[size_] = T();
	{
	}
}

template <class T>
StaticList<T> &StaticList<T>::operator>>(T &item)
{
	item = last();
	removeLast();
	return *this;
}
