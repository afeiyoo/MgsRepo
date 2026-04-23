#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace Utils {

//////////////////
// find helpers
//////////////////
template<typename R, typename S, typename T>
decltype(auto) equal(R (S::*function)() const, T value)
{
    return [function, value](const S &obj) { return (obj.*function)() == value; };
}

template<typename R, typename S, typename T>
decltype(auto) equal(R S::*member, T value)
{
    return [member, value](const S &obj) { return obj.*member == value; };
}

//////////////////
// comparison predicates
//////////////////
template<typename Type>
auto equalTo(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of equalTo should be the same to prevent "
                      "unnecessary conversion.");
        return entry == value;
    };
}

template<typename Type>
auto unequalTo(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of unequalTo should be the same to prevent "
                      "unnecessary conversion.");
        return !(entry == value);
    };
}

template<typename Type>
auto lessThan(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of unequalTo should be the same to prevent "
                      "unnecessary conversion.");
        return entry < value;
    };
}

template<typename Type>
auto lessEqualThan(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of lessEqualThan should be the same to "
                      "prevent unnecessary conversion.");
        return !(value < entry);
    };
}

template<typename Type>
auto greaterThan(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of greaterThan should be the same to "
                      "prevent unnecessary conversion.");
        return value < entry;
    };
}

template<typename Type>
auto greaterEqualThan(Type &&value)
{
    return [value = std::forward<Type>(value)](const auto &entry) {
        static_assert(std::is_same<std::decay_t<Type>, std::decay_t<decltype(entry)>>::value,
                      "The container and predicate type of greaterEqualThan should be the same to "
                      "prevent unnecessary conversion.");
        return !(entry < value);
    };
}
} // namespace Utils
