#include "Coroutines.h"
#include <iostream>
#include <random>

namespace {

    template <typename Action>
    Task runHandlerLoop(DispatchContext& ctx, Action action) {
        while (true) {
            action(ctx.currentValue);
            co_await RoutingAwaiter{ ctx };
        }
    }

} 
Task coroutineEvenA(DispatchContext& ctx) {
    return runHandlerLoop(ctx, [](int value) {
        std::cout << "Coroutine A (even): " << value << '\n';
        });
}

Task coroutineOddB(DispatchContext& ctx) {
    return runHandlerLoop(ctx, [](int value) {
        std::cout << "Coroutine B (odd):  " << value << "^2 = " << (value * value) << '\n';
        });
}

Task randomSequenceGenerator(DispatchContext& ctx, std::size_t count) {
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<int> dist(MIN_RANDOM_VALUE, MAX_RANDOM_VALUE);

    for (std::size_t i = 0; i < count; ++i) {
        ctx.currentValue = dist(engine);
        std::cout << "\nStep " << (i + 1) << " of " << count
            << " | generated: " << ctx.currentValue << '\n';
        co_await RoutingAwaiter{ ctx };
    }
}