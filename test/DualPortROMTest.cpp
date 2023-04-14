#include <doctest.h>

#include <vector>

#include "ModuleTester.h"

#include "VDualPortROM.h"
#include "VDualPortROM___024root.h"

TEST_CASE("DualPortROMTest") {
    ModuleTester<VDualPortROM> tester;
    std::vector<uint8_t> reference_data = {0x04, 0x20, 0xD4, 0x38, 0x0F, 0xE1, 0x6E, 0xC7};

    for (uint8_t addrA = 0; addrA < (1 << 3); addrA++) {
        for (uint8_t addrB = 0; addrB < (1 << 3); addrB++) {
            tester.GetDUT().addrA = addrA;
            tester.GetDUT().addrB = addrB;
            tester.AdvanceCombinational();

            CHECK(tester.GetDUT().qA == reference_data[addrA]);
            CHECK(tester.GetDUT().qB == reference_data[addrB]);
        }
    }
}
