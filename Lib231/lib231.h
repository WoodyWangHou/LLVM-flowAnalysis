#ifndef LIB_H
#define LIB_H
const char *mapCodeToName(unsigned Op);

extern "C" __attribute__((visibility("default")))
void updateInstrInfo(unsigned num, uint32_t * keys, uint32_t * values);

extern "C" __attribute__((visibility("default")))
void updateBranchInfo(bool taken);

extern "C" __attribute__((visibility("default")))
void printOutInstrInfo();

extern "C" __attribute__((visibility("default")))
void printOutBranchInfo();
#endif

