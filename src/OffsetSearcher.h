#pragma once

#include "JudgeData.h"
#include "OffsetTypes.h"
#include "PatternScanner.h"

#include <cstddef>
#include <cstdint>
#include <vector>

struct OffsetSearchResult
{
    OffsetType type;
    std::size_t rva = 0;
};

class Module;

class OffsetSearcher
{
public:
    explicit OffsetSearcher(
        const PatternScanner &patternScanner);

    bool searchSongList(
        const Module &module,
        OffsetSearchResult &result) const;

    bool searchUnlockData(
        const Module &module,
        OffsetSearchResult &result) const;

    bool searchDataMap(
        const Module &module,
        OffsetSearchResult &result) const;

    bool searchPlayData(
        const Module &module,
        std::uint32_t songId,
        int playType,
        int exScore,
        std::uintptr_t knownRva,
        OffsetSearchResult &result) const;
    bool searchCurrentSong(
        const Module &module,
        std::uint32_t songId,
        int difficulty,
        std::size_t knownRva,
        std::size_t ignoredPlayDataRva,
        OffsetSearchResult &result) const;


    bool searchPlaySettingsPattern1(
        const Module &module,
        std::size_t knownRva,
        std::size_t &candidateRva) const;

    bool validatePlaySettingsPattern2(
        const Module &module,
        std::size_t candidateRva) const;

    bool searchJudgeData(
        const Module &module,
        const JudgeSnapshot &snapshot,
        OffsetSearchResult &result) const;

private:
    static Pattern createSongListPattern();

    static Pattern createUnlockDataPattern();

    static Pattern createDataMapPattern();

    static Pattern createPlayDataPattern(
        std::uint32_t songId,
        int playType,
        int exScore);
    static Pattern createCurrentSongPattern(
        std::uint32_t songId,
        int difficulty);


    static Pattern createPlaySettingsPattern1();

    static Pattern createPlaySettingsPattern2();

    static Pattern createJudgeP1Pattern(
        const JudgeSnapshot &snapshot);

    static Pattern createJudgeP2Pattern(
        const JudgeSnapshot &snapshot);

private:
    const PatternScanner &patternScanner_;
};