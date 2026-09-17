#pragma once

#include "MemoryReader.h"
#include "Module.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct VersionCandidate
{
    std::string version;
    std::size_t rva = 0;
};

class VersionDetector
{
public:
    explicit VersionDetector(
        const MemoryReader& memoryReader
    );

    /*
     * モジュール内に存在するVersion候補を
     * すべて取得する。
     */
    bool findCandidates(
        const Module& module,
        std::vector<VersionCandidate>& candidates
    ) const;

    /*
     * 従来のdetect()互換API。
     *
     * Version候補をすべて検索し、
     * 最後の候補を返す。
     */
    bool detect(
        const Module& module,
        std::string& version
    ) const;

private:
    static bool isValidVersionSuffix(
        const std::string& suffix
    );

    static std::string extractVersionAt(
        const std::uint8_t* data,
        std::size_t size
    );

    const MemoryReader& memoryReader_;
};