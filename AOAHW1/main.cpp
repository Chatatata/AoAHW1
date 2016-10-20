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
#include <cassert>
#include <cmath>

#ifdef __i386
#define FastUInt uint_least64_t
#define UInt uint64_t
#else
#define FastUInt uint_least64_t
#define UInt uint64_t
#endif

#define __likely(arg) if(__builtin_expect(arg, 1))
#define __unlikely(arg) if(__builtin_expect(arg, 0))

template <typename T>
class Array {
protected:
    T *data = nullptr;
    UInt length;
    
public:
    Array(UInt length) : data(new T[length]()), length(length) { }
    Array(const Array &object) : data(new T[object.length]()), length(object.length) {
        memcpy(data, object.data, sizeof(T) * object.length);
        
    }
    
    ~Array() {
        delete [] data;
    }
    
    inline UInt getLength() const noexcept {
        return length;
    }
    
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
    
    inline UInt findIndex(FastUInt value) const noexcept {
        for (UInt i = 0; i < length; ++i) {
            if (data[i] == value) {
                return i;
            }
        }
        
        return 0;
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
    MergeSortableArray(const Array<T> &object) : Array<T>(object) { }
    
    inline void sort() noexcept {
        cache = new T[length]();
        
        mergeRange(0, length);
        
        delete[] cache;
    }
};

template <typename T>
class InsertionSortableArray : public Array<T> {
    using Array<T>::data;
    using Array<T>::length;
    
public:
    InsertionSortableArray(UInt length) : Array<T>(length) { }
    InsertionSortableArray(const Array<T> &object) : Array<T>(object) { }
    
    inline void sort() noexcept {
        for (FastUInt index = 0; index < length; ++index) {
            const T object = data[index];
            
            FastUInt reverseIndex = index - 1;
            
            while (reverseIndex >= 0 && data[reverseIndex] > object) {
                data[reverseIndex + 1] = data[reverseIndex];
                
                reverseIndex -= 1;
            }
            
            data[reverseIndex + 1] = object;
        }
    }
};
                                
struct ImmutableLocation {
    UInt longitude;
    UInt latitude;
    
    ImmutableLocation(const UInt longitude = 0,
                      const UInt latitude = 0) : longitude(longitude), latitude(latitude) { }
    
    inline const UInt squareDistanceTo(const ImmutableLocation& otherLocation) const noexcept {
        UInt distanceSquare = 0LL;
        
        if (longitude > otherLocation.longitude) {
            distanceSquare += (longitude - otherLocation.longitude) * (longitude - otherLocation.longitude);
        } else {
            distanceSquare += (otherLocation.longitude - longitude) * (otherLocation.longitude - longitude);
        }
        
        if (latitude > otherLocation.latitude) {
            distanceSquare += (latitude - otherLocation.latitude) * (latitude - otherLocation.latitude);
        } else {
            distanceSquare += (otherLocation.latitude - latitude) * (otherLocation.latitude - latitude);
        }
        
        return distanceSquare;
    }
};

struct WarehouseImmutableLocation : public ImmutableLocation {
    UInt identifier;
    
    WarehouseImmutableLocation(const UInt identifier = 0, const UInt longitude = 0, const UInt latitude = 0) : ImmutableLocation(longitude, latitude), identifier(identifier) { }
};

class ActionDecoder {
    std::string *actionIdentifiers = nullptr;
    std::function<void()> *lambdaExpressions = nullptr;
    uint32_t index = 0;
    
public:
    ActionDecoder() : actionIdentifiers(new std::string[64]()), lambdaExpressions(new std::function<void()>[64]()) { }
    
    inline void registerAction(std::string identifier, std::function<void()> fn) {
        //  Make it uppercase
        std::transform(identifier.begin(),
                       identifier.end(),
                       identifier.begin(),
                       ::toupper);
        
        actionIdentifiers[index] = identifier;
        lambdaExpressions[index] = fn;
        
        index += 1;
    }
    
    inline void invokeAction(std::string identifier) const {
        for (uint32_t i = 0; i < index; ++i) {
            //  Make it uppercase
            std::transform(identifier.begin(),
                           identifier.end(),
                           identifier.begin(),
                           ::toupper);
            
            //  Find the corresponding lambda expression
            if (actionIdentifiers[i] == identifier) {
                lambdaExpressions[i]();
                
                return;
            }
        }
        
        throw "egzeption";
    }
};

int main(int argc, const char * argv[]) {
    static const UInt sortableSize = atol(argv[1]);
    static const UInt numberOfWarehouses = atol(argv[2]);
    
    auto referenceLocation = WarehouseImmutableLocation(atol(argv[4]), atol(argv[5]));
    
    printf("Reading file from disk...\n");
    FILE *file;
    
    if(!(file = fopen("warehouselocations.txt", "r"))) {
        perror("could not read file");
        exit(9);
    }
    
    Array<WarehouseImmutableLocation> locationsArray(sortableSize);
    
    for (UInt i = 0; i < sortableSize; ++i) {
        fscanf(file, "%llu\t%llu\t%llu\n", &locationsArray[i].identifier, &locationsArray[i].longitude, &locationsArray[i].latitude);
    }
    
    printf("Calculating distance squares array...\n");
    
    Array<FastUInt> distances(sortableSize);
    
    for (UInt i = 0; i < distances.getLength(); ++i) {
        distances.insertAtIndex(locationsArray[i].squareDistanceTo(referenceLocation), i);
    }
    
    ActionDecoder decoder;
    
    decoder.registerAction("is", [&](){
        volatile float timeElapsed;
        volatile float startTime;
        
        printf("Starting benchmark suite: Insertion Sort, n = %llu\n", sortableSize);
        
        InsertionSortableArray<FastUInt> insertionSortableArray(distances);
        
        startTime = (float)clock() / CLOCKS_PER_SEC;
        
        insertionSortableArray.sort();
        
        timeElapsed = (float)clock() / CLOCKS_PER_SEC - startTime;
        
        printf("Benchmarking completed, asserting results...\n");
        
        for (UInt i = 1; i < sortableSize; ++i) {
            assert(insertionSortableArray[i] >= insertionSortableArray[i - 1]);
        }
        
        printf("Insertion sort average: %f seconds\n", timeElapsed);
        
        printf("Nearest locations: \n");
        
        for (UInt i = 0; i < numberOfWarehouses; ++i) {
            WarehouseImmutableLocation wil = locationsArray[distances.findIndex(insertionSortableArray[i])];
            
            printf("%llu.   \t{#%llu, (%llu, %llu), ∂ = %.2f}\n", i, wil.identifier, wil.longitude, wil.latitude, sqrt(insertionSortableArray[i]));
        }
        
        printf("\n");
    });
    
    decoder.registerAction("ms", [&](){
        volatile float timeElapsed;
        volatile float startTime;
        
        printf("Starting benchmark suite: Merge Sort, n = %llu\n", sortableSize);
        
        MergeSortableArray<FastUInt> mergeSortableArray(distances);
        
        startTime = (float)clock() / CLOCKS_PER_SEC;
        
        mergeSortableArray.sort();
        
        timeElapsed = (float)clock() / CLOCKS_PER_SEC - startTime;
        
        printf("Benchmarking completed, asserting results...\n");
        
        for (UInt i = 1; i < mergeSortableArray.getLength(); ++i) {
            assert(mergeSortableArray[i] <= mergeSortableArray[i - 1]);
        }
        
        printf("Merge sort average: %f seconds\n", timeElapsed);
        
        printf("Nearest locations: \n");
        
        for (UInt i = 0; i < numberOfWarehouses; ++i) {
            WarehouseImmutableLocation wil = locationsArray[distances.findIndex(mergeSortableArray[mergeSortableArray.getLength() - (i + 1)])];
            
            printf("%llu.   \t{#%llu, (%llu, %llu), ∂ = %.2f}\n", i, wil.identifier, wil.longitude, wil.latitude, sqrt(mergeSortableArray[mergeSortableArray.getLength() - (i + 1)]));
        }
        
        printf("\n");
    });
    
    decoder.invokeAction(argv[3]);
    
    return 0;
}

