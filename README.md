![Build and Test Workflow](https://github.com/Sayama3/Todo/actions/workflows/build_and_test.yml/badge.svg)

# Todo

A Job System written in c++20 to create and manage multithreaded application. (*Specifically my own Game Engine but you know...*)

## Requirements

Currently, the engine has no dependency other than the C++ 20 standard library.
You will need to use CMake to test the exe or link the library. (*Or just get the sources from the folder `Todo/` to fit any build system*)

A hardware with 16 bytes atomic implementation will boost tremendously the performances.

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

## Async

For this purpose, I've re-implemented my own _future_ / _shared_future_ / _promise_ / _packaged_task_ to be able to check
without waiting whether a future was ready or not. Almost all the rest of the API has been re-implemented. 

Where as the GCC/Clang have a pretty good implementation for the cases where the duration is 0, that is not the case for
MSVC that can wait up to multiple thousands nanoseconds compare to a hundred with an atomic check.

The implementation count on an atomic implementation for 16 bytes types, so the performance will take a huge it if that's
not the case, beware.

## Credits

Usage of the following libraries :

- [Work-Stealing Queue](https://github.com/taskflow/work-stealing-queue) by _T.-W. Huang_

A lot of the code written here has been written thanks to the excellent book

    C++ Concurrency in Action (Second Edition) by Anthony Williams

Needed to mention it so here it is. Don't hesitate to read it and you'll understand why I did what I did.

## License

> Copyright 2026 github.com/Sayama3
> 
> Licensed under the Apache License, Version 2.0 (the "License");
> you may not use this file except in compliance with the License.
> You may obtain a copy of the License at
> 
>        http://www.apache.org/licenses/LICENSE-2.0
> 
> Unless required by applicable law or agreed to in writing, software
> distributed under the License is distributed on an "AS IS" BASIS,
> WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
> See the License for the specific language governing permissions and
> limitations under the License.

---

## **/!\\ WARNING /!\\**

This project is my first multithreaded project. It will serve my own purpose and other but is also mainly a way for me to learn how to handle multithreading.
