#pragma once

#include "MemoryReader.h"

#include <cstdint>

struct PlayDataSnapshot
{
    std::int32_t songId = 0;
    std::int32_t difficulty = 0;
    std::int32_t clearLamp = 0;
};

class PlayDataReader
{
public:
    explicit PlayDataReader(const MemoryReader& memoryReader);

    bool read(
        std::uintptr_t address,
        PlayDataSnapshot& snapshot) const;

private:
    const MemoryReader& memoryReader_;
};