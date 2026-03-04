#include "../include/hyperion.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <iomanip>
#include <iostream>

Hyperion::Hyperion(size_t total_size) : total_arena_size(total_size) {
    arena_start = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (arena_start == MAP_FAILED) {
        throw std::runtime_error("Failed to allocate memory for Hyperion arena");
    }

    free_list = static_cast<Block*>(arena_start);
    free_list->size = total_size - sizeof(Block);
    free_list->is_free = true;
    free_list->next = nullptr;
    free_list->prev = nullptr;
}

Hyperion::~Hyperion() {
    if (arena_start != MAP_FAILED) munmap(arena_start, total_arena_size);
}


size_t Hyperion::align(size_t n) {
    return(n + 7) & ~7;
}

void* Hyperion::alloc(size_t size) {
    size_t requested_size = align(size);
    Block* current = free_list;

    while (current) {
        if (current->is_free && current->size >= requested_size) {
            if (current->size >= requested_size + sizeof(Block) + 8) {
                Block* next_block = (Block*)((char*)current + sizeof(Block) + requested_size);
                next_block->size = current->size - requested_size - sizeof(Block);
                next_block->is_free = true;

                next_block->next = current->next;
                next_block->prev = current;
                if (next_block->next) next_block->next->prev = next_block;

                current->size = requested_size;
                current->next = next_block;
            }
            current->is_free = false;
            return static_cast<void*>(current + 1);
        }
        current = current->next;
    }
    return nullptr;
}

void Hyperion::free(void* ptr) {
    if (!ptr) return;

    Block* block = static_cast<Block*>(ptr) - 1;
    block->is_free = true;

    if (block->next && block->next->is_free) {
        block->size += sizeof(Block) + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }

    if (block->prev && block->prev->is_free) {
        block->prev->size += sizeof(Block) + block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
    }
}

void Hyperion::inspect() {
    Block* current = free_list;
    std::cout << "--- Heap Map ---" << '\n';

    while (current) {
        std::cout << "[" << (current->is_free ? "FREE" : "USED") << " | Size: " << current->size << "] -> ";
        current = current->next;
    }
    std::cout << "NULL" << "\n";
}

void Hyperion::dump_heap() {
    Block* curr = free_list;
    size_t i = 0;

    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << " HYPERION HEAP DUMP\n";
    std::cout << std::string(50, '=') << "\n";

    while (curr) {
        std::cout << "Block #" << i++ << " [" << (curr->is_free ? " FREE " : " USED ") << "]\n";
        std::cout << "  - Address: " << static_cast<void*>(curr) << "\n";
        std::cout << "  - Size:    " << curr->size << " bytes\n";
        std::cout << "  - Next:    " << static_cast<void*>(curr->next) << "\n";

        std::cout << "  - Visual:  |";
        for (size_t b = 0; b < (curr->size / 64) + 1; ++b) {
            std::cout << (curr->is_free ? "." : "#");
        }
        std::cout << "|\n\n";

        curr = curr->next;
    }
    std::cout << std::string(50, '=') << "\n" << std::endl;
}