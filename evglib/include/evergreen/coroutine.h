#pragma once

#if 0

namespace std
{

    // STRUCT TEMPLATE coroutine_traits
    template <class _Ret, class = void>
    struct _Coroutine_traits {};

    template <class _Ret>
    struct _Coroutine_traits<_Ret, void_t<typename _Ret::promise_type>> {
        using promise_type = typename _Ret::promise_type;
    };

    template <class _Ret, class...>
    struct coroutine_traits : _Coroutine_traits<_Ret> {};

    // STRUCT TEMPLATE coroutine_handle
    template <class = void>
    struct coroutine_handle;

    template <>
    struct coroutine_handle<void> {
        constexpr coroutine_handle() noexcept = default;
        constexpr coroutine_handle(nullptr_t) noexcept {}

        coroutine_handle& operator=(nullptr_t) noexcept {
            _Ptr = nullptr;
            return *this;
        }

        [[nodiscard]] constexpr void* address() const noexcept {
            return _Ptr;
        }

        [[nodiscard]] static constexpr coroutine_handle from_address(void* const _Addr) noexcept { // strengthened
            coroutine_handle _Result;
            _Result._Ptr = _Addr;
            return _Result;
        }

        constexpr explicit operator bool() const noexcept {
            return _Ptr != nullptr;
        }

        [[nodiscard]] bool done() const noexcept { // strengthened
            return __builtin_coro_done(_Ptr);
        }

        void operator()() const {
            __builtin_coro_resume(_Ptr);
        }

        void resume() const {
            __builtin_coro_resume(_Ptr);
        }

        void destroy() const noexcept { // strengthened
            __builtin_coro_destroy(_Ptr);
        }

    private:
        void* _Ptr = nullptr;
    };

    template <class _Promise>
    struct coroutine_handle {
        constexpr coroutine_handle() noexcept = default;
        constexpr coroutine_handle(nullptr_t) noexcept {}

        [[nodiscard]] static coroutine_handle from_promise(_Promise& _Prom) noexcept { // strengthened
            const auto _Prom_ptr = const_cast<void*>(static_cast<const volatile void*>(::std::addressof(_Prom)));
            const auto _Frame_ptr = __builtin_coro_promise(_Prom_ptr, 0, true);
            coroutine_handle _Result;
            _Result._Ptr = _Frame_ptr;
            return _Result;
        }

        coroutine_handle& operator=(nullptr_t) noexcept {
            _Ptr = nullptr;
            return *this;
        }

        [[nodiscard]] constexpr void* address() const noexcept {
            return _Ptr;
        }

        [[nodiscard]] static constexpr coroutine_handle from_address(void* const _Addr) noexcept { // strengthened
            coroutine_handle _Result;
            _Result._Ptr = _Addr;
            return _Result;
        }

        constexpr operator coroutine_handle<>() const noexcept {
            return coroutine_handle<>::from_address(_Ptr);
        }

        constexpr explicit operator bool() const noexcept {
            return _Ptr != nullptr;
        }

        [[nodiscard]] bool done() const noexcept { // strengthened
            return __builtin_coro_done(_Ptr);
        }

        void operator()() const {
            __builtin_coro_resume(_Ptr);
        }

        void resume() const {
            __builtin_coro_resume(_Ptr);
        }

        void destroy() const noexcept { // strengthened
            __builtin_coro_destroy(_Ptr);
        }

        [[nodiscard]] _Promise& promise() const noexcept { // strengthened
            return *reinterpret_cast<_Promise*>(__builtin_coro_promise(_Ptr, 0, false));
        }

    private:
        void* _Ptr = nullptr;
    };

    [[nodiscard]] constexpr bool operator==(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return _Left.address() == _Right.address();
    }

#if _HAS_CXX20
    [[nodiscard]] constexpr strong_ordering operator<=>(
        const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
#ifdef __cpp_lib_concepts
        return compare_three_way{}(_Left.address(), _Right.address());
#else // ^^^ no workaround / workaround vvv
        return _Left.address() <=> _Right.address();
#endif // __cpp_lib_concepts
    }
#else // ^^^ <=> exists / <=> does not exist vvv
    [[nodiscard]] constexpr bool operator!=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return !(_Left == _Right);
    }

    [[nodiscard]] constexpr bool operator<(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return less<void*>{}(_Left.address(), _Right.address());
    }

    [[nodiscard]] constexpr bool operator>(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return _Right < _Left;
    }

    [[nodiscard]] constexpr bool operator<=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return !(_Left > _Right);
    }

    [[nodiscard]] constexpr bool operator>=(const coroutine_handle<> _Left, const coroutine_handle<> _Right) noexcept {
        return !(_Left < _Right);
    }
#endif // _HAS_CXX20

    template <class _Promise>
    struct hash<coroutine_handle<_Promise>> {
        [[nodiscard]] size_t operator()(const coroutine_handle<_Promise>& _Coro) const noexcept {
            return _Hash_representation(_Coro.address());
        }
    };

    // STRUCT noop_coroutine_promise
    struct noop_coroutine_promise {};

    // STRUCT coroutine_handle<noop_coroutine_promise>
    template <>
    struct coroutine_handle<noop_coroutine_promise> {
        friend coroutine_handle noop_coroutine() noexcept;

        constexpr operator coroutine_handle<>() const noexcept {
            return coroutine_handle<>::from_address(_Ptr);
        }

        constexpr explicit operator bool() const noexcept {
            return true;
        }
        [[nodiscard]] constexpr bool done() const noexcept {
            return false;
        }

        constexpr void operator()() const noexcept {}
        constexpr void resume() const noexcept {}
        constexpr void destroy() const noexcept {}

        [[nodiscard]] noop_coroutine_promise& promise() const noexcept {
            // Returns a reference to the associated promise
            return *reinterpret_cast<noop_coroutine_promise*>(__builtin_coro_promise(_Ptr, 0, false));
        }

        [[nodiscard]] constexpr void* address() const noexcept {
            return _Ptr;
        }

    private:
        coroutine_handle() noexcept = default;

        void* _Ptr = __builtin_coro_noop();
    };

    // ALIAS noop_coroutine_handle
    using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;

    // FUNCTION noop_coroutine
    [[nodiscard]] inline noop_coroutine_handle noop_coroutine() noexcept {
        // Returns a handle to a coroutine that has no observable effects when resumed or destroyed.
        return noop_coroutine_handle{};
    }

    // STRUCT suspend_never
    struct suspend_never {
        [[nodiscard]] constexpr bool await_ready() const noexcept {
            return true;
        }

        constexpr void await_suspend(coroutine_handle<>) const noexcept {}
        constexpr void await_resume() const noexcept {}
    };

    // STRUCT suspend_always
    struct suspend_always {
        [[nodiscard]] constexpr bool await_ready() const noexcept {
            return false;
        }

        constexpr void await_suspend(coroutine_handle<>) const noexcept {}
        constexpr void await_resume() const noexcept {}
    };

}

#endif