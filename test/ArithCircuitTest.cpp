#include <doctest.h>

#include "ModuleTester.h"

#include "VArithCircuit.h"
#include "VArithCircuit___024root.h"

TEST_CASE("ArithCircuitTest") {
    ModuleTester<VArithCircuit> tester;

    for (uint64_t opsel = 0; opsel < (1 << 3); opsel++) {
        for (uint64_t OpA = 0; OpA < UINT8_MAX; OpA++) {
            for (uint64_t OpB = 0; OpB < UINT8_MAX; OpB++) {
                const uint8_t EXPECTED_ANSWER[8] = {
                    [0x0] = (uint8_t)(OpA),
                    [0x1] = (uint8_t)(OpA + OpB),
                    [0x2] = (uint8_t)(OpA - OpB),
                    [0x3] = (uint8_t)(OpB - OpA),
                    [0x4] = (uint8_t)(-OpA),
                    [0x5] = (uint8_t)(OpA + 1u),
                    [0x6] = (uint8_t)(OpA - 3u),
                    [0x7] = (uint8_t)(OpB + 2u),
                };

                tester.GetDUT().OpA = OpA;
                tester.GetDUT().OpB = OpB;
                tester.GetDUT().opselect = opsel;

                tester.AdvanceCombinational();
                CHECK(tester.GetDUT().result == EXPECTED_ANSWER[opsel]);
            }
        }
    }
}
