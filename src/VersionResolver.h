#pragma once

#include "OffsetManager.h"
#include "VersionDetector.h"

#include <string>
#include <vector>

class VersionResolver
{
public:
    bool resolve(
        const std::vector<VersionCandidate>& candidates,
        const OffsetManager& offsetManager,
        VersionCandidate& resolved
    ) const;
};