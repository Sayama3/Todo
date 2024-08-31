![Build and Test Workflow](https://github.com/Sayama3/Todo/actions/workflows/build_and_test.yml/badge.svg)

# Todo

A Job System written in c++20 to create and manage multithreaded application. (*Specifically my own Game Engine but you know...*)

## Requirements

Currently, the engine has no dependency other than the C++ 20 standard library.
You will need to use CMake to test the exe or link the library. (*Or just get the sources from the folder `Todo/` to fit any build system*)

### Logging

Using the cmake option `TODO_LOG` will add a console logger (using `std::cout`) to the library.

You can supply your own using `Todo::Logger::SetupLogger` and providing a raw function pointer
(`void (*/*variable_name*/)(std::source_location, Clock::time_point, LogType, std::string)`) to the library.

Disabling the option and not providing any loader will effectivly disable the LOG, but still keep a check as there could be a logger.

To entirely disable the log enable the option `TODO_DISABLE_LOG`.

## Examples

There will be example of way to use the project with severals executable in the folder `Examples/` (*use the Cmake option* `TODO_EXAMPLES`)

## Tests

There are CTests (using Google Tests) available in the folder `Tests/` (*use the Cmake option* `TODO_TESTS`)

---

## **/!\\ WARNING /!\\**

This project is my first multithreaded project. It will serve my own purpose and other but is also mainly a way for me to learn how to handle multithreading.
