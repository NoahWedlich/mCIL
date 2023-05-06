#pragma once
#include "../../cil-system.h"

template <typename obj>
class ThreadSafeObj
{
public:
	template <typename... Args>
	ThreadSafeObj(Args&&... args);
	~ThreadSafeObj();

	obj* operator->();
private:
	class ProxyObj
	{
	public:
		ProxyObj(obj* object, std::mutex& lock);
		~ProxyObj();

		obj* object;
		std::unique_lock<std::mutex> lock;
	};

	std::mutex lock_;

	obj* object_;
};

template<typename obj>
template<typename... Args>
ThreadSafeObj<obj>::ThreadSafeObj(Args&&... args)
{
	object_ = new obj(std::forward<Args>(args)...);
}

template<typename obj>
ThreadSafeObj<obj>::~ThreadSafeObj()
{
	delete object_;
}

template<typename obj>
obj* ThreadSafeObj<obj>::operator->()
{
	return ProxyObj{ object_, lock_ }.object;
}

template<typename obj>
ThreadSafeObj<obj>::ProxyObj::ProxyObj(obj* object, std::mutex& lock)
	: object(object), lock(lock)
{}

template<typename obj>
ThreadSafeObj<obj>::ProxyObj::~ProxyObj()
{}

template <typename T>
using SafeVector = ThreadSafeObj<std::vector<T>>;

template <typename T>
using SafeQueue = ThreadSafeObj<std::queue<T>>;