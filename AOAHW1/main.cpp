//
//  main.cpp
//  AOAHW1
//
//  Created by Buğra Ekuklu on 19.10.2016.
//  Copyright © 2016 The Digital Warehouse. All rights reserved.
//

#include <iostream>
#include <cstdint>
#include <ctime>

#ifdef __i386
#define FastUInt uint_least64_t
#define UInt uint64_t
#else
#define FastUInt uint_least64_t
#define UInt uint64_t
#endif

#define kSORTABLE_SIZE 100000000LL

#define __likely(arg) __builtin_expect(arg, 1)
#define __unlikely(arg) if(__builtin_expect(arg, 0))

template <typename T>
class Array {
protected:
    T *data = nullptr;
    UInt length;
    
public:
    Array(UInt length) : data(new T[length]()), length(length) { }
    
    inline void insertAtIndex(const T &object, const UInt index) noexcept {
        data[index] = object;
    }
    
    inline T &objectAtIndex(const UInt index) const noexcept {
        //  TODO: Static invocation opportunity
        return data[index];
    }
    
    inline T &operator [](const UInt index) const noexcept {
        //  TODO: Flat call, static invocation opportunity
        return objectAtIndex(index);
    }
};

template <typename T>
class MergeSortableArray : public Array<T> {
    using Array<T>::data;
    using Array<T>::length;
    
    T *cache = nullptr;
    
    inline void mergeRange(UInt start, UInt end) noexcept {
        __unlikely (end == start + 1) return;
        
        UInt i = 0;
        UInt length = end - start;
        UInt middle = length / 2;
        
        UInt leftIterator = start, rightIterator = start + middle;
        
        mergeRange(start, start + middle);
        mergeRange(start + middle, end);
        
        for (i = 0; i < length; ++i) {
            if (leftIterator < start + middle && (rightIterator == end || data[leftIterator] > data[rightIterator])) {
                cache[i] = data[leftIterator++];
            } else {
                cache[i] = data[rightIterator++];
            }
        }
        
        for (i = start; i < end; ++i) {
            data[i] = cache[i - start];
        }
    }
    
public:
    MergeSortableArray(UInt length) : Array<T>(length), cache(nullptr) { }
    
    inline void mergeSort() noexcept {
        cache = new T[length]();
        
        mergeRange(0, length);
        
        delete[] cache;
    }
};

int main(int argc, const char * argv[]) {
    float timeTotal = 0.00f;
    
    for (int i = 0; i < 1; ++i) {
        MergeSortableArray<FastUInt> array(kSORTABLE_SIZE);
        
        for (FastUInt i = 0; i < kSORTABLE_SIZE; ++i) {
            array.insertAtIndex(arc4random_uniform(BUFSIZ * BUFSIZ), i);
        }
        
        float startTime = (float)clock() / CLOCKS_PER_SEC; volatile float endTime;
        
        array.mergeSort();
        
        endTime = (float)clock() / CLOCKS_PER_SEC;
        
        timeTotal += (endTime - startTime);
    }
    
    return 0;
}

