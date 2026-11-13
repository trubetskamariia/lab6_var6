#include "Coroutines.h"
#include <charconv>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {

    constexpr std::size_t MIN_SEQUENCE_COUNT = 1;
    constexpr std::size_t MAX_SEQUENCE_COUNT = 100000;

    std::string_view trimWhitespace(std::string_view sv) noexcept {
        const auto first = sv.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) {
            return {};
        }
        const auto last = sv.find_last_not_of(" \t\r\n");
        return sv.substr(first, last - first + 1);
    }

    std::size_t readValidatedCount() {
        while (true) {
            std::cout << "Enter how many numbers to generate ("
                << MIN_SEQUENCE_COUNT << ".." << MAX_SEQUENCE_COUNT << "): ";

            std::string line;
            if (!std::getline(std::cin, line)) {
                throw std::runtime_error("Input closed (EOF).");
            }

            const std::string_view trimmed = trimWhitespace(line);
            if (trimmed.empty()) {
                std::cout << "Empty input, please enter a number.\n\n";
                continue;
            }

            if (trimmed.front() == '-') {
                std::cout << "Number cannot be negative.\n\n";
                continue;
            }

            if (trimmed.front() == '+') {
                std::cout << "Please enter a number without '+' sign.\n\n";
                continue;
            }

            std::size_t value = 0;
            const char* begin = trimmed.data();
            const char* end = trimmed.data() + trimmed.size();
            const auto [ptr, ec] = std::from_chars(begin, end, value);

            if (ec == std::errc::result_out_of_range) {
                std::cout << "Number is too large or has invalid characters.\n\n";
                continue;
            }

            if (ec != std::errc{} || ptr != end) {
                std::cout << "Invalid input, please enter one positive integer.\n\n";
                continue;
            }

            if (value < MIN_SEQUENCE_COUNT || value > MAX_SEQUENCE_COUNT) {
                std::cout << "Number must be between "
                    << MIN_SEQUENCE_COUNT << " and " << MAX_SEQUENCE_COUNT << ".\n\n";
                continue;
            }

            return value;
        }
    }

    void runCoroutinePipeline(std::size_t count) {
        DispatchContext ctx;

        Task taskA = coroutineEvenA(ctx);
        Task taskB = coroutineOddB(ctx);
        Task taskGen = randomSequenceGenerator(ctx, count);

        ctx.coroAHandle = taskA.handle();
        ctx.coroBHandle = taskB.handle();
        ctx.generatorHandle = taskGen.handle();

        std::cout << "\nStart \n";
        ctx.generatorHandle.resume();
        std::cout << "\nDone \n";
    }

}

int main() {
    try {
        std::cout << "Random numbers " << MIN_RANDOM_VALUE << ", " << MAX_RANDOM_VALUE << "\n";

        const std::size_t count = readValidatedCount();
        runCoroutinePipeline(count);
    }
    catch (const std::exception& ex) {
        std::cerr << "\nError: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}