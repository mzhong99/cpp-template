#ifndef __SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include <exception>
#include <atomic>

#include <string>

#include <stdio.h>
#include <cxxabi.h>

#include <spdlog/spdlog.h>

template <typename T>
class Subsystem {
 private:
    bool _has_parallel_thread = false;
    std::thread _thread;

 protected:
    std::atomic<bool> _running = false;

    void UseWorkerThread() {
        _has_parallel_thread = true;
    }

 public:
    Subsystem() = default;
    Subsystem(const Subsystem<T> &rhs) = delete;

    std::string GetSubsystemName() {
        int status;
        std::string name = typeid(T).name();

        char *demangled = abi::__cxa_demangle(name.c_str(), NULL, NULL, &status);
        if (0 == status) {
            name = demangled;
            free(demangled);
        }

        return name;
    }

    virtual void Start() {
        _running = true;

        if (_has_parallel_thread) {
            _thread = std::thread([this] { dynamic_cast<T*>(this)->ThreadFunction(); });
            spdlog::info("[{}] opened thread", GetSubsystemName());
        }
    }

    virtual void Stop() {
        _running = false;

        if (_has_parallel_thread) {
            _thread.join();
        }
    }

    virtual void ThreadFunction() {
        spdlog::warn("Subsystem created without implementing thread function");
    }
};

#endif  // __MODULE_H__
