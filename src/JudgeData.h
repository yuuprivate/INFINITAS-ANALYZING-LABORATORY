#pragma once

#include "MemoryReader.h"

#include <cstdint>

enum class JudgePlayType
{
    P1,
    P2,
    DP
};

struct JudgeSnapshot
{
    std::int32_t p1Pgreat = 0;
    std::int32_t p1Great = 0;
    std::int32_t p1Good = 0;
    std::int32_t p1Bad = 0;
    std::int32_t p1Poor = 0;

    std::int32_t p2Pgreat = 0;
    std::int32_t p2Great = 0;
    std::int32_t p2Good = 0;
    std::int32_t p2Bad = 0;
    std::int32_t p2Poor = 0;

    std::int32_t p1ComboBreak = 0;
    std::int32_t p2ComboBreak = 0;

    std::int32_t p1Fast = 0;
    std::int32_t p2Fast = 0;

    std::int32_t p1Slow = 0;
    std::int32_t p2Slow = 0;

    std::int32_t p1MeasureEnd = 0;
    std::int32_t p2MeasureEnd = 0;

    JudgePlayType playType =
        JudgePlayType::P1;
};

class JudgeDataReader
{
public:
    explicit JudgeDataReader(
        const MemoryReader &memoryReader);

    bool read(
        std::uintptr_t address,
        JudgeSnapshot &snapshot) const;

private:
    const MemoryReader &memoryReader_;
};

// 判定値が1つでも入っているか
bool hasJudgeResult(const JudgeSnapshot &snapshot);

// P1 / P2 / DP を判定
JudgePlayType detectJudgePlayType(const JudgeSnapshot &snapshot);