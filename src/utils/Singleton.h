#pragma once
#include <mutex>

template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (!_instance)
		{
			std::lock_guard<std::mutex> locker(lock);
			if (!_instance)
			{
				_instance = new T();
			}
		}
		return _instance;
	}
	~Singleton()
	{
		if (_instance)
			delete _instance;
	}
private:
	static T* _instance;
	static std::mutex lock;
};
template <typename T>
T* Singleton<T>::_instance{ nullptr };

template <typename T>
std::mutex Singleton<T>::lock;