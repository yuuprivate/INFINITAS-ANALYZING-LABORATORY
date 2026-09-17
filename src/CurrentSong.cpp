#include "CurrentSong.h"
#include "Logger.h"

#include <array>
#include <cstdint>

CurrentSongReader::CurrentSongReader(
    const MemoryReader &memoryReader)
    : memoryReader_(memoryReader)
{
    LOG_INFO(
        "CurrentSongReader initialized successfully.");
}

bool CurrentSongReader::read(
    std::uintptr_t address,
    CurrentSongSnapshot &snapshot) const
{
    snapshot = {};

    std::array<std::int32_t, 2> values{};

    if (!memoryReader_.read(
            address,
            values.data(),
            sizeof(values)))
    {
        LOG_ERROR(
            "Failed to read CurrentSong.");

        return false;
    }

    snapshot.songId = values[0];
    snapshot.difficulty = values[1];

    return true;
}