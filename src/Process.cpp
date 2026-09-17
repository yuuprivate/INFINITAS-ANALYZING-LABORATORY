#include "Process.h"
#include "Logger.h"

#include <tlhelp32.h>

#include <string>

bool Process::open(DWORD processId)
{
    close();

    processHandle_ = OpenProcess(
        PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
        FALSE,
        processId
    );

    if (processHandle_ == nullptr)
    {
        processId_ = 0;

        LOG_ERROR(
            "INFINITAS のプロセスを開けませんでした。"
        );

        LOG_ERROR(
            "Failed to open the INFINITAS process."
        );

        return false;
    }

    processId_ = processId;

    LOG_INFO(
        "INFINITAS のプロセスへの接続に成功しました。"
    );

    LOG_INFO(
        "Successfully opened the INFINITAS process."
    );

    return true;
}

void Process::close()
{
    if (processHandle_ != nullptr)
    {
        CloseHandle(processHandle_);

        LOG_INFO(
            "プロセスハンドルを解放しました。"
        );

        LOG_INFO(
            "Process handle released."
        );

        processHandle_ = nullptr;
    }

    processId_ = 0;
}

bool Process::isOpen() const
{
    return processHandle_ != nullptr;
}

HANDLE Process::handle() const
{
    return processHandle_;
}

DWORD Process::processId() const
{
    return processId_;
}

bool Process::getMainModuleBaseAddress(
    std::uintptr_t& baseAddress
) const
{
    baseAddress = 0;

    if (!isOpen())
    {
        LOG_ERROR(
            "プロセスが開かれていません。"
        );

        LOG_ERROR(
            "The process is not open."
        );

        return false;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
        processId_
    );

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR(
            "メインモジュールの取得に失敗しました。"
        );

        LOG_ERROR(
            "Failed to create module snapshot."
        );

        return false;
    }

    MODULEENTRY32W moduleEntry{};
    moduleEntry.dwSize = sizeof(moduleEntry);

    if (!Module32FirstW(snapshot, &moduleEntry))
    {
        CloseHandle(snapshot);

        LOG_ERROR(
            "メインモジュールの取得に失敗しました。"
        );

        LOG_ERROR(
            "Failed to get the main module."
        );

        return false;
    }

    baseAddress = reinterpret_cast<std::uintptr_t>(
        moduleEntry.modBaseAddr
    );

    CloseHandle(snapshot);

    LOG_INFO(
        "メインモジュールのベースアドレスを取得しました。"
    );

    LOG_INFO(
        "Main module base address acquired."
    );

    return true;
}

Process::~Process()
{
    close();
}