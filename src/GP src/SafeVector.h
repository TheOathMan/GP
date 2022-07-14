#pragma once

#include <thread>
#include <vector>
#include <mutex>

template <class T>
class SafeVector {
public:

	SafeVector() = default;

	using vitr = typename std::vector<T>::iterator;
	void push_back(T item);
	void clear();
	const int size();
	const bool empty();
	T& at(int index);
	T& back();
	std::vector<T>& toVector();
	void for_each(void (*fe_func)(int,T&));

	void operator=(const std::vector<T>& other) { vec = other; }
	
private:
	std::vector<T> vec;
	std::mutex mut;
};



template<typename T>
inline std::vector<T>& SafeVector<T>::toVector()
{
	//std::lock_guard<std::mutex> lock(mut);
	return vec;
}

template<class T>
inline void SafeVector<T>::for_each(void(*fe_func)(int,T&))
{
	std::lock_guard<std::mutex> lock(mut);
	for (size_t i = 0; i < vec.size(); i++)
	{
		fe_func(i, vec.at(i) );
	}
}

template<typename T>
inline void SafeVector<T>::push_back(T item)
{
	std::lock_guard<std::mutex> lock(mut);
	vec.push_back(item);
}

template<typename T>
inline void SafeVector<T>::clear()
{
	std::lock_guard<std::mutex> lock(mut);
	vec.clear();
}

template<typename T>
inline const int SafeVector<T>::size()
{
	return  vec.size();
}

template<typename T>
inline const bool SafeVector<T>::empty()
{
	return vec.empty();
}

template<typename T>
inline T& SafeVector<T>::at(int index)
{
	return vec.at(index);
}

template<typename T>
inline T& SafeVector<T>::back()
{
	return vec.back();
}

