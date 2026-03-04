#pragma once
#include <cstddef>

class Hyperion {
public:
    Hyperion(size_t total_size);
    ~Hyperion();

    void* alloc(size_t size);
    void free(void* ptr);
    void inspect();
    void dump_heap();

private:
    struct Block {
        size_t size;
        bool is_free;
        Block* next;
        Block* prev;
    };

    void* arena_start;
    size_t total_arena_size;
    Block* free_list;

    size_t align(size_t n);
};