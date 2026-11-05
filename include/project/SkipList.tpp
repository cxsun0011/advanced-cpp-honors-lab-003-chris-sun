#pragma once

#include <stdexcept>

// Lab 23: Skip Lists; Probabilistic Search Structures
// Implement the functions below. Do not change the public API.

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(std::size_t maxLevel, double probability, unsigned int seed)
    : head_(nullptr), size_(0), maxLevel_(maxLevel), probability_(probability),
      generator_(seed), distribution_(0.0, 1.0) {
    // TODO: Validate arguments and create the sentinel head node.
    if(maxLevel == 0 || !(probability_ > 0.0 && probability_ < 1.0))
        throw std::invalid_argument("Invalid parameters");
    head_ = new Node(Key(), Value(), maxLevel_);
}

template <typename Key, typename Value>
SkipList<Key, Value>::~SkipList() { clear(); }

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(SkipList&& other) noexcept
    : head_(other.head_), size_(other.size_), maxLevel_(other.maxLevel_),
      probability_(other.probability_), generator_(std::move(other.generator_)),
      distribution_(other.distribution_) {
    other.head_ = nullptr;
    other.size_ = 0;
}

template <typename Key, typename Value>
SkipList<Key, Value>& SkipList<Key, Value>::operator=(SkipList&& other) noexcept {
    if (this != &other) {
        clear();
        head_ = other.head_;
        size_ = other.size_;
        maxLevel_ = other.maxLevel_;
        probability_ = other.probability_;
        generator_ = std::move(other.generator_);
        distribution_ = other.distribution_;
        other.head_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::randomLevel() {
    // TODO: Start at level 0 and promote while distribution_() < probability_,
    // stopping at maxLevel_.
    std::size_t level = 0;
    while (level < maxLevel_ && distribution_(generator_) < probability_)
        level++;
    return level;
}

template <typename Key, typename Value>
void SkipList<Key, Value>::clear() {
    // TODO: Delete every data node, then delete the sentinel.
    if(!head_) return;
    Node* current = head_ ->forward[0];
    while(current != nullptr) {
        Node* nextNode = current->forward[0];
        delete current;
        current = nextNode;
    }
    delete head_;
    head_ = nullptr;
    size_ = 0;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key& key, const Value& value) {
    // TODO: Locate predecessors at every level. Update duplicate values without
    // increasing size_. Otherwise create a random-height node and splice it in.
        std::size_t numLevels = levelCount();
    std::vector<Node*> update(numLevels, nullptr);
    Node* curr = head_;

    for (std::size_t level = numLevels; level-- > 0;) {
        while (level < curr->forward.size() &&
               curr->forward[level] != nullptr &&
               curr->forward[level]->key < key) {
            curr = curr->forward[level];
        }
        update[level] = curr;
    }
    curr = curr->forward[0];
    if (curr != nullptr && curr->key == key) {
        curr->value = value;
        return false;
    }
    std::size_t newLevel = randomLevel();
    Node* newNode = new Node(key, value, newLevel);

    for (std::size_t level = 0; level <= newLevel; level++) {
        newNode->forward[level] = update[level]->forward[level];
        update[level]->forward[level] = newNode;
    }
    ++size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::erase(const Key& key) {
    // TODO: Locate predecessors, unlink the target from every applicable level,
    // delete it, decrement size_, and return true. Return false if absent.
     std::size_t numLevels = levelCount();
    std::vector<Node*> update(numLevels, nullptr);
    Node* curr = head_;

    for (std::size_t level = numLevels; level-- > 0;) {
        while (level < curr->forward.size() &&
               curr->forward[level] != nullptr &&
               curr->forward[level]->key < key) {
            curr = curr->forward[level];
        }
        update[level] = curr;
    }
    curr = curr->forward[0];
    if (curr == nullptr || curr->key != key)
        return false;

    for (std::size_t level = 0; level < numLevels; level++) {
        if (update[level]->forward[level] == curr)
            update[level]->forward[level] = curr->forward[level];
    }
    delete curr;
    --size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::contains(const Key& key) const { return find(key) != nullptr; }

template <typename Key, typename Value>
Value* SkipList<Key, Value>::find(const Key& key) {
    // TODO: Search from the highest level down; return the value pointer or nullptr.
    if (!head_) return nullptr;
    Node* curr = head_;
    for (std::size_t level = levelCount(); level > 0; --level) {
        while (level < curr->forward.size() &&
               curr->forward[level] != nullptr &&
               curr->forward[level]->key < key) {
            curr = curr->forward[level];
        }
    }
    curr = curr->forward[0];
    if (curr != nullptr && curr->key == key)
        return &(curr->value);
    return nullptr;
}

template <typename Key, typename Value>
const Value* SkipList<Key, Value>::find(const Key& key) const {
    // TODO: Const version of find().
    if(!head_) return nullptr;
    const Node* curr = head_;
    for (std::size_t level = levelCount(); level-- > 0;) {
        while (level < curr->forward.size() &&
               curr->forward[level] != nullptr &&
               curr->forward[level]->key < key) {
            curr = curr->forward[level];
        }
    }
    curr = curr->forward[0];
    if (curr != nullptr && curr->key == key)
        return &(curr->value);
    return nullptr;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::size() const noexcept { return size_; }

template <typename Key, typename Value>
bool SkipList<Key, Value>::empty() const noexcept { return size_ == 0; }

template <typename Key, typename Value>
std::vector<std::pair<Key, Value>> SkipList<Key, Value>::items() const {
    // TODO: Traverse level 0 and return all entries in ascending key order.
     std::vector<std::pair<Key, Value>> result;
    if(!head_) return result;
    const Node* curr = head_->forward[0];
    while (curr != nullptr) {
        result.emplace_back(curr->key, curr->value);
        curr = curr->forward[0];
    }
    return result;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::levelCount() const noexcept {
    return head_ ? head_->forward.size() : 0;
}
