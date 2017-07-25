#include "message.h"

using namespace std;

class CheckSumUtil{
public:
    uint16_t static computeSum(void* bytestream, int streamSize);
    bool static checkChecksum(uint16_t checksum, void* bytestream, int streamSize);
};