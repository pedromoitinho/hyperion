# Hyperion

A custom memory allocator in C++23, built to study allocator design from the ground up: how an arena is obtained from the kernel, how free blocks are tracked, and how fragmentation is controlled.

Hyperion requests a contiguous virtual memory arena directly from the kernel with `mmap` and manages it with a doubly linked free list, first-fit placement and constant-time bidirectional coalescing.

## Design

**Arena from the kernel.** Memory comes from `mmap`/`munmap`, not from `malloc`. There is no user-space allocator underneath.

**Doubly linked free list.** Each block carries a header with its size, state and links to its neighbors. The doubly linked structure is what makes coalescing constant time in both directions: when a block is freed, merging it with the previous and next free blocks requires no traversal.

**First-fit placement.** The allocator walks the free list and takes the first block large enough. This is O(n) in the number of free blocks — simple and predictable in behavior, but not constant time. See *Limitations*.

**Alignment.** Returned pointers are aligned with bitwise rounding so that loads and stores are naturally aligned.

**Introspection.** `inspect()` prints a summary of the heap and free-list state; `dump_heap()` prints every block with address, size and status, which makes fragmentation visible while testing.

## Building

Requires a C++23 compiler and CMake.

```bash
mkdir build && cd build
cmake ..
make
./hyperion_test
```

## Layout

```
include/hyperion.hpp   class definitions and block metadata
src/hyperion.cpp       allocation, deallocation, coalescing
tests/main.cpp         unit and stress tests
CMakeLists.txt         build configuration
```

## Limitations

Stated explicitly, because they define what this allocator is and is not:

- **Not thread-safe.** There is no locking; concurrent use will corrupt the free list.
- **First-fit is O(n).** Allocation time grows with the number of free blocks. Not suitable for hard real-time use despite the predictable structure.
- **No size classes or segregated lists.** A single free list means fragmentation behavior depends heavily on allocation patterns.
- **Alignment is currently 8 bytes.** A drop-in `malloc` replacement on x86-64 must return memory aligned to `alignof(std::max_align_t)`, which is 16. Raising this is the next correctness fix.
- **No benchmarks yet.** Performance claims are deliberately absent until measured against glibc malloc, jemalloc and mimalloc.

## Roadmap

- 16-byte alignment for conformance with `max_align_t`
- Benchmarks against glibc malloc, jemalloc and mimalloc: throughput, fragmentation and peak RSS
- Validation under AddressSanitizer and UndefinedBehaviorSanitizer, plus fuzzing of the allocation API
- Segregated free lists by size class
- Thread safety, first with a global lock, then with per-thread arenas
- Hardening of block metadata against corruption

## License

MIT.
