# UTF8++ #
This repository is a fork of [utf8cpp](https://sourceforge.net/projects/utfcpp).

### WARNING ###
This fork is *not* entirely source compatible to its parent library!
All deprecated APIs were removed and the `prior()` functions were renamed to
`previous()`.

## table of contents
1. Setup
    1. Requirements
    2. Dependencies
        1. Boost
    3. Compilation
2. Documentation
    1. Design Goals
    2. Alternatives
3. License

## 1. Setup ##

### 1.1. Requirements ###
A C++11 compatible compiler. Tested with:

- MSVC 19.00.22816 (x86-64)
- MinGW 4.9.2-5 (x86-64)

CMake for the unit tests.

### 1.2. Dependencies ###
The library itself doesn't depend on any external library, but the unit tests
are based upon the boost unit test framework.

1. [Boost](http://www.boost.org/) (tested with the versions { 1.58.0 })

#### 1.2.1. Boost ####
Boost is found through the FindBoost cmake module. The boost unit test framework
library is used and needs to be available to the linker.

### 1.3. compilation
It's a header only library, so you can simply add the source directory to your
include path and use the library.

The unit tests have a straight forward cmake project, so generate your build
files and build the project.


## 2. Documentation ##
See doc/utf8cpp.html for the API reference and some examples.

### 2.1. Design Goals ###
1. Generic: for better or worse, there are many C++ string classes out there,
   and the library should work with as many of them as possible. 
2. Portable: the library should be portable both accross different platforms and
   compilers. It requires a C++11 compliant compiler though, because of features
   like `<cstdint>`, `charXX_t`, `constexpr`, `noexcept`, `enum class`, etc.
3. Lightweight: follow the "pay only for what you use" guideline. 
4. Unintrusive: avoid forcing any particular design or even programming style on
   the user. This is a library, not a framework. 

### 2.2. Alternatives ###
1. ICU Library. It is very powerful, complete, feature-rich, mature, and widely
   used. Also big, intrusive, non-generic, and doesn't play well with the
   Standard Library. I definitelly recommend looking at ICU even if you don't
   plan to use it.
2. Glib::ustring. A class specifically made to work with UTF-8 strings, and also
   feel like std::string. If you prefer to have yet another string class in your
   code, it may be worth a look. Be aware of the licensing issues, though. 
3. Platform dependent solutions: Windows and POSIX have functions to convert
   strings from one encoding to another. That is only a subset of what this
   library offers, but if that is all you need it may be good enough. 


## 3. License ##
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

For Copyright information and third party licenses/copyrights see `NOTICE`.
