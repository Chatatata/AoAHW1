//
//  main.cpp
//  AOAHW1
//
//  Created by Buğra Ekuklu on 19.10.2016.
//  Copyright © 2016 The Digital Warehouse. All rights reserved.
//

#include <iostream>
#include <cstdint>

#ifdef __i386
#define FastUInt uint_fast32_t
#else
#define FastUInt uint_fast64_t
#endif

template <typename T>
class Array {
private:
    T *data = nullptr;
    uint64_t length;
    T *external = nullptr;
    
    void mergeRange(uint64_t start, uint64_t end) {
        if (end == start + 1) return;
        
        uint64_t i = 0;
        uint64_t length = end - start;
        uint64_t middle = length / 2;
        
        uint64_t leftIterator = start, rightIterator = start + middle;
        
        mergeRange(start, start + middle);
        mergeRange(start + middle, end);
        
        for (i = 0; i < length; ++i) {
            if (leftIterator < start + middle && (rightIterator == end || data[leftIterator] > data[rightIterator])) {
                external[i] = data[leftIterator++];
            } else {
                external[i] = data[rightIterator++];
            }
        }
        
        for (i = start; i < end; ++i) {
            data[i] = external[i - start];
        }
    }
    
public:
    Array(uint64_t length) : data(new T[length]()), length(length) { }
    
    inline void insertAtIndex(const T &object, const uint64_t index) noexcept {
        data[index] = object;
    }
    
    inline T &objectAtIndex(const uint64_t index) const noexcept {
        //  TODO: Static invocation opportunity
        return data[index];
    }
    
    inline T &operator [](const uint64_t index) const noexcept {
        //  TODO: Flat call, static invocation opportunity
        return objectAtIndex(index);
    }
    
    void mergeSort() {
        external = new T[length]();
        
        mergeRange(0, length);
        
        delete[] external;
    }
};

int main(int argc, const char * argv[]) {
    int i;
    
    printf("List before sorting\n");
    
    Array<FastUInt> array(5);
    
    array.insertAtIndex(19, 0);
    array.insertAtIndex(10, 1);
    array.insertAtIndex(14, 2);
    array.insertAtIndex(27, 3);
    array.insertAtIndex(4, 4);
    
    for(i = 0; i < 5; i++) {
        printf("%llu\n", array[i]);
    }
    
    array.mergeSort();
    
    printf("\nList after sorting\n");
    
    for(i = 0; i < 5; i++)
        printf("%llu\n", array[i]);
}

