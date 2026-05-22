#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include <cstddef>  // size_t, ptrdiff_t
#include <new>      // ::operator new, placement new
#include <utility>  // std::forward (we'll use mystl::forward from utility.h)

#include "type_traits.h"
#include "utility.h"

namespace mystl {

// ============================================================================
// allocator — the default STL allocator
// ============================================================================

template <typename T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;

    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };

    allocator() noexcept = default;
    allocator(const allocator&) noexcept = default;
    template <typename U>
    allocator(const allocator<U>&) noexcept {}

    pointer allocate(size_type n) {
        if (n > max_size()) {
            throw "allocator::allocate: size exceeds max_size()";
        }
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type) noexcept { ::operator delete(p); }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new (static_cast<void*>(p)) U(mystl::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) noexcept {
        p->~U();
    }

    size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(T); }

    pointer address(reference x) const noexcept { return &x; }

    const_pointer address(const_reference x) const noexcept { return &x; }
};

template <typename T, typename U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept {
    return false;
}

// ============================================================================
// allocator_traits — uniform interface to any allocator
// ============================================================================

template <typename Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;
    using size_type = typename Alloc::size_type;
    using difference_type = typename Alloc::difference_type;

    using propagate_on_container_move_assignment =
        typename Alloc::propagate_on_container_move_assignment;
    using is_always_equal = typename Alloc::is_always_equal;

    template <typename T>
    using rebind_alloc = typename Alloc::template rebind<T>::other;

    template <typename U>
    struct rebind_traits {
        using other = allocator_traits<rebind_alloc<U>>;
    };

    static pointer allocate(Alloc& a, size_type n) { return a.allocate(n); }

    static void deallocate(Alloc& a, pointer p, size_type n) noexcept { a.deallocate(p, n); }

    template <typename T, typename... Args>
    static void construct(Alloc& a, T* p, Args&&... args) {
        a.construct(p, mystl::forward<Args>(args)...);
    }

    template <typename T>
    static void destroy(Alloc& a, T* p) noexcept {
        a.destroy(p);
    }

    static size_type max_size(const Alloc& a) noexcept { return a.max_size(); }

    static Alloc select_on_container_copy_construction(const Alloc& a) { return a; }
};

}  // namespace mystl

#endif  // MYSTL_ALLOCATOR_H_
