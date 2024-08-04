#include <asio.hpp>

int main()
{
    asio::io_context io_context;
    asio::random_access_file file(io_context);
    io_context.run();
    return 0;
}
