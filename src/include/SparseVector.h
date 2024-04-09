//
// Created by bison on 04-09-23.
//

#ifndef PLATFORMER_COMPONENTVECTOR_H
#define PLATFORMER_COMPONENTVECTOR_H

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

template <typename T>
class SparseVector {
public:
    explicit SparseVector() = default;

    void insert(size_t id, T value) {
        auto it = idToIndex.find(id);
        if(it != idToIndex.end())
            throw std::runtime_error("SparseVector::insert() called with an existing id");
        values.push_back(value);
        idToIndex[id] = values.size() - 1;
    }

    void update(size_t id, T value) {
        auto it = idToIndex.find(id);
        if(it == idToIndex.end())
            throw std::runtime_error("SparseVector::update() called on non-existing index");
        values[it->second] = value;
    }

    auto find(size_t id) {
        auto it = idToIndex.find(id);
        if(it == idToIndex.end())
            return values.end();
        return values.begin() + it->second;
    }

    void remove(size_t id) {
        auto it = idToIndex.find(id);
        if(it != idToIndex.end()) {
            values.erase(values.begin() + it->second);
            idToIndex.erase(it);
        }
    }

    size_t size() {
        return values.size();
    }

    void clear() {
        values.clear();
        idToIndex.clear();
    }

    auto begin() { return values.begin(); }
    auto end() { return values.end(); }
    auto cbegin() const { return values.begin(); }
    auto cend() const { return values.end(); }
    auto begin() const { return values.begin(); }
    auto end() const { return values.end(); }

    std::vector<T> values;
private:
    std::unordered_map<size_t, size_t> idToIndex;
};

#endif //PLATFORMER_COMPONENTVECTOR_H
