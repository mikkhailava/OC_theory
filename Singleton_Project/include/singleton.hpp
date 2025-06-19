#pragma once

#include <iostream>
#include <mutex>

struct SingleThreaded {
    struct Lock {
        Lock() {}
    };
};

// Стратегия синхронизации: многопоточная
struct MultiThreaded {
    static std::mutex& GetMutex() {
        static std::mutex mtx;
        return mtx;
    }

    struct Lock {
        Lock() { GetMutex().lock(); }
        ~Lock() { GetMutex().unlock(); }
    };
};

// Стратегия создания
template <typename T>
struct CreateUsingNew {
    static T* Create() {
        return new T();
    }
};

// Стратегия уничтожения
struct NoDestroy {
    template <typename T>
    static void Destroy(T*) {
        // Ничего не делаем
    }
};

// Шаблон Singleton
template <
    typename T,
    template <typename> class CreationPolicy = CreateUsingNew,
    typename ThreadingPolicy = SingleThreaded,
    typename DestructionPolicy = NoDestroy
>
class Singleton {
public:
    static T& Instance() {
        typename ThreadingPolicy::Lock lock;
        if (!_instance) {
            _instance = CreationPolicy<T>::Create();
        }
        return *_instance;
    }

private:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T* _instance;
};

template <typename T, template <typename> class CP, typename TP, typename DP>
T* Singleton<T, CP, TP, DP>::_instance = nullptr;
