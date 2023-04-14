#ifndef __MODULE_TESTER_H__
#define __MODULE_TESTER_H__

#include <string>
#include <cxxabi.h>
#include <iostream>
#include <filesystem>

#include <verilated.h>
#include <verilated_vcd_c.h>

#define MAX_SIM_DEPTH   (5)
#define SIM_STEP        (1)

template <typename VModuleName>
class ModuleTester {
 private:
    std::string _output_path;
    VModuleName *_dut;
    VerilatedVcdC *_vcd_trace;
    vluint64_t _sim_time = 0;

 public:
    ModuleTester(const std::string &output_filename = "default", size_t max_sim_depth = MAX_SIM_DEPTH) {
        Verilated::traceEverOn(true);
        _vcd_trace = new VerilatedVcdC();
        _dut = new VModuleName();

        std::filesystem::path filename = output_filename;
        if (output_filename == "default") {
            int status;
            char *demangled = abi::__cxa_demangle(typeid(*_dut).name(), 0, 0, &status);

            filename = std::filesystem::path(std::string(demangled) + "_waveform.vcd");
            free(demangled);
        }

        std::filesystem::path directory = WAVEFORM_DIR;
        std::filesystem::path fullpath = directory / filename;
        _output_path = fullpath.string();

        _dut->trace(_vcd_trace, max_sim_depth);
        std::cout << "FILE: " << _output_path << std::endl;
        _vcd_trace->open(_output_path.c_str());
    }

    ModuleTester(const ModuleTester<VModuleName> &) = delete;

    ~ModuleTester() {
        _vcd_trace->close();
        delete _dut;
    }

    VModuleName &GetDUT(void) {
        return *_dut;
    }

    void AdvanceCombinational(void) {
        _dut->eval();
        _vcd_trace->dump(_sim_time);
        _sim_time += SIM_STEP;
    }
};

#endif  // __MODULE_TESTER_H__
