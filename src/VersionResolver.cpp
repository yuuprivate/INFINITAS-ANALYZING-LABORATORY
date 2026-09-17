#include "VersionResolver.h"
#include "Logger.h"

#include <sstream>

bool VersionResolver::resolve(
    const std::vector<VersionCandidate>& candidates,
    const OffsetManager& offsetManager,
    VersionCandidate& resolved
) const
{
    resolved = {};

    if (candidates.empty())
    {
        LOG_ERROR(
            "Version候補が存在しません。"
        );

        LOG_ERROR(
            "No version candidates are available."
        );

        return false;
    }

    if (!offsetManager.isLoaded())
    {
        LOG_ERROR(
            "Offsetがロードされていません。"
        );

        LOG_ERROR(
            "Offsets have not been loaded."
        );

        return false;
    }

    const std::string& offsetVersion =
        offsetManager.version();

    /*
     * OffsetファイルのVersionと
     * メモリ内Version候補を完全一致で比較。
     *
     * 同じVersionが複数存在した場合は、
     * 最後に見つかったものを採用する。
     */
    for (
        auto iterator = candidates.rbegin();
        iterator != candidates.rend();
        ++iterator
    )
    {
        if (iterator->version == offsetVersion)
        {
            resolved = *iterator;

            LOG_INFO(
                "対応するVersion候補を解決しました: " +
                resolved.version
            );

            LOG_INFO(
                "Resolved matching version candidate: " +
                resolved.version
            );

            LOG_INFO(
                [&]()
                {
                    std::ostringstream stream;

                    stream
                        << "Resolved version RVA: 0x"
                        << std::hex
                        << std::uppercase
                        << resolved.rva;

                    return stream.str();
                }()
            );

            return true;
        }
    }

    LOG_ERROR(
        "メモリ内のVersionとOffset Versionが一致しません。"
    );

    LOG_ERROR(
        "No memory version matches the offset version."
    );

    LOG_ERROR(
        "Offset version: " +
        offsetVersion
    );

    return false;
}