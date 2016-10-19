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
#define FastUInt int
#endif

template <typename T>
class Array {
private:
    T *data = nullptr;
    uint64_t length;
    T *external = nullptr;
    
    void merge(const FastUInt lowerBound, const FastUInt medianValue, const FastUInt upperBound) {
        FastUInt firstArrayIterator = lowerBound;
        FastUInt secondArrayIterator = medianValue + 1;
        FastUInt count;
        
        for (count = lowerBound; firstArrayIterator <= medianValue && secondArrayIterator <= upperBound; ++count) {
            if (data[firstArrayIterator] <= data[secondArrayIterator]) {
                external[count] = data[firstArrayIterator++];
            } else {
                external[count] = data[secondArrayIterator++];
            }
        }
        
        while (firstArrayIterator <= medianValue) {
            external[count++] = data[firstArrayIterator++];
        }
        
        while (secondArrayIterator <= upperBound) {
            external[count++] = data[secondArrayIterator++];
        }
    }
    
    void sort(FastUInt lowerBound, FastUInt upperBound) {
        if (lowerBound < upperBound) {
            FastUInt median = (lowerBound + upperBound) / 2;
            
            sort(lowerBound, median);
            sort(median + 1, upperBound);
            merge(lowerBound, median, upperBound);
        } else {
            return;
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
        
        sort(0, length);
        
        delete[] data;
        
        data = external;
    }
};

//namespace Algorithm {
//    namespace MergeSort {
//        void merge(const FastUInt lowerBound, const FastUInt medianValue, const FastUInt upperBound) {
//            FastUInt firstArrayIterator, secondArrayIterator, count;
//            
//            for (firstArrayIterator = lowerBound, secondArrayIterator = medianValue + 1, count = lowerBound; firstArrayIterator <= medianValue && secondArrayIterator <= upperBound; ++count) {
//                if (a[firstArrayIterator] <= a[secondArrayIterator]) {
//                    b[count] = a[firstArrayIterator];
//                    
//                    firstArrayIterator += 1;
//                } else {
//                    b[count] = a[secondArrayIterator];
//                    
//                    secondArrayIterator += 1;
//                }
//            }
//            
//            while (firstArrayIterator <= medianValue) {
//                b[count++] = a[firstArrayIterator++];
//            }
//            
//            while (secondArrayIterator <= upperBound) {
//                b[count++] = a[secondArrayIterator++];
//            }
//            
//            for (FastUInt i = lowerBound; i <= upperBound; ++i) {
//                a[i] = b[i];
//            }
//        }
//        
//        void sort(FastUInt lowerBound, FastUInt upperBound) {
//            if (lowerBound < upperBound) {
//                FastUInt median = (lowerBound + upperBound) / 2l;
//                
//                sort(lowerBound, median);
//                sort(median + 1, upperBound);
//                merge(lowerBound, median, upperBound);
//            } else {
//                return;
//            }
//        }
//    }
//}

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
        printf("%u\n", array[i]);
    }
    
    array.mergeSort();
    
    printf("\nList after sorting\n");
    
    for(i = 0; i < 5; i++)
        printf("%d\n", array[i]);
}
