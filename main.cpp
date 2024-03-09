#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <typeinfo>

using namespace std::chrono_literals;

class callback_watchdog
{
public:
    [[maybe_unused]] explicit callback_watchdog(asio::io_context& context)
        : context_(context)
    {
    }
    void run()
    {
        timer_.emplace(context_, 1s).async_wait([this](const std::error_code& error_code) { handler(error_code); });
    }
    void cancel()
    {
        if (timer_)
        {// TODO: It is not safe! We need to have the mutex here
            timer_->cancel();
        }
    }

private:
    void handler(const std::error_code& error_code)
    {
        if (error_code)
        {
            if (error_code == asio::error::operation_aborted)
            {
                std::cout << "Tick_v1 cancelled\n";
            }
            else
            {
                std::cerr << "Tick_v1 " << error_code.message() << '\n';
            }
            return;
        }
        std::cout << "Tick_v1 " << ++i_ << "s" << std::endl;
        if (i_ < 5)
        {
            run();
        }
    }
    asio::io_context& context_;
    std::optional<asio::steady_timer> timer_;
    int i_ = 0;
};

asio::awaitable<void> coroutine_watchdog(std::stop_token stop_token)
{
    try
    {
        const auto& executor = co_await asio::this_coro::executor;
        for (int i = 1; i <= 5; ++i)
        {
            asio::steady_timer timer(executor, 1s);
            std::stop_callback cancel{stop_token, [&timer] { timer.cancel(); }};
            co_await timer.async_wait(asio::use_awaitable);
            std::cout << "Tick_v2 " << i << "s" << std::endl;
        }
    }
    catch (const std::system_error& e)
    {
        if (e.code() == asio::error::operation_aborted)
        {
            std::cout << "Tick_v2 cancelled\n";
        }
        else
        {
            std::cerr << "Tick_v2 " << e.what() << '\n';
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

    asio::steady_timer cancel_timer(io_context, 2500ms);
    cancel_timer.async_wait([&](const std::error_code&) {
        std::cout << "Cancellation request!\n";
        handler.cancel();
        stop_source.request_stop();
    });
    io_context.run();
    return 0;
}
