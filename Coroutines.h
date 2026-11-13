#pragma once

#include "CoroutineFramework.h"
#include <cstddef>

inline constexpr int MIN_RANDOM_VALUE = 1;
inline constexpr int MAX_RANDOM_VALUE = 256;

Task coroutineEvenA(DispatchContext& ctx);
Task coroutineOddB(DispatchContext& ctx);
Task randomSequenceGenerator(DispatchContext& ctx, std::size_t count);