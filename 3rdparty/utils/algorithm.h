#pragma once

#include "optional.h"
#include "predicates.h"

#include <qcompilerdetection.h> // for Q_REQUIRED_RESULT

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <QObject>
#include <QSet>
#include <QStringList>

#include <memory>
#include <type_traits>

namespace Utils {

/**
 * anyOf(container, pred)	容器中是否存在任意元素满足 pred 条件
 * count(container, pred)	统计容器中满足 pred 条件的元素数量
 * allOf(container, pred)	容器中是否所有元素都满足 pred 条件
 * erase(container, pred)	从容器中删除满足 pred 条件的元素
 * contains(container, pred)	和 anyOf 一样，语义更直观
 * findOr(container, default, pred)	查找容器中满足 pred 条件的元素，否则返回默认值 default
 * findOrDefault(container, pred)	查找容器中满足 pred 条件的元素，否则返回类型默认构造值（需可复制）
 * indexOf(container, pred)	返回第一个满足 pred 条件元素的索引，不存在则返回 -1
 * maxElementOr(container, default) 在一个容器中查找最大元素。如果容器是空的，则返回指定的“默认值”。
 * transform(container, func) 将容器中的每个元素通过某个函数 func 变换为新的值，并生成一个新的容器（可以是不同类型的容器，例如可以QList转成QSet）
 * filtered(container, pred)  筛选容器中满足 pred 条件的元素并返回（以容器的方式）
 * partition(container, pred)   将容器按条件 pred 拆分为两个（作为tuple返回）
 * filteredUnique(container, pred)   在原容器中去重，并返回一个新容器（保留每个元素的第一次出现，后面重复的会被忽略）
 * qobject_container_cast(container)  对容器中的所有 QObject* 派生类指针做 qobject_cast<T>()，返回成功转换的结果容器。
 * static_container_cast(container)  将一个容器中的元素通过 static_cast 转换为另一种类型，返回新的容器
 * sort(container, pred)	对容器进行按照自定义条件函数 pred 进行排序
 * reverseForeach(container, op)  从容器尾部开始遍历，并对每个元素执行一次操作 op
 * toReferences(container) 将容器元素转换为引用包装 std::reference_wrapper<T>，适合不想复制容器元素时使用。
 * toConstReferences(container)  和 toReferences 类似，但结果是 std::reference_wrapper<const T>，不允许修改。
 * take(container, pred)   从容器中查找满足 pred 条件的第一个匹配项并移除，返回 optional<value_type>，找不到就返回 nullopt。
 * setUnionMerge(...)  智能合并两个已排序序列。允许定义：合并规则，当两个元素“相等”时怎么合并；比较规则，如何判断元素大小（默认使用 <）
 * usize(container)/ssize(container)  将 container.size() 显式转换为无符号（usize）或有符号（ssize）的整数类型
 */

//////////////////
// anyOf
/////////////////
template<typename T, typename F>
bool anyOf(const T &container, F predicate)
{
    return std::any_of(std::begin(container), std::end(container), predicate);
}

// anyOf taking a member function pointer
template<typename T, typename R, typename S>
bool anyOf(const T &container, R (S::*predicate)() const)
{
    return std::any_of(std::begin(container), std::end(container), std::mem_fn(predicate));
}

// anyOf taking a member pointer
template<typename T, typename R, typename S>
bool anyOf(const T &container, R S::*member)
{
    return std::any_of(std::begin(container), std::end(container), std::mem_fn(member));
}

//////////////////
// count
/////////////////
template<typename T, typename F>
int count(const T &container, F predicate)
{
    return std::count_if(std::begin(container), std::end(container), predicate);
}

//////////////////
// allOf
/////////////////
template<typename T, typename F>
bool allOf(const T &container, F predicate)
{
    return std::all_of(std::begin(container), std::end(container), predicate);
}

//////////////////
// erase
/////////////////
template<typename T, typename F>
void erase(T &container, F predicate)
{
    container.erase(std::remove_if(std::begin(container), std::end(container), predicate),
                    std::end(container));
}

//////////////////
// contains
/////////////////
template<typename T, typename F>
bool contains(const T &container, F function)
{
    return anyOf(container, function);
}

template<typename T, typename R, typename S>
bool contains(const T &container, R (S::*function)() const)
{
    return anyOf(container, function);
}

template<typename C, typename R, typename S>
bool contains(const C &container, R S::*member)
{
    return anyOf(container, std::mem_fn(member));
}

//////////////////
// findOr
/////////////////
template<typename C, typename F>
Q_REQUIRED_RESULT typename C::value_type findOr(const C &container,
                                                typename C::value_type other,
                                                F function)
{
    typename C::const_iterator begin = std::begin(container);
    typename C::const_iterator end = std::end(container);

    typename C::const_iterator it = std::find_if(begin, end, function);
    return it == end ? other : *it;
}

template<typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container,
                                                typename T::value_type other,
                                                R (S::*function)() const)
{
    return findOr(container, other, std::mem_fn(function));
}

template<typename T, typename R, typename S>
Q_REQUIRED_RESULT typename T::value_type findOr(const T &container,
                                                typename T::value_type other,
                                                R S::*member)
{
    return findOr(container, other, std::mem_fn(member));
}

//////////////////
// findOrDefault
//////////////////
// Default implementation:
template<typename C, typename F>
Q_REQUIRED_RESULT typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value,
                                            typename C::value_type>
findOrDefault(const C &container, F function)
{
    return findOr(container, typename C::value_type(), function);
}

template<typename C, typename R, typename S>
Q_REQUIRED_RESULT typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value,
                                            typename C::value_type>
findOrDefault(const C &container, R (S::*function)() const)
{
    return findOr(container, typename C::value_type(), std::mem_fn(function));
}

template<typename C, typename R, typename S>
Q_REQUIRED_RESULT typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value,
                                            typename C::value_type>
findOrDefault(const C &container, R S::*member)
{
    return findOr(container, typename C::value_type(), std::mem_fn(member));
}

//////////////////
// index of:
//////////////////
template<typename C, typename F>
Q_REQUIRED_RESULT int indexOf(const C &container, F function)
{
    typename C::const_iterator begin = std::begin(container);
    typename C::const_iterator end = std::end(container);

    typename C::const_iterator it = std::find_if(begin, end, function);
    return it == end ? -1 : std::distance(begin, it);
}

//////////////////
// max element
//////////////////
template<typename T>
typename T::value_type maxElementOr(const T &container, typename T::value_type other)
{
    typename T::const_iterator begin = std::begin(container);
    typename T::const_iterator end = std::end(container);

    typename T::const_iterator it = std::max_element(begin, end);
    if (it == end)
        return other;
    return *it;
}

//////////////////
// transform
/////////////////

namespace {
/////////////////
// helper code for transform to use back_inserter and thus push_back for everything
// and insert for QSet<>
//

// SetInsertIterator, straight from the standard for insert_iterator
// just without the additional parameter to insert
template<class Container>
class SetInsertIterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
protected:
    Container *container;

public:
    using container_type = Container;
    explicit SetInsertIterator(Container &x)
        : container(&x)
    {}
    SetInsertIterator<Container> &operator=(const typename Container::value_type &value)
    {
        container->insert(value);
        return *this;
    }
    SetInsertIterator<Container> &operator=(typename Container::value_type &&value)
    {
        container->insert(std::move(value));
        return *this;
    }
    SetInsertIterator<Container> &operator*() { return *this; }
    SetInsertIterator<Container> &operator++() { return *this; }
    SetInsertIterator<Container> operator++(int) { return *this; }
};

// for QMap / QHash, inserting a std::pair / QPair
template<class Container>
class MapInsertIterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
protected:
    Container *container;

public:
    using container_type = Container;
    explicit MapInsertIterator(Container &x)
        : container(&x)
    {}
    MapInsertIterator<Container> &operator=(
        const std::pair<const typename Container::key_type, typename Container::mapped_type> &value)
    {
        container->insert(value.first, value.second);
        return *this;
    }
    MapInsertIterator<Container> &operator=(
        const QPair<typename Container::key_type, typename Container::mapped_type> &value)
    {
        container->insert(value.first, value.second);
        return *this;
    }
    MapInsertIterator<Container> &operator*() { return *this; }
    MapInsertIterator<Container> &operator++() { return *this; }
    MapInsertIterator<Container> operator++(int) { return *this; }
};

// inserter helper function, returns a std::back_inserter for most containers
// and is overloaded for QSet<> and other containers without push_back, returning custom inserters
template<typename C>
inline std::back_insert_iterator<C> inserter(C &container)
{
    return std::back_inserter(container);
}

template<typename X>
inline SetInsertIterator<QSet<X>> inserter(QSet<X> &container)
{
    return SetInsertIterator<QSet<X>>(container);
}

template<typename K, typename C, typename A>
inline SetInsertIterator<std::set<K, C, A>> inserter(std::set<K, C, A> &container)
{
    return SetInsertIterator<std::set<K, C, A>>(container);
}

template<typename K, typename H, typename C, typename A>
inline SetInsertIterator<std::unordered_set<K, H, C, A>> inserter(
    std::unordered_set<K, H, C, A> &container)
{
    return SetInsertIterator<std::unordered_set<K, H, C, A>>(container);
}

template<typename K, typename V, typename C, typename A>
inline SetInsertIterator<std::map<K, V, C, A>> inserter(std::map<K, V, C, A> &container)
{
    return SetInsertIterator<std::map<K, V, C, A>>(container);
}

template<typename K, typename V, typename H, typename C, typename A>
inline SetInsertIterator<std::unordered_map<K, V, H, C, A>> inserter(
    std::unordered_map<K, V, H, C, A> &container)
{
    return SetInsertIterator<std::unordered_map<K, V, H, C, A>>(container);
}

template<typename K, typename V>
inline MapInsertIterator<QMap<K, V>> inserter(QMap<K, V> &container)
{
    return MapInsertIterator<QMap<K, V>>(container);
}

template<typename K, typename V>
inline MapInsertIterator<QHash<K, V>> inserter(QHash<K, V> &container)
{
    return MapInsertIterator<QHash<K, V>>(container);
}

// Helper code for container.reserve that makes it possible to effectively disable it for
// specific cases

// default: do reserve
// Template arguments are more specific than the second version below, so this is tried first
template<template<typename...> class C, typename... CArgs, typename = decltype(&C<CArgs...>::reserve)>
void reserve(C<CArgs...> &c, typename C<CArgs...>::size_type s)
{
    c.reserve(s);
}

// containers that don't have reserve()
template<typename C>
void reserve(C &, typename C::size_type)
{}

} // namespace

// --------------------------------------------------------------------
// Different containers for input and output:
// --------------------------------------------------------------------

// different container types for input and output, e.g. transforming a QList into a QSet

// function without result type deduction:
template<typename ResultContainer, // complete result container type
         typename SC,              // input container type
         typename F>               // function type
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function)
{
    ResultContainer result;
    reserve(result, typename ResultContainer::size_type(container.size()));
    std::transform(std::begin(container), std::end(container), inserter(result), function);
    return result;
}

// function with result type deduction:
template<template<typename> class C, // result container type
         typename SC,                // input container type
         typename F,                 // function type
         typename Value = typename std::decay_t<SC>::value_type,
         typename Result = std::decay_t<std::result_of_t<F(Value &)>>,
         typename ResultContainer = C<Result>>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function)
{
    return transform<ResultContainer>(std::forward<SC>(container), function);
}

template<template<typename, typename> class C, // result container type
         typename SC,                          // input container type
         typename F,                           // function type
         typename Value = typename std::decay_t<SC>::value_type,
         typename Result = std::decay_t<std::result_of_t<F(Value &)>>,
         typename ResultContainer = C<Result, std::allocator<Result>>>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, F function)
{
    return transform<ResultContainer>(std::forward<SC>(container), function);
}

// member function without result type deduction:
template<template<typename...> class C, // result container type
         typename SC,                   // input container type
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const)
{
    return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

// member function with result type deduction:
template<typename ResultContainer, // complete result container type
         typename SC,              // input container type
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R (S::*p)() const)
{
    return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

// member without result type deduction:
template<typename ResultContainer, // complete result container type
         typename SC,              // input container
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p)
{
    return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

// member with result type deduction:
template<template<typename...> class C, // result container
         typename SC,                   // input container
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(SC &&container, R S::*p)
{
    return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

// same container types for input and output, const input

// function:
template<template<typename...> class C, // container type
         typename F,                    // function type
         typename... CArgs>             // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, F function)
{
    return transform<C, const C<CArgs...> &>(container, function);
}

// member function:
template<template<typename...> class C, // container type
         typename R,
         typename S,
         typename... CArgs> // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R (S::*p)() const)
{
    return transform<C, const C<CArgs...> &>(container, std::mem_fn(p));
}

// members:
template<template<typename...> class C, // container
         typename R,
         typename S,
         typename... CArgs> // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(const C<CArgs...> &container, R S::*p)
{
    return transform<C, const C<CArgs...> &>(container, std::mem_fn(p));
}

// same container types for input and output, non-const input

// function:
template<template<typename...> class C, // container type
         typename F,                    // function type
         typename... CArgs>             // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, F function)
{
    return transform<C, C<CArgs...> &>(container, function);
}

// member function:
template<template<typename...> class C, // container type
         typename R,
         typename S,
         typename... CArgs> // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R (S::*p)() const)
{
    return transform<C, C<CArgs...> &>(container, std::mem_fn(p));
}

// members:
template<template<typename...> class C, // container
         typename R,
         typename S,
         typename... CArgs> // Arguments to SC
Q_REQUIRED_RESULT decltype(auto) transform(C<CArgs...> &container, R S::*p)
{
    return transform<C, C<CArgs...> &>(container, std::mem_fn(p));
}

// Specialization for QStringList:

template<template<typename...> class C = QList, // result container
         typename F>                            // Arguments to C
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, F function)
{
    return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container), function);
}

// member function:
template<template<typename...> class C = QList, // result container type
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, R (S::*p)() const)
{
    return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container),
                                                std::mem_fn(p));
}

// members:
template<template<typename...> class C = QList, // result container
         typename R,
         typename S>
Q_REQUIRED_RESULT decltype(auto) transform(const QStringList &container, R S::*p)
{
    return transform<C, const QList<QString> &>(static_cast<QList<QString>>(container),
                                                std::mem_fn(p));
}

//////////////////
// filtered
/////////////////
template<typename C, typename F>
Q_REQUIRED_RESULT C filtered(const C &container, F predicate)
{
    C out;
    std::copy_if(std::begin(container), std::end(container), inserter(out), predicate);
    return out;
}

template<typename C, typename R, typename S>
Q_REQUIRED_RESULT C filtered(const C &container, R (S::*predicate)() const)
{
    C out;
    std::copy_if(std::begin(container), std::end(container), inserter(out), std::mem_fn(predicate));
    return out;
}

//////////////////
// partition
/////////////////

// Recommended usage:
// C hit;
// C miss;
// std::tie(hit, miss) = Utils::partition(container, predicate);

template<typename C, typename F>
Q_REQUIRED_RESULT std::tuple<C, C> partition(const C &container, F predicate)
{
    C hit;
    C miss;
    auto hitIns = inserter(hit);
    auto missIns = inserter(miss);
    for (auto i : container) {
        if (predicate(i))
            hitIns = i;
        else
            missIns = i;
    }
    return std::make_tuple(hit, miss);
}

template<typename C, typename R, typename S>
Q_REQUIRED_RESULT std::tuple<C, C> partition(const C &container, R (S::*predicate)() const)
{
    return partition(container, std::mem_fn(predicate));
}

//////////////////
// filteredUnique
/////////////////

template<typename C>
Q_REQUIRED_RESULT C filteredUnique(const C &container)
{
    C result;
    auto ins = inserter(result);

    QSet<typename C::value_type> seen;
    int setSize = 0;

    auto endIt = std::end(container);
    for (auto it = std::begin(container); it != endIt; ++it) {
        seen.insert(*it);
        if (setSize == seen.size()) // unchanged size => was already seen
            continue;
        ++setSize;
        ins = *it;
    }
    return result;
}

//////////////////
// qobject_container_cast
/////////////////
template<class T, template<typename> class Container, typename Base>
Container<T> qobject_container_cast(const Container<Base> &container)
{
    Container<T> result;
    auto ins = inserter(result);
    for (Base val : container) {
        if (T target = qobject_cast<T>(val))
            ins = target;
    }
    return result;
}

//////////////////
// static_container_cast
/////////////////
template<class T, template<typename> class Container, typename Base>
Container<T> static_container_cast(const Container<Base> &container)
{
    Container<T> result;
    reserve(result, container.size());
    auto ins = inserter(result);
    for (Base val : container)
        ins = static_cast<T>(val);
    return result;
}

//////////////////
// sort
/////////////////
template<typename Container>
inline void sort(Container &container)
{
    std::sort(std::begin(container), std::end(container));
}

template<typename Container, typename Predicate>
inline void sort(Container &container, Predicate p)
{
    std::sort(std::begin(container), std::end(container), p);
}

// pointer to member
template<typename Container, typename R, typename S>
inline void sort(Container &container, R S::*member)
{
    auto f = std::mem_fn(member);
    using const_ref = typename Container::const_reference;
    std::sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) {
        return f(a) < f(b);
    });
}

// pointer to member function
template<typename Container, typename R, typename S>
inline void sort(Container &container, R (S::*function)() const)
{
    auto f = std::mem_fn(function);
    using const_ref = typename Container::const_reference;
    std::sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) {
        return f(a) < f(b);
    });
}

//////////////////
// reverseForeach
/////////////////
template<typename Container, typename Op>
inline void reverseForeach(const Container &c, const Op &operation)
{
    auto rend = c.rend();
    for (auto it = c.rbegin(); it != rend; ++it)
        operation(*it);
}

//////////////////
// toReferences
/////////////////
template<template<typename...> class ResultContainer, typename SourceContainer>
auto toReferences(SourceContainer &sources)
{
    return transform<ResultContainer>(sources, [](auto &value) { return std::ref(value); });
}

template<typename SourceContainer>
auto toReferences(SourceContainer &sources)
{
    return transform(sources, [](auto &value) { return std::ref(value); });
}

//////////////////
// toConstReferences
/////////////////
template<template<typename...> class ResultContainer, typename SourceContainer>
auto toConstReferences(const SourceContainer &sources)
{
    return transform<ResultContainer>(sources, [](const auto &value) { return std::cref(value); });
}

template<typename SourceContainer>
auto toConstReferences(const SourceContainer &sources)
{
    return transform(sources, [](const auto &value) { return std::cref(value); });
}

//////////////////
// take:
/////////////////

template<class C, typename P>
Q_REQUIRED_RESULT optional<typename C::value_type> take(C &container, P predicate)
{
    const auto end = std::end(container);

    const auto it = std::find_if(std::begin(container), end, predicate);
    if (it == end)
        return nullopt;

    optional<typename C::value_type> result = Utils::make_optional(std::move(*it));
    container.erase(it);
    return result;
}

// pointer to member
template<typename C, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) take(C &container, R S::*member)
{
    return take(container, std::mem_fn(member));
}

// pointer to member function
template<typename C, typename R, typename S>
Q_REQUIRED_RESULT decltype(auto) take(C &container, R (S::*function)() const)
{
    return take(container, std::mem_fn(function));
}

//////////////////
// setUnionMerge: Works like std::set_union but provides a merge function for items that match
//                !(a > b) && !(b > a) which normally means that there is an "equal" match.
//                It uses iterators to support move_iterators.
/////////////////

template<class InputIt1, class InputIt2, class OutputIt, class Merge, class Compare>
OutputIt setUnionMerge(InputIt1 first1,
                       InputIt1 last1,
                       InputIt2 first2,
                       InputIt2 last2,
                       OutputIt d_first,
                       Merge merge,
                       Compare comp)
{
    for (; first1 != last1; ++d_first) {
        if (first2 == last2)
            return std::copy(first1, last1, d_first);
        if (comp(*first2, *first1)) {
            *d_first = *first2++;
        } else {
            if (comp(*first1, *first2)) {
                *d_first = *first1;
            } else {
                *d_first = merge(*first1, *first2);
                ++first2;
            }
            ++first1;
        }
    }
    return std::copy(first2, last2, d_first);
}

template<class InputIt1, class InputIt2, class OutputIt, class Merge>
OutputIt setUnionMerge(
    InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Merge merge)
{
    return setUnionMerge(first1,
                         last1,
                         first2,
                         last2,
                         d_first,
                         merge,
                         std::less<std::decay_t<decltype(*first1)>>{});
}

template<class OutputContainer, class InputContainer1, class InputContainer2, class Merge, class Compare>
OutputContainer setUnionMerge(InputContainer1 &&input1,
                              InputContainer2 &&input2,
                              Merge merge,
                              Compare comp)
{
    OutputContainer results;
    results.reserve(input1.size() + input2.size());

    setUnionMerge(std::make_move_iterator(std::begin(input1)),
                  std::make_move_iterator(std::end(input1)),
                  std::make_move_iterator(std::begin(input2)),
                  std::make_move_iterator(std::end(input2)),
                  std::back_inserter(results),
                  merge,
                  comp);

    return results;
}

template<class OutputContainer, class InputContainer1, class InputContainer2, class Merge>
OutputContainer setUnionMerge(InputContainer1 &&input1, InputContainer2 &&input2, Merge merge)
{
    return setUnionMerge<OutputContainer>(std::forward<InputContainer1>(input1),
                                          std::forward<InputContainer2>(input2),
                                          merge,
                                          std::less<std::decay_t<decltype(*std::begin(input1))>>{});
}

template<typename Container>
std::make_unsigned_t<typename Container::size_type> usize(Container container)
{
    return static_cast<std::make_unsigned_t<typename Container::size_type>>(container.size());
}

template<typename Container>
std::make_signed_t<typename Container::size_type> ssize(Container container)
{
    return static_cast<std::make_signed_t<typename Container::size_type>>(container.size());
}
} // namespace Utils
