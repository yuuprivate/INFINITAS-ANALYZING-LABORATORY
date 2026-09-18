#pragma once

#include "OffsetTypes.h"

#include <cstdint>
#include <string>

class OffsetManager
{
public:
    bool load(
        const std::string& filePath,
        std::uintptr_t moduleBaseAddress
    );

    bool validate() const;

    bool isLoaded() const;

    const std::string& version() const;

    std::uintptr_t get(OffsetType type) const;

    std::uintptr_t getAddress(
        OffsetType type,
        std::uintptr_t moduleBaseAddress
    ) const;

private:
    OffsetCollection offsets_;
    bool loaded_ = false;

    static bool parseHex(
        const std::string& text,
        std::uintptr_t& value
    );

    static std::string trim(
        const std::string& value
    );
};

class OffsetManager
{
public:
    void setOffsets(const OffsetCollection& offsets);

    const OffsetCollection& offsets() const;

    bool hasOffsets() const;

private:
    OffsetCollection offsets_{};
    bool initialized_ = false;
};