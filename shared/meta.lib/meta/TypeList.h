#pragma once
#include "Type.h"

#include <utility>

namespace meta {

template<class... T>
struct TypeList {
    enum : size_t { npos = sizeof...(T) };
    using Indices = std::make_index_sequence<sizeof...(T)>;

    //    template<class C>
    //    constexpr static bool contains(Type<C> = {}) {
    //        return (true || ... || (Type<C>{} == Type<T>{}));
    //    }

    template<class C>
    constexpr static size_t indexOf(Type<C> = {}) {
        return indexOfImpl<C>(Indices{});
    }

    //    template<size_t index>
    //    constexpr static auto typeAt() {
    //        if constexpr (0 != sizeof...(T)) {
    //            return typeAtImpl<index, T...>();
    //        }
    //        else {
    //            return Type<void>{};
    //        }
    //    }

    template<class F>
    constexpr static size_t countPred(F pred) {
        return ((pred(Type<T>{}) ? 1 : 0) + ... + 0);
    }

    template<class F>
    constexpr static bool containsPred(F pred) {
        return 0 < countPred(pred);
    }

    //    template<class... C>
    //    constexpr static size_t containsAny(Type<C>...) {
    //        auto result = false;
    //        auto x = {(contains<C>() ? result = true : false)...};
    //        return result;
    //    }

    //    template<class... C>
    //    constexpr static size_t containsAny() {
    //        return containsAny(Type<C>{}...);
    //    }

    template<typename... O>
    constexpr static auto join(TypeList<O...> = {}) {
        return TypeList<T..., O...>{};
    }

    // note: we cannot use a lambda here, otherwise we cannot pass it to sub calls in constexpr context
    template<class F>
    constexpr static auto filterPred() {
        if constexpr (!containsPred(F{})) {
            return TypeList<T...>{};
        }
        else {
            return filterPredImpl<T...>(F{});
        }
    }

    //    template<class F>
    //    constexpr static auto map(F = {}) {
    //        return TypeList<unwrapType<decltype(F(Type<T>{}))>...>{};
    //    }

private:
    template<class C, size_t... I>
    constexpr static size_t indexOfImpl(std::index_sequence<I...>) {
        size_t result = npos;
        (void)((std::is_same_v<C, T> ? (result = I, true) : false) || ...);
        //(void)std::initializer_list<int>({((std::is_same_v<C, T> ? result = I : 0), 0)...});
        return result;
    }

    //    template<size_t i, class V, class... W>
    //    constexpr static auto typeAtImpl() {
    //        if constexpr (0 == i) {
    //            return Type<V>{};
    //        }
    //        else if constexpr (0 != sizeof...(W)) {
    //            return typeAtImpl<i - 1, W...>();
    //        }
    //        else {
    //            return Type<void>{};
    //        }
    //    }

    template<class V, class... W, class F>
    constexpr static auto filterPredImpl(F pred) {
        if constexpr (pred(Type<V>{})) {
            if constexpr (0 != sizeof...(W)) {
                return filterPredImpl<W...>(pred);
            }
            else {
                return TypeList<>{};
            }
        }
        else {
            if constexpr (0 != sizeof...(W)) {
                return TypeList<V>{}.join(filterPredImpl<W...>(pred));
            }
            else {
                return TypeList<V>{};
            }
        }
    }
};

template<class... A, class... B>
constexpr bool operator==(TypeList<A...>, TypeList<B...>) noexcept {
    return (std::is_same_v<A, B> && ...);
}
template<class... A, class... B>
constexpr bool operator!=(TypeList<A...> a, TypeList<B...> b) noexcept {
    return !(a == b);
}

} // namespace meta
