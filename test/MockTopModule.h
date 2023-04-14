#ifndef __MOCK_TOP_MODULE_H__
#define __MOCK_TOP_MODULE_H__

#include <string>
#include <fstream>
#include <exception>
#include <functional>
#include <map>

#include <json.hpp>

#include <spdlog/spdlog.h>

class MockTopModule {
private:
    static constexpr uint16_t LAST4PID_MAX = 9999u;
    uint16_t _last4pid = UINT16_MAX;

    std::map<uint8_t, std::string> _instruction_decoder;
    std::map<std::string, std::function<uint8_t(uint8_t, uint8_t)>> ARITHMETIC_OPCODE_TABLE = {
        {"movA" , [] (uint8_t a, uint8_t b) { return a;      }},
        {"add"  , [] (uint8_t a, uint8_t b) { return a + b;  }},
        {"subAB", [] (uint8_t a, uint8_t b) { return a - b;  }},
        {"subBA", [] (uint8_t a, uint8_t b) { return b - a;  }},
        {"negA" , [] (uint8_t a, uint8_t b) { return -a;     }},
        {"incA" , [] (uint8_t a, uint8_t b) { return a + 1u; }},
        {"decA3", [] (uint8_t a, uint8_t b) { return a - 3u; }},
        {"negB" , [] (uint8_t a, uint8_t b) { return -b;     }},
        {"incB2", [] (uint8_t a, uint8_t b) { return b + 2u; }},
        {"decB2", [] (uint8_t a, uint8_t b) { return b - 2u; }},
    };

    void ParsePID(const nlohmann::json &specification) {
        std::string last4pid_str = specification["last4pid"];
        if (last4pid_str.length() != 4) {
            spdlog::warn("Last 4 digits of PID were never defined - tests will FAIL");
        } else if (!std::all_of(last4pid_str.begin(), last4pid_str.end(), [&] (char c) { return isdigit(c); })) {
            spdlog::warn("PID '{}' is not numeric - tests will FAIL", last4pid_str);
        } else {
            _last4pid = std::stoul(last4pid_str);
            spdlog::info("Parsed PID digits '{:04d}'", _last4pid);
        }
    }

public:
    MockTopModule(const std::string &filepath) {
        std::ifstream ifs(filepath);
        nlohmann::json specification = nlohmann::json::parse(ifs);

        ParsePID(specification);

        for (const auto &[opselect_str, arithmetic_str] : specification["operations"].items()) {
            uint8_t opselect = std::stoul(opselect_str, NULL, 0);
            _instruction_decoder[opselect] = arithmetic_str;
        }
    }

    void ValidateLast4PID(uint16_t last4pid_external) {
        if (_last4pid > LAST4PID_MAX) {
            throw std::runtime_error(fmt::format("No PID specified - assuming value is WRONG"));
        }

        if (last4pid_external != _last4pid) {
            std::string error_msg = fmt::format(
                "Test data has the WRONG PID - your PID is '{:04d}' but the test data has {:04d}",
                _last4pid, last4pid_external);
            spdlog::critical(error_msg);
            throw std::runtime_error(error_msg);
        }

        spdlog::info("PID digits '{:04d}' MATCH test data!", _last4pid);
    }

    void ValidateLast4PIDFromBCD(uint8_t high_bcd, uint8_t low_bcd) {
        uint16_t last4pid_external = (((uint16_t)high_bcd) * 100u) + ((uint16_t)low_bcd);
        ValidateLast4PID(last4pid_external);
    }

    uint8_t Evaluate(uint8_t opcode, uint8_t a, uint8_t b) {
        auto it = _instruction_decoder.find(opcode);
        if (it == _instruction_decoder.end()) {
            throw std::runtime_error(fmt::format("Opcode {} was not specified in specification.json", opcode));
        }
        return ARITHMETIC_OPCODE_TABLE[it->second](a, b);
    }
};

#endif  // __MOCK_TOP_MODULE_H__
