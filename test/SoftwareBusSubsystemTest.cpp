#include <doctest.h>
#include <spdlog/spdlog.h>

#include <string>

#include "Subsystem.h"
#include "SoftwareBusSubsystem.h"

void TestConsumerSubsystemCallback(void *context, const SoftwareBusMessage &message);

class TestConsumerSubsystem : public Subsystem<TestConsumerSubsystem> {
 private:
    MessageQueue<std::string> _mqueue;
    std::string _endpoint;

 public:
    std::atomic<int> _rx_count;

    TestConsumerSubsystem(const std::string &endpoint, SoftwareBusSubsystem *software_bus) {
        _endpoint = endpoint;
        _rx_count = 0;
        software_bus->Subscribe(endpoint, this, TestConsumerSubsystemCallback);
        UseWorkerThread();
    }

    void ThreadFunction() override {
        while (_running) {
            std::string line;
            if (_mqueue.Pop(line, 0ms)) {
                if (line == _endpoint) {
                    _rx_count++;
                }
            }
        }
    }

    void ConsumeMessage(const SoftwareBusMessage &message) {
        std::string line((const char *)message._payload.data(), message._payload.size());
        _mqueue.Push(line);
    }
};

void TestConsumerSubsystemCallback(void *context, const SoftwareBusMessage &message) {
    reinterpret_cast<TestConsumerSubsystem *>(context)->ConsumeMessage(message);
}

TEST_CASE("SoftwareBusSubsystem_InitStartStop") {
    SoftwareBusSubsystem software_bus;
    software_bus.Start();
    software_bus.Stop();
}

TEST_CASE("SoftwareBusSubsystem_SingleProducerMultiConsumer") {
    // SoftwareBusSubsystem software_bus;
    // software_bus.Start();

    // TestConsumerSubsystem consumer1("/endpoints/1", &software_bus);
    // TestConsumerSubsystem consumer2("/endpoints/2", &software_bus);
    // TestConsumerSubsystem consumer3("/endpoints/3", &software_bus);
    // TestConsumerSubsystem consumer4("/endpoints/4", &software_bus);

    // consumer1.Start();
    // consumer2.Start();
    // consumer3.Start();
    // consumer4.Start();

    // for (int i = 0; i < 50000; i++) {
    //     for (int j = 1; j <= 4; j++) {
    //         std::string endpoint = fmt::format("/endpoints/{}", j);
    //         software_bus.Publish(endpoint, (const char *)endpoint.c_str(), endpoint.length());
    //     }
    // }

    // auto all_received = [&] (int threshold) {
    //     return consumer1._rx_count == threshold && consumer2._rx_count == threshold
    //         && consumer3._rx_count == threshold && consumer4._rx_count == threshold;
    // };

    // while (!all_received(50000)) {
    //     continue;
    // }

    // consumer4.Stop();
    // consumer3.Stop();
    // consumer2.Stop();
    // consumer1.Stop();

    // software_bus.Stop();

    // REQUIRE(all_received(50000));
}
