#include "checkSumUtil.h"

//streamSize is measured in bytes
uint16_t CheckSumUtil::computeSum(void* bytestream, int streamSize) {
    uint16_t sum = 0;
    for (void* it = bytestream; it < static_cast<uint8_t*>(bytestream) + streamSize; it = static_cast<uint8_t*>(it) + 1) {
        // read high byte
        uint16_t nextValue = 0x0000;
        nextValue |= (*static_cast<uint8_t*>(it) << 8);
        // read low byte, if there is one
        it = static_cast<uint8_t*>(it) + 1;
        if (it < static_cast<uint8_t*>(bytestream) + streamSize) {
            nextValue |= (*static_cast<uint8_t*>(it));
        }
        // Carry the 1 if we overflow the counter
        if (static_cast<uint32_t>(nextValue + sum) > 0xFFFF) {
            sum += 1;
        }
        sum += nextValue;
    }
    return sum;
}

bool CheckSumUtil::checkChecksum(int16_t checksum, void* bytestream, int streamSize) {
    uint16_t sum = computeSum(bytestream, streamSize);
    if (sum + checksum == 0xFFFF) return true;
    return false;
}
