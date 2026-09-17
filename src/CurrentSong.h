#pragma once

#include "MemoryReader.h"

#include <cstdint>

struct CurrentSongSnapshot
{
    std::int32_t songId = 0;
    std::int32_t difficulty = 0;
};

class CurrentSongReader
{
public:
    explicit CurrentSongReader(
        const MemoryReader &memoryReader);

    bool read(
        std::uintptr_t address,
        CurrentSongSnapshot &snapshot) const;

private:
    const MemoryReader &memoryReader_;
};