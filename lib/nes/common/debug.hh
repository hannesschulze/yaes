#pragma once

#ifdef NES_ENABLE_DEBUG_OUTPUT

#include <spdlog/spdlog.h>

#define NES_DEBUG_LOG(name, ...) \
  spdlog::info("[" #name "] " __VA_ARGS__)

#else

#define NES_DEBUG_LOG(name, fmt, ...) \
  (void)0

#endif