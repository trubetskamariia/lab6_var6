#pragma once

#include <coroutine>
#include <exception>
#include <utility>

struct DispatchContext {
    int currentValue = 0;
    std::coroutine_handle<> generatorHandle{};
    std::coroutine_handle<> coroAHandle{};
    std::coroutine_handle<> coroBHandle{};
};

class Task {
public:
    struct promise_type {
        Task get_return_object() noexcept {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
    };

    explicit Task(std::coroutine_handle<promise_type> handle) noexcept
        : handle_(handle) {
    }

    Task(Task&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr)) {
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    std::coroutine_handle<> handle() const noexcept {
        return handle_;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

class RoutingAwaiter {
public:
    explicit RoutingAwaiter(DispatchContext& ctx) noexcept
        : ctx_(ctx) {
    }

    bool await_ready() const noexcept {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) noexcept {
        if (caller == ctx_.generatorHandle) {
            if (ctx_.currentValue % 2 == 0) {
                return ctx_.coroAHandle;
            }
            return ctx_.coroBHandle;
        }
        return ctx_.generatorHandle;
    }

    void await_resume() const noexcept {}

private:
    DispatchContext& ctx_;
};