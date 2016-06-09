# Brando

Brando is a lightweight and efficient library for concurrenct and functional programming in modern C++.

Brando includes Monadic Futures/Promises as well as a simple (but powerful) Actor implementation, plus Executors to control the running of asynchronous work.

Brando also includes immutable data types to aid with safe concurrent programming, as well as other functional data types such as Option that are generally useful when writing modern programs.

Brando is efficient but aims for correctness, safety, and elegance above that. The library makes full use of the latest C++ standard to push forward with how C++ code *should* be written, rather than perhaps how it tends to be written.

## Example Code

```
Task<int>([](){ return 42; })                         // Create a deferred task of work
  .runAsync(ex)                                       // Run it asynchronously (using an executor)
  .map([](int i){ return i + 1 })                     // transform the result, asynchronously
  .map([](int i){ return i * 2 })                     // .. and again ..
  .foreach([](int i){ cout << "Result is: " << i; })  // then do something with the work
```
