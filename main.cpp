#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>

using namespace std::chrono_literals;

class callback_watchdog
{
public:
    explicit callback_watchdog(asio::io_context& context)
        : context_(context)
    {
    }
    void run()
    {
        std::scoped_lock lock(mutex_);
        timer_.emplace(context_, 1s).async_wait([this](const std::error_code& error_code) { handler(error_code); });
    }
    void cancel()
    {
        std::scoped_lock lock(mutex_);
        cancelled_ = true;
        if (timer_)
        {
            timer_->cancel();
        }
    }

private:
    void handler(const std::error_code& error_code)
    {
        {
            // operation might complete successful regardless of cancellation request due to race condition
            std::scoped_lock lock(mutex_);
            if (cancelled_)
            {
                std::cout << "Tick_v1 cancelled\n";
                return;
            }
        }
        if (!error_code)
        {
            std::cout << "Tick_v1 " << ++i_ << "s" << std::endl;
            if (i_ < 5)
            {
                run();
            }
        }
        else
        {
            std::cerr << "Tick_v1 " << error_code.message() << '\n';
        }
    }
    asio::io_context& context_;

    std::mutex mutex_;
    std::optional<asio::steady_timer> timer_;
    bool cancelled_ = false;

    int i_ = 0;
};

asio::awaitable<void> coroutine_watchdog(std::stop_token stop_token)
{
    const auto& executor = co_await asio::this_coro::executor;
    for (int i = 1; i <= 5; ++i)
    {
        asio::steady_timer timer(executor, 1s);
        std::stop_callback cancel{stop_token, [&timer] { timer.cancel(); }};
        auto [error_code] = co_await timer.async_wait(asio::as_tuple(asio::use_awaitable));
        if (stop_token.stop_requested())
        {
            std::cout << "Tick_v2 cancelled\n";
            break;
        }
        if (!error_code)
        {
            std::cout << "Tick_v2 " << i << "s" << std::endl;
        }
        else
        {
            std::cerr << "Tick_v1 " << error_code.message() << '\n';
            break;
        }
    }
}

int main()
{
    asio::io_context io_context;
    callback_watchdog handler(io_context);
    handler.run();
    std::stop_source stop_source;
    asio::co_spawn(io_context, coroutine_watchdog(stop_source.get_token()), asio::detached);

    asio::steady_timer cancel_timer(io_context, 3500ms);
    cancel_timer.async_wait([&](const std::error_code&) {
        std::cout << "Cancellation request!\n";
        handler.cancel();
        stop_source.request_stop();
    });
    io_context.run();
    return 0;
}
