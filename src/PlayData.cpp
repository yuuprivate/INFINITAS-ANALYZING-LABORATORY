#include "PlayData.h"

PlayDataReader::PlayDataReader(
    const MemoryReader& memoryReader)
    : memoryReader_(memoryReader)
{
}

bool PlayDataReader::read(
    std::uintptr_t address,
    PlayDataSnapshot& snapshot) const
{
    if (!memoryReader_.read(
            address + 0x00,
            snapshot.songId))
    {
        return false;
    }

    if (!memoryReader_.read(
            address + 0x04,
            snapshot.difficulty))
    {
        return false;
    }

    if (!memoryReader_.read(
            address + 0x18,
            snapshot.clearLamp))
    {
        return false;
    }

    return true;
}