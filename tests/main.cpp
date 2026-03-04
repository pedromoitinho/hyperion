#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>
#include "../include/hyperion.hpp"

void test_alignment() {
    std::cout << "[Test] Checking Memory Alignment..." << std::endl;
    Hyperion heap(1024 * 1024);

    void* p1 = heap.alloc(1);
    void* p2 = heap.alloc(13);

    assert((uintptr_t)p1 % 8 == 0);
    assert((uintptr_t)p2 % 8 == 0);

    std::cout << "  - Success: Pointers are 8-byte aligned." << std::endl;
}

void test_stress_allocation() {
    std::cout << "[Test] Stress Testing Alloc/Free Cycle..." << std::endl;
    Hyperion heap(1024 * 1024);
    std::vector<void*> ptrs;

    for (int i = 0; i < 100; ++i) {
        void* p = heap.alloc(64);
        if (p) ptrs.push_back(p);
    }

    assert(ptrs.size() == 100);
    std::cout << "  - Success: Allocated 100 blocks of 64 bytes." << std::endl;

    for (void* p : ptrs) {
        heap.free(p);
    }
    std::cout << "  - Success: All memory freed without crash." << std::endl;
}

void test_out_of_memory() {
    std::cout << "[Test] Checking Out of Memory Handling..." << std::endl;
    Hyperion heap(1024);

    void* p = heap.alloc(2048);
    assert(p == nullptr);

    std::cout << "  - Success: Allocator correctly returned nullptr." << std::endl;
}

void test_visualization_and_coalescing() {
    std::cout << "[Test] Visualizing Allocation and Coalescing..." << std::endl;

    Hyperion heap(4096);

    std::cout << "\nInitial empty heap:";
    heap.inspect();

    void* p1 = heap.alloc(128);
    void* p2 = heap.alloc(256);
    void* p3 = heap.alloc(128);

    std::cout << "\nAfter 3 allocations (128, 256, 128):";
    heap.dump_heap();

    std::cout << "Freeing middle block (256 bytes)...";
    heap.free(p2);
    heap.inspect();

    std::cout << "Freeing first block (128 bytes) to trigger coalescing...";
    heap.free(p1);

    std::cout << "\nFinal state after coalescing (p1 and p2 should be one large FREE block):";
    heap.dump_heap();

    heap.free(p3);
    std::cout << "Final heap after all frees:";
    heap.inspect();
}

int main() {
    try {
        test_alignment();
        test_stress_allocation();
        test_out_of_memory();
        test_visualization_and_coalescing();

        std::cout << "\nALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
