#pragma once

#include <windows.h>
#include <cstdint>

class Process
{
public:
    Process() = default;
    ~Process();

    bool open(DWORD processId);
    void close();

    bool isOpen() const;
    HANDLE handle() const;
    DWORD processId() const;

    // メインモジュールのベースアドレスを取得
    bool getMainModuleBaseAddress(
        std::uintptr_t& baseAddress
    ) const;

private:
    HANDLE processHandle_ = nullptr;
    DWORD processId_ = 0;
};