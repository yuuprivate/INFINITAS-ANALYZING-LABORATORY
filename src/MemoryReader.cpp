#include "MemoryReader.h"
#include "Logger.h"

#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <limits>

MemoryReader::MemoryReader(
    const Process& process
)
    : process_(process)
{
    LOG_INFO(
        "MemoryReader を初期化しました。"
    );

    LOG_INFO(
        "MemoryReader initialized successfully."
    );
}

bool MemoryReader::isReadableProtection(
    DWORD protection
)
{
    /*
     * PAGE_GUARD が付いている領域は、
     * 通常の読み取り対象として扱わない。
     */
    if ((protection & PAGE_GUARD) != 0)
    {
        return false;
    }

    const DWORD basicProtection =
        protection & 0xFF;

    switch (basicProtection)
    {
    case PAGE_READONLY:
    case PAGE_READWRITE:
    case PAGE_WRITECOPY:
    case PAGE_EXECUTE_READ:
    case PAGE_EXECUTE_READWRITE:
    case PAGE_EXECUTE_WRITECOPY:
        return true;

    default:
        return false;
    }
}

bool MemoryReader::isReadable(
    std::uintptr_t address,
    std::size_t size
) const
{
    if (!process_.isOpen())
    {
        return false;
    }

    if (size == 0)
    {
        return true;
    }

    std::uintptr_t currentAddress =
        address;

    std::size_t remainingSize =
        size;

    while (remainingSize > 0)
    {
        MEMORY_BASIC_INFORMATION memoryInfo{};

        const SIZE_T queryResult =
            VirtualQueryEx(
                process_.handle(),
                reinterpret_cast<LPCVOID>(
                    currentAddress
                ),
                &memoryInfo,
                sizeof(memoryInfo)
            );

        if (queryResult == 0)
        {
            return false;
        }

        /*
         * MEM_COMMIT 以外は読み取り可能とは扱わない。
         */
        if (memoryInfo.State != MEM_COMMIT)
        {
            return false;
        }

        if (!isReadableProtection(
                memoryInfo.Protect
            ))
        {
            return false;
        }

        const auto regionBase =
            reinterpret_cast<std::uintptr_t>(
                memoryInfo.BaseAddress
            );

        const auto regionSize =
            static_cast<std::uintptr_t>(
                memoryInfo.RegionSize
            );

        if (regionSize == 0)
        {
            return false;
        }

        if (currentAddress < regionBase)
        {
            return false;
        }

        const std::uintptr_t offsetInRegion =
            currentAddress - regionBase;

        if (offsetInRegion >= regionSize)
        {
            return false;
        }

        const std::uintptr_t availableSize =
            regionSize - offsetInRegion;

        const std::uintptr_t remaining =
            static_cast<std::uintptr_t>(
                remainingSize
            );

        const std::uintptr_t advance =
            std::min(
                availableSize,
                remaining
            );

        if (advance == 0)
        {
            return false;
        }

        if (
            advance >
            static_cast<std::uintptr_t>(
                std::numeric_limits<std::size_t>::max()
            )
        )
        {
            return false;
        }

        currentAddress += advance;

        remainingSize -=
            static_cast<std::size_t>(
                advance
            );
    }

    return true;
}

bool MemoryReader::read(
    std::uintptr_t address,
    void* buffer,
    std::size_t size
) const
{
    if (!process_.isOpen())
    {
        return false;
    }

    if (buffer == nullptr ||
        size == 0)
    {
        return false;
    }

    /*
     * 要求範囲全体が読み取り可能であることを確認する。
     */
    if (!isReadable(address, size))
    {
        return false;
    }

    SIZE_T bytesRead = 0;

    const BOOL result =
        ReadProcessMemory(
            process_.handle(),
            reinterpret_cast<LPCVOID>(
                address
            ),
            buffer,
            size,
            &bytesRead
        );

    if (result == FALSE)
    {
        return false;
    }

    return bytesRead == size;
}

bool MemoryReader::readReadable(
    std::uintptr_t address,
    void* buffer,
    std::size_t size
) const
{
    if (!process_.isOpen())
    {
        return false;
    }

    if (buffer == nullptr ||
        size == 0)
    {
        return false;
    }

    auto* output =
        static_cast<std::uint8_t*>(buffer);

    std::uintptr_t currentAddress =
        address;

    std::size_t remainingSize =
        size;

    bool readSomething = false;

    while (remainingSize > 0)
    {
        MEMORY_BASIC_INFORMATION memoryInfo{};

        const SIZE_T queryResult =
            VirtualQueryEx(
                process_.handle(),
                reinterpret_cast<LPCVOID>(
                    currentAddress
                ),
                &memoryInfo,
                sizeof(memoryInfo)
            );

        if (queryResult == 0)
        {
            /*
             * 残りは読み取れないため0埋め。
             */
            std::fill(
                output,
                output + remainingSize,
                0
            );

            break;
        }

        const auto regionBase =
            reinterpret_cast<std::uintptr_t>(
                memoryInfo.BaseAddress
            );

        const auto regionSize =
            static_cast<std::uintptr_t>(
                memoryInfo.RegionSize
            );

        if (
            regionSize == 0 ||
            currentAddress < regionBase
        )
        {
            std::fill(
                output,
                output + remainingSize,
                0
            );

            break;
        }

        const std::uintptr_t offsetInRegion =
            currentAddress - regionBase;

        if (offsetInRegion >= regionSize)
        {
            std::fill(
                output,
                output + remainingSize,
                0
            );

            break;
        }

        const std::uintptr_t availableSize =
            regionSize - offsetInRegion;

        const std::size_t requestedSize =
            std::min(
                remainingSize,
                static_cast<std::size_t>(
                    std::min(
                        availableSize,
                        static_cast<std::uintptr_t>(
                            std::numeric_limits<
                                std::size_t
                            >::max()
                        )
                    )
                )
            );

        bool regionReadSuccessfully = false;

        if (
            memoryInfo.State == MEM_COMMIT &&
            isReadableProtection(
                memoryInfo.Protect
            )
        )
        {
            SIZE_T bytesRead = 0;

            const BOOL result =
                ReadProcessMemory(
                    process_.handle(),
                    reinterpret_cast<LPCVOID>(
                        currentAddress
                    ),
                    output,
                    requestedSize,
                    &bytesRead
                );

            if (
                result != FALSE &&
                bytesRead == requestedSize
            )
            {
                regionReadSuccessfully = true;
                readSomething = true;
            }
        }

        if (!regionReadSuccessfully)
        {
            /*
             * 読み取れない領域は0で埋める。
             */
            std::fill(
                output,
                output + requestedSize,
                0
            );
        }

        /*
         * 成功・失敗に関係なく、
         * バッファと実アドレスの対応を維持するため、
         * requestedSize 分だけ進める。
         */
        output += requestedSize;

        currentAddress +=
            static_cast<std::uintptr_t>(
                requestedSize
            );

        remainingSize -=
            requestedSize;
    }

    return readSomething;
}