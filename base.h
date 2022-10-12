/// Copyright (c) 2022 pagict. All rights reserved.
///
/// Author: pagict@gmail.com(pagict)
#ifndef _BASE_H_
#define _BASE_H_

#include <iterator>
#include <type_traits>
#if (__cplusplus >= 201703L)
template <typename T, typename = void> struct is_iterable : std::false_type {};

// this gets used only when we can call std::begin() and std::end() on that type
template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))>>
    : std::true_type {};
#endif
#endif