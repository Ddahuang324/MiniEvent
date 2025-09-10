#include "../include/MultiplexerSelector.hpp"
#include "../include/MiniEventLog.hpp"

#include <vector>
#include <chrono>
#include <limits>
#include <iostream>

#include "../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp"
#ifdef __linux__
#include "../include/InheritedFromIO_Multiplexer/Epoll_multiplexer.hpp"
#endif
#ifdef __APPLE__
#include "../include/InheritedFromIO_Multiplexer/Kqueue_multiplexer.hpp"
#endif
#ifdef _WIN32
#include "../include/InheritedFromIO_Multiplexer/IOCP_multiplexer.hpp"
#endif

namespace {
    struct Candidate {
        const char* name;
        std::function<std::unique_ptr<IOMultiplexer>()> factory;
    };

    // 运行一个非常轻量的微基准：连续 N 次 dispatch(0)；
    // 因为没有 fd/事件，dispatch 应当立即返回；用来衡量基础调用开销。
    double micro_bench(IOMultiplexer& mux, int rounds = 200) {
        std::vector<Channel*> sink; // 不使用
        using clock = std::chrono::steady_clock;
        auto t0 = clock::now();
        for (int i = 0; i < rounds; ++i) {
            mux.dispatch(0, sink);
            sink.clear();
        }
        auto t1 = clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        return static_cast<double>(ns) / rounds; // 每次调用平均纳秒
    }
}

std::unique_ptr<IOMultiplexer> choose_best_multiplexer() {
    std::vector<Candidate> candidates;

    // 平台候选
#ifdef __linux__
    candidates.push_back({"epoll", []{ return std::make_unique<EpollMultiplexer>(); }});
#endif
#ifdef __APPLE__
    candidates.push_back({"kqueue", []{ return std::make_unique<KqueueMultiplexer>(); }});
#endif
#ifdef _WIN32
    candidates.push_back({"iocp", []{ return std::make_unique<IOCPMultiplexer>(); }});
#endif
    // 通用回退
    candidates.push_back({"select", []{ return std::make_unique<SelectMultiplexer>(); }});

    double best_score = std::numeric_limits<double>::infinity();
    std::unique_ptr<IOMultiplexer> best;
    const char* best_name = "";

    for (auto& c : candidates) {
        try {
            auto mux = c.factory();
            if (!mux) continue;
            double score = micro_bench(*mux);
            std::cout << "Mux candidate " << c.name << ": avg " << score << " ns/dispatch(0)" << std::endl;
            if (score < best_score) {
                best_score = score;
                best = std::move(mux);
                best_name = c.name;
            }
        } catch (...) {
            // 某些实现可能在当前平台不可用或初始化失败
            continue;
        }
    }

    if (!best) {
        log_error("No IO multiplexer available; fallback to Select");
        return std::make_unique<SelectMultiplexer>();
    }
    std::cout << "Using best multiplexer by micro-bench: " << best_name << std::endl;
    return best;
}
