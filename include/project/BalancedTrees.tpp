namespace project {

// ========================= AVLTree =========================

template <typename Key, typename Value, typename Compare>
int AVLTree<Key, Value, Compare>::nodeHeight(const Node* n) {
    return n ? n->height : 0;
}

template <typename Key, typename Value, typename Compare>
int AVLTree<Key, Value, Compare>::balanceFactor(const Node* n) {
    return n ? nodeHeight(n->left) - nodeHeight(n->right) : 0;
}

template <typename Key, typename Value, typename Compare>
void AVLTree<Key, Value, Compare>::destroy(Node* n) {
    if (!n)
        return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

template <typename Key, typename Value, typename Compare>
AVLTree<Key, Value, Compare>::~AVLTree() {
    destroy(root_);
}

template <typename Key, typename Value, typename Compare>
AVLTree<Key, Value, Compare>::AVLTree(AVLTree&& other) noexcept
    : root_(other.root_), size_(other.size_), comp_(std::move(other.comp_)) {
    other.root_ = nullptr;
    other.size_ = 0;
}

template <typename Key, typename Value, typename Compare>
AVLTree<Key, Value, Compare>&
AVLTree<Key, Value, Compare>::operator=(AVLTree&& other) noexcept {
    if (this != &other) {
        destroy(root_);
        root_ = other.root_;
        size_ = other.size_;
        comp_ = std::move(other.comp_);
        other.root_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename Key, typename Value, typename Compare>
bool AVLTree<Key, Value, Compare>::empty() const noexcept {
    return size_ == 0;
}

template <typename Key, typename Value, typename Compare>
std::size_t AVLTree<Key, Value, Compare>::size() const noexcept {
    return size_;
}

template <typename Key, typename Value, typename Compare>
int AVLTree<Key, Value, Compare>::height() const noexcept {
    return nodeHeight(root_);
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::rotateLeft(Node* x) {
    Node* y = x->right;
    if (!y)
        return x;

    x->right = y->left;
    y->left = x;

    x->height = 1 + std::max(nodeHeight(x->left), nodeHeight(x->right));
    y->height = 1 + std::max(nodeHeight(y->left), nodeHeight(y->right));
    return y;
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::rotateRight(Node* y) {
    Node* x = y->left;
    if (!x)
        return y;

    y->left = x->right;
    x->right = y;

    y->height = 1 + std::max(nodeHeight(y->left), nodeHeight(y->right));
    x->height = 1 + std::max(nodeHeight(x->left), nodeHeight(x->right));
    return x;
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::rebalance(Node* n) {
    n->height = 1 + std::max(nodeHeight(n->left), nodeHeight(n->right));
    int bf = balanceFactor(n);

    if (bf > 1) {
        if (balanceFactor(n->left) < 0)
            n->left = rotateLeft(n->left);
        return rotateRight(n);
    }

    if (bf < -1) {
        if (balanceFactor(n->right) > 0)
            n->right = rotateRight(n->right);
        return rotateLeft(n);
    }

    return n;
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::insert(Node* n, const Key& key,
                                     const Value& value, bool& inserted) {
    if (!n) {
        inserted = true;
        return new Node(key, value);
    }

    if (comp_(key, n->key)) {
        n->left = insert(n->left, key, value, inserted);
    } else if (comp_(n->key, key)) {
        n->right = insert(n->right, key, value, inserted);
    } else {
        n->value = value;
        inserted = false;
        return n;
    }

    n->height = 1 + std::max(nodeHeight(n->left), nodeHeight(n->right));
    return rebalance(n);
}

template <typename Key, typename Value, typename Compare>
void AVLTree<Key, Value, Compare>::insert(const Key& key, const Value& value) {
    bool inserted = false;
    root_ = insert(root_, key, value, inserted);
    if (inserted)
        ++size_;
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::minimum(Node* n) {
    while (n && n->left)
        n = n->left;
    return n;
}

template <typename Key, typename Value, typename Compare>
typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::erase(Node* n, const Key& key, bool& removed) {
    if (!n) {
        removed = false;
        return nullptr;
    }

    if (comp_(key, n->key)) {
        n->left = erase(n->left, key, removed);
    } else if (comp_(n->key, key)) {
        n->right = erase(n->right, key, removed);
    } else {
        if (!n->left || !n->right) {
            Node* temp = n->left ? n->left : n->right;
            delete n;
            removed = true;
            return temp;
        }

        Node* successor = minimum(n->right);
        n->key = successor->key;
        n->value = successor->value;
        n->right = erase(n->right, successor->key, removed);
        removed = true;
    }

    if (!n)
        return nullptr;

    n->height = 1 + std::max(nodeHeight(n->left), nodeHeight(n->right));
    return rebalance(n);
}

template <typename Key, typename Value, typename Compare>
bool AVLTree<Key, Value, Compare>::erase(const Key& key) {
    bool removed = false;
    root_ = erase(root_, key, removed);
    if (removed)
        --size_;
    return removed;
}

template <typename Key, typename Value, typename Compare>
const typename AVLTree<Key, Value, Compare>::Node*
AVLTree<Key, Value, Compare>::findNode(const Node* n, const Key& key) const {
    while (n) {
        if (comp_(key, n->key))
            n = n->left;
        else if (comp_(n->key, key))
            n = n->right;
        else
            return n;
    }
    return nullptr;
}

template <typename Key, typename Value, typename Compare>
bool AVLTree<Key, Value, Compare>::contains(const Key& key) const {
    return findNode(root_, key) != nullptr;
}

template <typename Key, typename Value, typename Compare>
const Value* AVLTree<Key, Value, Compare>::find(const Key& key) const {
    const Node* n = findNode(root_, key);
    return n ? &n->value : nullptr;
}

template <typename Key, typename Value, typename Compare>
void AVLTree<Key, Value, Compare>::inorder(const Node* n,
                                           std::vector<Key>& out) {
    if (!n)
        return;
    inorder(n->left, out);
    out.push_back(n->key);
    inorder(n->right, out);
}

template <typename Key, typename Value, typename Compare>
std::vector<Key> AVLTree<Key, Value, Compare>::inorder() const {
    std::vector<Key> out;
    inorder(root_, out);
    return out;
}

template <typename Key, typename Value, typename Compare>
bool AVLTree<Key, Value, Compare>::validate(const Node* n, const Key* lo,
                                            const Key* hi, int& computedHeight) const {
    if (!n) {
        computedHeight = 0;
        return true;
    }

    if (lo && !comp_(*lo, n->key))
        return false;
    if (hi && !comp_(n->key, *hi))
        return false;

    int leftHeight = 0;
    int rightHeight = 0;
    if (!validate(n->left, lo, &n->key, leftHeight))
        return false;
    if (!validate(n->right, &n->key, hi, rightHeight))
        return false;

    if ((leftHeight - rightHeight) > 1 || (rightHeight - leftHeight)> 1)
        return false;

    computedHeight = 1 + std::max(leftHeight, rightHeight);
    return n->height == computedHeight;
}

template <typename Key, typename Value, typename Compare>
bool AVLTree<Key, Value, Compare>::isValidAVL() const {
    int height = 0;
    return validate(root_, nullptr, nullptr, height);
}

// ========================= SplayTree =========================

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::destroy(Node* n) {
    if (!n)
        return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

template <typename Key, typename Value, typename Compare>
SplayTree<Key, Value, Compare>::~SplayTree() {
    destroy(root_);
}

template <typename Key, typename Value, typename Compare>
SplayTree<Key, Value, Compare>::SplayTree(SplayTree&& other) noexcept
    : root_(other.root_), size_(other.size_), comp_(std::move(other.comp_)) {
    other.root_ = nullptr;
    other.size_ = 0;
}

template <typename Key, typename Value, typename Compare>
SplayTree<Key, Value, Compare>&
SplayTree<Key, Value, Compare>::operator=(SplayTree&& other) noexcept {
    if (this != &other) {
        destroy(root_);
        root_ = other.root_;
        size_ = other.size_;
        comp_ = std::move(other.comp_);
        other.root_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename Key, typename Value, typename Compare>
bool SplayTree<Key, Value, Compare>::empty() const noexcept {
    return size_ == 0;
}

template <typename Key, typename Value, typename Compare>
std::size_t SplayTree<Key, Value, Compare>::size() const noexcept {
    return size_;
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::rotateLeft(Node* x) {
    Node* y = x->right;
    if (!y)
        return;

    x->right = y->left;
    if (y->left)
        y->left->parent = x;

    y->left = x;
    Node* parent = x->parent;
    y->parent = parent;
    x->parent = y;

    if (!parent) {
        root_ = y;
    } else if (parent->left == x) {
        parent->left = y;
    } else {
        parent->right = y;
    }
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::rotateRight(Node* x) {
    Node* y = x->left;
    if (!y)
        return;

    x->left = y->right;
    if (y->right)
        y->right->parent = x;

    y->right = x;
    Node* parent = x->parent;
    y->parent = parent;
    x->parent = y;

    if (!parent) {
        root_ = y;
    } else if (parent->left == x) {
        parent->left = y;
    } else {
        parent->right = y;
    }
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::splay(Node* x) {
    if (!x)
        return;

    while (x->parent) {
        Node* p = x->parent;
        Node* g = p->parent;

        if (!g) {
            if (x == p->left)
                rotateRight(p);
            else
                rotateLeft(p);
        } else if (x == p->left && p == g->left) {
            rotateRight(g);
            rotateRight(p);
        } else if (x == p->right && p == g->right) {
            rotateLeft(g);
            rotateLeft(p);
        } else if (x == p->right && p == g->left) {
            rotateLeft(p);
            rotateRight(g);
        } else {
            rotateRight(p);
            rotateLeft(g);
        }
    }
}

template <typename Key, typename Value, typename Compare>
typename SplayTree<Key, Value, Compare>::Node*
SplayTree<Key, Value, Compare>::findNode(const Key& key) {
    Node* curr = root_;
    Node* last = nullptr;

    while (curr) {
        last = curr;
        if (comp_(key, curr->key)) {
            curr = curr->left;
        } else if (comp_(curr->key, key)) {
            curr = curr->right;
        } else {
            splay(curr);
            return curr;
        }
    }

    if (last)
        splay(last);
    return nullptr;
}

template <typename Key, typename Value, typename Compare>
bool SplayTree<Key, Value, Compare>::contains(const Key& key) {
    return findNode(key) != nullptr;
}

template <typename Key, typename Value, typename Compare>
const Value* SplayTree<Key, Value, Compare>::find(const Key& key) {
    Node* n = findNode(key);
    return n ? &n->value : nullptr;
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::insert(const Key& key,
                                            const Value& value) {
    Node* curr = root_;
    Node* parent = nullptr;

    while (curr) {
        parent = curr;
        if (comp_(key, curr->key)) {
            curr = curr->left;
        } else if (comp_(curr->key, key)) {
            curr = curr->right;
        } else {
            curr->value = value;
            splay(curr);
            return;
        }
    }

    Node* newNode = new Node(key, value);
    newNode->parent = parent;
    if (!parent) {
        root_ = newNode;
    } else if (comp_(key, parent->key)) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    splay(newNode);
    ++size_;
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::transplant(Node* u, Node* v) {
    if (!u->parent) {
        root_ = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }

    if (v)
        v->parent = u->parent;
}

template <typename Key, typename Value, typename Compare>
typename SplayTree<Key, Value, Compare>::Node*
SplayTree<Key, Value, Compare>::subtreeMinimum(Node* n) {
    while (n && n->left)
        n = n->left;
    return n;
}

template <typename Key, typename Value, typename Compare>
bool SplayTree<Key, Value, Compare>::erase(const Key& key) {
    Node* target = findNode(key);
    if (!target)
        return false;

    Node* left = target->left;
    Node* right = target->right;

    if (!left && !right) {
        root_ = nullptr;
    } else if (!left) {
        root_ = right;
        right->parent = nullptr;
    } else if (!right) {
        root_ = left;
        left->parent = nullptr;
    } else {
        Node* minRight = subtreeMinimum(right);
        if (minRight != right) {
            Node* parentMin = minRight->parent;
            parentMin->left = minRight->right;
            if (minRight->right)
                minRight->right->parent = parentMin;

            minRight->right = right;
            right->parent = minRight;
        }

        minRight->left = left;
        left->parent = minRight;
        minRight->parent = nullptr;
        root_ = minRight;
    }

    delete target;
    --size_;
    return true;
}

template <typename Key, typename Value, typename Compare>
const Key* SplayTree<Key, Value, Compare>::rootKey() const noexcept {
    return root_ ? &root_->key : nullptr;
}

template <typename Key, typename Value, typename Compare>
void SplayTree<Key, Value, Compare>::inorder(const Node* n,
                                             std::vector<Key>& out) {
    if (!n)
        return;
    inorder(n->left, out);
    out.push_back(n->key);
    inorder(n->right, out);
}

template <typename Key, typename Value, typename Compare>
std::vector<Key> SplayTree<Key, Value, Compare>::inorder() const {
    std::vector<Key> out;
    inorder(root_, out);
    return out;
}

template <typename Key, typename Value, typename Compare>
std::size_t SplayTree<Key, Value, Compare>::depth(const Node* n) {
    if (!n)
        return 0;
    return 1 + std::max(depth(n->left), depth(n->right));
}

template <typename Key, typename Value, typename Compare>
std::size_t SplayTree<Key, Value, Compare>::depth() const noexcept {
    return depth(root_);
}

} // namespace project
