# Hyperion

Hyperion is a high-performance, deterministic custom memory allocator implemented in C++. It is designed to replace standard library allocation with a specialized strategy focusing on memory alignment, minimal fragmentation, and $O(1)$ bidirectional coalescing.

### Technical Architecture
Hyperion manages memory using a **Doubly Linked Free-List** strategy. By interfacing directly with the kernel via the `mmap` system call, the allocator manages a contiguous virtual memory arena, bypassing the overhead of standard user-space wrappers.



#### Key Features:
* **Memory Alignment:** Guarantees 8-byte alignment for all allocated pointers using bitwise rounding formulas to ensure optimal CPU cache line performance.
* **Deterministic Allocation:** Implements a First-Fit search algorithm, providing predictable behavior essential for real-time systems.
* **Bidirectional Coalescing:** Utilizes a doubly linked list structure to perform $O(1)$ merging of adjacent free blocks in both directions (Forward and Backward). This effectively eliminates external fragmentation.
* **Zero Standard Library Dependency:** Built using raw pointer arithmetic and POSIX system calls (`mmap`, `munmap`), making it suitable for systems-level environments where the STL may be unavailable or undesirable.



---

### Project Structure
```text
Hyperion/
├── include/
│   └── hyperion.hpp    # Class definitions and metadata structures
├── src/
│   └── hyperion.cpp    # Core logic (Alloc/Free/Coalesce)
├── tests/
│   └── main.cpp        # Unit tests and stress testing
└── CMakeLists.txt      # Build configuration

Build and Execution

Hyperion utilizes CMake for cross-platform build management. A compiler with C++23 support is required for advanced system features.

1. Configure and Build:
Bash

mkdir build && cd build
cmake ..
make

2. Run Tests:
Bash

./hyperion_test

Performance & Debugging Tools

Hyperion includes built-in telemetry for heap analysis:

    inspect(): Provides a high-level summary of the current heap map and free-list state.

    dump_heap(): Generates a detailed visual representation of the memory arena, displaying block addresses, sizes, and allocation status to assist in identifying fragmentation patterns.

License

This project is open-source and available under the MIT License.
