#include <iostream>

template<class T>
class Avl {
private:

    struct Node {
        Node *parent = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;
        int32_t height = -1;
        T value;

        Node() = default;

        explicit Node(T val) {
            value = val;
            height = 0;
        }

        explicit Node(Node *n) {
            parent = n->parent;
            left = n->left;
            right = n->right;
            height = n->height;
            value = n->value;
        }
    };

    size_t node_count = 0;
    Node *root = new Node();

    Node *get_leftest_node(Node *n) {
        for (; n->left != nullptr;) {
            n = n->left;
        }
        return n;
    }

    int32_t get_height(Node *n) {
        return n == nullptr ? -1 : n->height;
    }

    int32_t height_difference(Node *n) {
        return n ? get_height(n->left) - get_height(n->right) : 0;
    }

    void destroy(Node *n) {
        if (n == nullptr) {
            return;
        }
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    void destroy() {
        destroy(root->left);
    }

    void update_height(Node *node) {
        node->height = 1 + std::max(get_height(node->left), get_height(node->right));
    }

    Node *left_rotate(Node *a) {
        Node *b = a->right;
        a->right = b->left;
        if (b->left) {
            b->left->parent = a;
        }
        b->left = a;
        b->parent = a->parent;
        a->parent = b;
        update_height(a);
        update_height(b);
        return b;
    }

    Node *right_rotate(Node *a) {
        Node *b = a->left;
        a->left = b->right;
        if (b->right) {
            b->right->parent = a;
        }
        b->right = a;
        b->parent = a->parent;
        a->parent = b;
        update_height(a);
        update_height(b);
        return b;
    }

    Node *big_left_rotate(Node *n) {
        n->right = right_rotate(n->right);
        return left_rotate(n);
    }

    Node *big_right_rotate(Node *n) {
        n->left = left_rotate(n->left);
        return right_rotate(n);
    }

    Node *rotate(Node *node) {
        int32_t n_diff = height_difference(node);
        int32_t l_diff = height_difference(node->left);
        int32_t r_diff = height_difference(node->right);
        if (n_diff == -2) {
            if (r_diff == 1) {
                node = big_left_rotate(node);
            } else {
                node = left_rotate(node);
            }
        } else if (n_diff == 2) {
            if (l_diff == -1) {
                node = big_right_rotate(node);
            } else {
                node = right_rotate(node);
            }
        }
        return node;
    }

    Node *recursive_erase(Node *node, T val) {
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
                Node *temp = get_leftest_node(node->right);
                node->value = temp->value;
                node->right = recursive_erase(node->right, temp->value);
                if (node->right) {
                    node->right->parent = node;
                }
            } else {
                Node *temp = node->left ? node->left : node->right;
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

    Node *recursive_insert(Node *node, T val) {
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

public:

    class iterator {
    public:
        iterator() = default;

        explicit iterator(Node *node_) {
            node = node_;
        }

        bool operator==(iterator &it) const {
            return it.node == node;
        }

        bool operator==(iterator &&it) const {
            return it.node == node;
        }

        bool operator!=(iterator &it) const {
            return it.node != node;
        }

        bool operator!=(iterator &&it) const {
            return it.node != node;
        }

        T operator*() const {
            return node->value;
        }

        T *operator->() const {
            return &node->value;
        }

        iterator &operator++() {
            node = get_next_vertex(node);
            return *this;
        }

        iterator operator++(int) {
            iterator temp = iterator(node);
            ++(*this);
            return temp;
        }

        iterator &operator--() {
            node = get_prev_vertex(node);
            return *this;
        }

        iterator operator--(int) {
            iterator temp = iterator(node);
            --(*this);
            return temp;
        }

    private:

        int get_parent_direction(Node *child) {
            return child->parent->left == child ? 0 : 1;
        }

        Node *get_leftest_node(Node *n) {
            for (; n->left != nullptr;) {
                n = n->left;
            }
            return n;
        }

        Node *get_rightest_node(Node *n) {
            for (; n->right;) {
                n = n->right;
            }
            return n;
        }

        Node *get_next_vertex(Node *n) {
            if (n->right != nullptr) {
                return get_leftest_node(n->right);
            }
            for (; n->parent;) {
                int32_t dir = get_parent_direction(n);
                n = n->parent;
                if (dir == 0) {
                    return n;
                }
            }
            return nullptr;
        }

        Node *get_prev_vertex(Node *n) {
            if (n->left != nullptr) {
                return get_rightest_node(n->left);
            }
            for (; n->parent;) {
                int32_t dir = get_parent_direction(n);
                n = n->parent;
                if (dir == 1) {
                    return n;
                }
            }
            return nullptr;
        }

        Node *node;
    };

    Avl() = default;

    Avl(const Avl<T> &other) {
        for (T val: other) {
            insert(val);
        }
    }

    Avl &operator=(const Avl &other) {
        if (&other == this) {
            return *this;
        }
        destroy();
        root->left = nullptr;
        node_count = 0;
        for (T val: other) {
            insert(val);
        }
        return *this;
    }

    Avl(std::initializer_list<T> elems) {
        for (T val: elems) {
            insert(val);
        }
    }

    template<typename Iterator>
    Avl(Iterator first, Iterator last) {
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    iterator find(const T &val) const {
        Node *n = root->left;
        for (; n != nullptr;) {
            if (val < n->value) {
                n = n->left;
            } else if (n->value < val) {
                n = n->right;
            } else {
                return iterator(n);
            }
        }
        return end();
    }

    iterator lower_bound(const T &val) const {
        Node *n = root->left;
        Node *ans = root;
        for (; n;) {
            if (n->value < val) {
                n = n->right;
            } else {
                ans = n;
                n = n->left;
            }
        }
        return iterator(ans);
    }

    iterator begin() const {
        Node *ans = root;
        for (; ans->left;) {
            ans = ans->left;
        }
        return iterator(ans);
    }

    iterator end() const {
        return iterator(root);
    }

    void insert(T val) {
        root->left = recursive_insert(root->left, val);
        root->left->parent = root;
    }

    void erase(T val) {
        root->left = recursive_erase(root->left, val);
        if (root->left) root->left->parent = root;
    }

    size_t size() const {
        return node_count;
    }

    bool empty() const {
        return node_count == 0;
    }

    ~Avl() {
        destroy(root);
    }
};

template<class T>
class Set {
private:
    Avl<T> avl;
public:

    Set() = default;

    Set(const Set<T> &other) {
        avl = other.avl;
    }

    Set &operator=(const Set &other) {
        avl = other.avl;
        return *this;
    }

    Set(std::initializer_list<T> elems) {
        avl = elems;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        avl = Avl<T>(first, last);
    }

    typedef typename Avl<T>::iterator iterator;

    iterator find(const T &val) const {
        return avl.find(val);
    }

    iterator lower_bound(const T &val) const {
        return avl.lower_bound(val);
    }

    iterator begin() {
        return avl.begin();
    }

    iterator end() {
        return avl.end();
    }

    iterator begin() const {
        return avl.begin();
    }

    iterator end() const {
        return avl.end();
    }

    void insert(T val) {
        avl.insert(val);
    }

    void erase(T val) {
        avl.erase(val);
    }

    size_t size() const {
        return avl.size();
    }

    bool empty() const {
        return avl.empty();
    }

    ~Set() = default;
};