#ifndef SINGLETON_H
#define SINGLETON_H
/******************************************************************************
 *
 * @file       singleton.h
 * @brief      单例模板类
 *
 * @author     Bigricecooker
 * @date       2024/11/14
 * @history
 *****************************************************************************/

#include "global.h"


template <typename T>
class Singleton {
protected:// 使子类能够调用
    Singleton()=default;
    Singleton(const Singleton<T>&)=delete;
    Singleton& operator = (const Singleton<T>&)=delete;

    static std::shared_ptr<T> _instance;

public:
    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);// 为什么不用make_shared?  继承模版单例类的子类的构造会设置为private，make_shared无法访问私有的构造函数
        });
        return _instance;
    }

    void PrintAddress() {
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance=nullptr;

#endif // SINGLETON_H
