#include <doctest.h>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "ModuleTester.h"
#include "MockTopModule.h"

#include "VTopModule.h"
#include "VTopModule___024root.h"

#include "VDualPortROM.h"
#include "VDualPortROM___024root.h"

static uint8_t ComputeBCD(uint8_t binary) {
    return (((binary >> 4u) & 0x0fu) * 10u) + (binary & 0x0fu);
}

TEST_CASE("TopModuleTest") {
    ModuleTester<VTopModule> tester;
    ModuleTester<VDualPortROM> rom_lookup("/tmp/rom.vcd");
    MockTopModule mock("config/specification.json");

    for (uint8_t addrA = 0; addrA < 8; addrA++) {
        for (uint8_t addrB = 0; addrB < 8; addrB++) {
            for (uint8_t opselect = 0; opselect < 8; opselect++) {
                tester.GetDUT().addrA = addrA;
                tester.GetDUT().addrB = addrB;
                tester.GetDUT().opselect = opselect;

                rom_lookup.GetDUT().addrA = addrA;
                rom_lookup.GetDUT().addrB = addrB;

                tester.AdvanceCombinational();
                rom_lookup.AdvanceCombinational();

                uint8_t opA = rom_lookup.GetDUT().qA;
                uint8_t opB = rom_lookup.GetDUT().qB;
                uint8_t dut_result = tester.GetDUT().result;
                uint8_t expected_result;

                REQUIRE_NOTHROW(expected_result = mock.Evaluate(opselect, opA, opB));
                CHECK(expected_result == dut_result);
            }
        }
    }
}

TEST_CASE("TopModuleTest_Last4PID") {
    ModuleTester<VTopModule> tester("last4pid_wavefrom.vcd");
    MockTopModule mock("config/specification.json");

    tester.GetDUT().opselect = 0;
    tester.GetDUT().addrA = 0;
    tester.AdvanceCombinational();


    uint8_t high_bcd = ComputeBCD(tester.GetDUT().result);

    tester.GetDUT().opselect = 0;
    tester.GetDUT().addrA = 1;
    tester.AdvanceCombinational();
    uint8_t low_bcd = ComputeBCD(tester.GetDUT().result);

    REQUIRE_NOTHROW(mock.ValidateLast4PIDFromBCD(high_bcd, low_bcd));
}

