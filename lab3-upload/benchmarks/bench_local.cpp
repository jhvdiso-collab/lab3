#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include "array.h"
#include "stack.h"
#include "queue.h"
#include "singly_list.h"
#include "doubly_list.h"
#include "avl_tree.h"
#include "hash_table.h"

template<class F>
void measure(const char* name, int n, F operation)
{
    using clock = std::chrono::steady_clock;
    const int repetitions = 10;
    double total = 0;
    for (int r = 0; r < repetitions; ++r) {
        auto start = clock::now();
        operation();
        total += std::chrono::duration<double, std::micro>(clock::now() - start).count();
    }
    std::cout << std::left << std::setw(23) << name << std::right << std::setw(7) << n
              << std::setw(16) << std::fixed << std::setprecision(2) << total / repetitions
              << std::setw(16) << total / repetitions / n << '\n';
}
int main()
{
    std::cout << "C++ benchmark: mean of 10 runs; construction, insertion and destruction\n";
    std::cout << std::left << std::setw(23) << "Structure" << std::right << std::setw(7) << "N" << std::setw(16) << "us/run" << std::setw(16) << "us/item" << '\n';
    for (int n : {1000,10000}) {
        std::mt19937 gen(42);
        std::vector<int> keys;
        for (int i=0;i<n;++i) keys.push_back(static_cast<int>(gen()%1000000));
        measure("Array", n, [&]{ Array a; for(int i=0;i<n;++i) a.pushBack("x"); });
        measure("Stack", n, [&]{ Stack s; for(int i=0;i<n;++i) s.push("x"); });
        measure("Queue", n, [&]{ Queue q; for(int i=0;i<n;++i) q.push("x"); });
        measure("Singly linked list", n, [&]{ Flist l; for(int i=0;i<n;++i) l.addTail("x"); });
        measure("Doubly linked list", n, [&]{ Dlist l; for(int i=0;i<n;++i) l.pushBack("x"); });
        measure("AVL", n, [&]{ AVLTree t; for(int k:keys) t.insert(k); });
        measure("Hash chaining", n, [&]{ HashTable h; for(int i=0;i<n;++i) h.add(i); });
        measure("Cuckoo hash", n, [&]{ Kukushka h; for(int i=0;i<n;++i) h.insert(i); });
    }
}
