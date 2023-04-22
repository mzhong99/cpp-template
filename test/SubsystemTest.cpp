#include <doctest.h>

#include "Subsystem.h"

class NonThreadedSubsystem : public Subsystem<NonThreadedSubsystem> {
 public:
    int _a, _b, _c;

    NonThreadedSubsystem(int a, int b, int c) {
        _a = a;
        _b = b;
        _c = c;
    }

    void Stop() {
        _a = 0;
        _b = 0;
        _c = 0;
    }
};

class ThreadedSubsystem : public Subsystem<ThreadedSubsystem> {
 public:
    int _count;
    int _dc;

    ThreadedSubsystem(int init_count) {
        _count = init_count;
        UseWorkerThread();
    }

    void Stop() override {
        _dc = 4000;
        Subsystem<ThreadedSubsystem>::Stop();
    }

    void ThreadFunction() override {
        while (_running || _count < 2000) {
            _count++;
        }
    }
};

TEST_CASE("Subsystem_NonThreaded") {
    NonThreadedSubsystem subsystem(1, 2, 3);
    subsystem.Start();

    REQUIRE(subsystem._a == 1);
    REQUIRE(subsystem._b == 2);
    REQUIRE(subsystem._c == 3);

    subsystem.Stop();

    REQUIRE(subsystem._a == 0);
    REQUIRE(subsystem._b == 0);
    REQUIRE(subsystem._c == 0);
}

TEST_CASE("Subsystem_Threaded") {
    ThreadedSubsystem subsystem(5);
    subsystem.Start();
    subsystem.Stop();

    REQUIRE(subsystem._count >= 2000);
    REQUIRE(subsystem._dc == 4000);
}

