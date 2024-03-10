# The project is for asio probing
## What it does?
It implements scheduled execution (deferred in 1 second) in two different ways:
- By callback handler
- By coroutine

Also, it shows how to cancel execution in the both cases.  
Although I don't use multithreading for that I pretend that it could be used.

## Building
The project depends on `asio` library, and you should add `-DCMAKE_PREFIX_PATH=`_path to asio-config.cmake like files_.

You can use `conan 2` to install dependencies. From source directory run:

```shell
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirement.txt
conan profile detect
conan install . --output=deps --build=missing --settings=build_type=Debug
```

Then you are ready to build the project:
```shell
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=deps -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

## Running
```shell
./cmake-build-debug/asio_sandbox
```
