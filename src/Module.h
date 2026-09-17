#pragma once

#include <cstdint>
#include <string>

#include "Process.h"

class Module
{
public:
    explicit Module(const Process& process);

    bool loadMainModule();

    bool isLoaded() const;

    const std::string& name() const;

    std::uintptr_t baseAddress() const;

    std::size_t imageSize() const;

    std::uintptr_t endAddress() const;

private:
    const Process& process_;

    std::string name_;
    std::uintptr_t baseAddress_ = 0;
    std::size_t imageSize_ = 0;
    bool loaded_ = false;
};