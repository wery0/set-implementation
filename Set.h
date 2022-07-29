/*
 * Template analogue of std::set, based on AVL tree.
 * Template type must have operator <.
 * It supports standard set operations in guaranteed O(log(tree_size)):
 * - insert
 * - erase
 * - find
 * - lower_bound
 */
template<class T>
class Set {
  public:
    
    // Invariant of root!=null is needed for always having node, representing end() iterator.
    Set() { root = new Node(); }

    Set(const Set<T>& other) {
        root = new Node();
        for (const T& val : other) {
            insert(val);
        }
    }

    // Assignment operator
    Set& operator=(const Set& other) {
        if (&other == this) {
            return *this;
        }
        destroy();
        root->left = nullptr;
        node_count = 0;
        for (T val : other) {
            insert(val);
        }
        return *this;
    }

    Set(const std::initializer_list<T>& elems) {
        root = new Node();
        for (const T& val : elems) {
            insert(val);
        }
    }

    template<typename Iterator>
    Set(const Iterator first, const Iterator last) {
        root = new Node();
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    class iterator;

    // If needed value exists, returns iterator on corresponding node, otherwise end().
    iterator find(const T& val) const {
        Node* cur = root->left;
        while (cur != nullptr) {
            if (val < cur->value) {
                cur = cur->left;
            } else if (cur->value < val) {
                cur = cur->right;
            } else {
                return iterator(cur);
            }
        }
        return end();
    }

    // Returns iterator on node with the lowest key >= val.
    iterator lower_bound(const T& val) const {
        Node* cur = root->left;
        Node* ans = root;
        while (cur) {
            if (cur->value < val) {
                cur = cur->right;
            } else {
                ans = cur;
                cur = cur->left;
            }
        }
        return iterator(ans);
    }

    iterator begin() const {
        Node* ans = root;
        while (ans->left) {
            ans = ans->left;
        }
        return iterator(ans);
    }

    iterator end() const { return iterator(root); }

    // Inserts element in tree. If such exists, does nothing.
    void insert(const T& val) {
        root->left = recursive_insert(root->left, val);
        root->left->parent = root;
    }

    // Erases element from tree. If such doesn't exist, does nothing.
    void erase(const T& val) {
        root->left = recursive_erase(root->left, val);
        if (root->left) root->left->parent = root;
    }

    size_t size() const { return node_count; }

    bool empty() const { return node_count == 0; }

    ~Set() { destroy(root); }

  private:
    // Auxiliary class for storing node's information.
    struct Node {
        static constexpr int32_t UNDEFINED = -1;
        static constexpr int32_t LEFT = 0;
        static constexpr int32_t RIGHT = -1;
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
        int32_t height = UNDEFINED;
        T value;

        Node() = default;

        explicit Node(const T& val) {
            value = val;
            height = 0;
        }

        explicit Node(Node* other) {
            parent = other->parent;
            left = other->left;
            right = other->right;
            height = other->height;
            value = other->value;
        }
    };

  public:
    /*
     * Bidirectional iterator for AVL tree nodes
     * Doesn't support random access
     * Prefix/postfix increment/decrement works in amortized O(1)
     * */
    class iterator {
      public:
        iterator() = default;

        explicit iterator(Node* node_) { node = node_; }

        bool operator==(const iterator& it) const { return it.node == node; }

        bool operator==(const iterator&& it) const { return it.node == node; }

        bool operator!=(const iterator& it) const { return it.node != node; }

        bool operator!=(const iterator&& it) const { return it.node != node; }

        T operator*() const { return node->value; }

        T* operator->() const { return &node->value; }

        iterator& operator++() {
            node = get_next_vertex(node);
            return *this;
        }

        iterator operator++(int) {
            iterator temp = iterator(node);
            ++(*this);
            return temp;
        }

        iterator& operator--() {
            node = get_prev_vertex(node);
            return *this;
        }

        iterator operator--(int) {
            iterator temp = iterator(node);
            --(*this);
            return temp;
        }

      private:
        int get_parent_direction(Node* child) {
            return child->parent->left == child ? Node::LEFT : Node::RIGHT;
        }

        Node* get_leftest_node(Node* node) {
            while (node->left != nullptr) {
                node = node->left;
            }
            return node;
        }

        Node* get_rightest_node(Node* node) {
            while (node->right) {
                node = node->right;
            }
            return node;
        }

        // Auxiliary method for finding next node in tree.
        Node* get_next_vertex(Node* node) {
            if (node->right != nullptr) {
                return get_leftest_node(node->right);
            }
            while (node->parent) {
                int32_t dir = get_parent_direction(node);
                node = node->parent;
                if (dir == Node::LEFT) {
                    return node;
                }
            }
            return nullptr;
        }

        // Auxiliary method for finding previous node in tree.
        Node* get_prev_vertex(Node* node) {
            if (node->left != nullptr) {
                return get_rightest_node(node->left);
            }
            while (node->parent) {
                int32_t dir = get_parent_direction(node);
                node = node->parent;
                if (dir == Node::RIGHT) {
                    return node;
                }
            }
            return nullptr;
        }

        Node* node = nullptr;
    };

  private:
    Node* get_leftest_node(Node* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    int32_t get_height(Node* node) { return node == nullptr ? Node::UNDEFINED : node->height; }

    int32_t height_difference(Node* node) {
        return node ? get_height(node->left) - get_height(node->right) : 0;
    }

    // Auxiliary function for deleting tree and releasing memory.
    void destroy(Node* node) {
        if (node == nullptr) {
            return;
        }
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void destroy() { destroy(root->left); }

    void update_height(Node* node) {
        node->height = 1 + std::max(get_height(node->left), get_height(node->right));
    }

    Node* left_rotate(Node* node) {
        Node* temp = node->right;
        node->right = temp->left;
        if (temp->left) {
            temp->left->parent = node;
        }
        temp->left = node;
        temp->parent = node->parent;
        node->parent = temp;
        update_height(node);
        update_height(temp);
        return temp;
    }

    Node* right_rotate(Node* node) {
        Node* temp = node->left;
        node->left = temp->right;
        if (temp->right) {
            temp->right->parent = node;
        }
        temp->right = node;
        temp->parent = node->parent;
        node->parent = temp;
        update_height(node);
        update_height(temp);
        return temp;
    }

    Node* big_left_rotate(Node* node) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    Node* big_right_rotate(Node* node) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Standard AVL's rotate implementation.
    Node* rotate(Node* node) {
        int32_t n_diff = height_difference(node);
        int32_t l_diff = height_difference(node->left);
        int32_t r_diff = height_difference(node->right);
        if (n_diff == -TWO) {
            if (r_diff == ONE) {
                node = big_left_rotate(node);
            } else {
                node = left_rotate(node);
            }
        } else if (n_diff == TWO) {
            if (l_diff == -ONE) {
                node = big_right_rotate(node);
            } else {
                node = right_rotate(node);
            }
        }
        return node;
    }

    // Auxiliary function for erasing element from tree.
    Node* recursive_erase(Node* node, const T& val) {
        if (node == nullptr) {
            return node;
        }
        if (val < node->value) {
            node->left = recursive_erase(node->left, val);
            if (node->left) {
                node->left->parent = node;
            }
        } else if (node->value < val) {
            node->right = recursive_erase(node->right, val);
            if (node->right) {
                node->right->parent = node;
            }
        } else {
            if (node->left && node->right) {
                Node* temp = get_leftest_node(node->right);
                node->value = temp->value;
                node->right = recursive_erase(node->right, temp->value);
                if (node->right) {
                    node->right->parent = node;
                }
            } else {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                --node_count;
                node = temp;
                if (node) {
                    node->parent = nullptr;
                }
            }
        }
        if (node == nullptr) {
            return node;
        }
        node = rotate(node);
        update_height(node);
        return node;
    }

    // Auxiliary function for inserting element from tree.
    Node* recursive_insert(Node* node, const T& val) {
        if (node == nullptr) {
            node = new Node(val);
            ++node_count;
        } else if (val < node->value) {
            node->left = recursive_insert(node->left, val);
            node->left->parent = node;
        } else if (node->value < val) {
            node->right = recursive_insert(node->right, val);
            node->right->parent = node;
        }
        node = rotate(node);
        update_height(node);
        return node;
    }

    size_t node_count = 0;
    Node* root;
    static constexpr int32_t ONE = 1;
    static constexpr int32_t TWO = 2;
};
