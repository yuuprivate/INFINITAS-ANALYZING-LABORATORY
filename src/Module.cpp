#include "Module.h"
#include "Logger.h"

#include <windows.h>
#include <tlhelp32.h>

#include <chrono>
#include <string>
#include <thread>

Module::Module(
    const Process &process)
    : process_(process)
{
}

bool Module::loadMainModule()
{
    name_.clear();
    baseAddress_ = 0;
    imageSize_ = 0;
    loaded_ = false;

    if (!process_.isOpen())
    {
        LOG_ERROR(
            "プロセスが開かれていません。");

        LOG_ERROR(
            "The process is not open.");

        return false;
    }

    /*
     * CreateToolhelp32Snapshotは、
     * プロセス起動直後などに一時的に失敗する場合がある。
     *
     * ERROR_BAD_LENGTH の場合は再試行する。
     */
    constexpr int maxAttempts = 5;

    HANDLE snapshot = INVALID_HANDLE_VALUE;

    for (int attempt = 1; attempt <= maxAttempts; ++attempt)
    {
        snapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            process_.processId());

        if (snapshot != INVALID_HANDLE_VALUE)
        {
            break;
        }

        const DWORD errorCode = GetLastError();

        LOG_ERROR(
            "モジュールスナップショットの取得に失敗しました。");

        LOG_ERROR(
            "Failed to create a module snapshot.");

        LOG_ERROR(
            "CreateToolhelp32Snapshot error code: " +
            std::to_string(errorCode));

        if (errorCode != ERROR_BAD_LENGTH)
        {
            return false;
        }

        if (attempt < maxAttempts)
        {
            LOG_INFO(
                "モジュールスナップショットを再試行します。");

            LOG_INFO(
                "Retrying module snapshot...");

            std::this_thread::sleep_for(
                std::chrono::milliseconds(500));
        }
    }

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR(
            "モジュールスナップショットの取得に失敗しました。");

        LOG_ERROR(
            "Failed to create a module snapshot after retries.");

        return false;
    }

    MODULEENTRY32W moduleEntry{};
    moduleEntry.dwSize = sizeof(moduleEntry);

    if (!Module32FirstW(
            snapshot,
            &moduleEntry))
    {
        const DWORD errorCode = GetLastError();

        CloseHandle(snapshot);

        LOG_ERROR(
            "メインモジュールを取得できませんでした。");

        LOG_ERROR(
            "Failed to get the main module.");

        LOG_ERROR(
            "Module32FirstW error code: " +
            std::to_string(errorCode));

        return false;
    }

    /*
     * 現時点ではbm2dx.exeというASCIIのモジュール名だけを対象にする。
     */
    char moduleNameBuffer[MAX_PATH]{};

    WideCharToMultiByte(
        CP_UTF8,
        0,
        moduleEntry.szModule,
        -1,
        moduleNameBuffer,
        MAX_PATH,
        nullptr,
        nullptr);

    name_ = moduleNameBuffer;

    baseAddress_ =
        reinterpret_cast<std::uintptr_t>(
            moduleEntry.modBaseAddr);

    imageSize_ =
        static_cast<std::size_t>(
            moduleEntry.modBaseSize);

    CloseHandle(snapshot);

    loaded_ = true;

    LOG_INFO(
        "メインモジュール情報を取得しました。");

    LOG_INFO(
        "Main module information acquired.");

    LOG_INFO(
        "Module name: " +
        name_);

    LOG_INFO(
        std::string("Module base address: 0x") +
        std::to_string(
            static_cast<unsigned long long>(
                baseAddress_)));

    LOG_INFO(
        std::string("Module image size: 0x") +
        std::to_string(
            static_cast<unsigned long long>(
                imageSize_)));

    return true;
}

bool Module::isLoaded() const
{
    return loaded_;
}

const std::string &Module::name() const
{
    return name_;
}

std::uintptr_t Module::baseAddress() const
{
    return baseAddress_;
}

std::size_t Module::imageSize() const
{
    return imageSize_;
}

std::uintptr_t Module::endAddress() const
{
    if (!loaded_)
    {
        return 0;
    }

    return baseAddress_ +
           static_cast<std::uintptr_t>(
               imageSize_);
}