#include "ProcessFinder.h"
#include "Logger.h"

#include <tlhelp32.h>

#include <chrono>
#include <string>
#include <thread>

namespace
{
    constexpr wchar_t targetProcessName[] = L"bm2dx.exe";
}

DWORD ProcessFinder::waitForProcess()
{
    LOG_INFO(
        "INFINITAS の起動を待機しています..."
    );

    LOG_INFO(
        "Waiting for INFINITAS to start..."
    );

    while (true)
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS,
            0
        );

        if (snapshot == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR(
                "プロセス一覧の取得に失敗しました。"
            );

            LOG_ERROR(
                "Failed to create process snapshot."
            );

            std::this_thread::sleep_for(
                std::chrono::milliseconds(500)
            );

            continue;
        }

        PROCESSENTRY32W processEntry{};
        processEntry.dwSize = sizeof(processEntry);

        if (Process32FirstW(snapshot, &processEntry))
        {
            do
            {
                if (_wcsicmp(
                        processEntry.szExeFile,
                        targetProcessName
                    ) == 0)
                {
                    const DWORD processId =
                        processEntry.th32ProcessID;

                    CloseHandle(snapshot);

                    LOG_INFO(
                        "INFINITAS (bm2dx.exe) を検出しました。"
                    );

                    LOG_INFO(
                        "Found INFINITAS (bm2dx.exe)."
                    );

                    LOG_INFO(
                        "PID: " + std::to_string(processId)
                    );

                    return processId;
                }

            } while (Process32NextW(snapshot, &processEntry));
        }

        CloseHandle(snapshot);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );
    }
}