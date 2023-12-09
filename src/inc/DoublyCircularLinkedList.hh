#pragma once
#include <cstdint>
#include <iostream>

template <typename T>
class DoublyCircularLinkedList {
  public:
    struct node {
        node* prev;
        T value;
        node* next;
    };

    node* head = nullptr;
    uint64_t size = 0;

    constexpr DoublyCircularLinkedList() {}

    constexpr DoublyCircularLinkedList(T* values, uint64_t size) {
        for (uint64_t i = 0; i < size; ++i) {
            push_back(values[i]);
        }
    }

    template <typename Iterator>
    constexpr DoublyCircularLinkedList(const Iterator& it) {
        for (const T& value : it) {
            push_back(value);
        }
    }

    explicit constexpr DoublyCircularLinkedList(const DoublyCircularLinkedList& other) {
        if (other.head != nullptr) {
            push_back(other.head->value);
            for (node* curr = other.head->next; curr != other.head; curr = curr->next) {
                push_back(curr->value);
            }
        }
    }

    ~DoublyCircularLinkedList() { clear(); }

    constexpr void
    clear() {
        if (head != nullptr) {
            uint64_t i = 0;
            for (node *curr = head->next, *next = head->next->next; i < size - 1; ++i, curr = next, next = next->next) {
                delete curr;
            }
            delete head;
        }
        head = nullptr;
        size = 0;
    }

    friend std::ostream&
    operator<<(std::ostream& os, const DoublyCircularLinkedList<T>& obj) {
        os << "Head <-> ";
        if (obj.head != nullptr) {
            os << obj.head->value << " <-> ";
            for (node* curr = obj.head->next; curr != obj.head; curr = curr->next) {
                os << curr->value << " <-> ";
            }
        }
        os << "Head" << '\n';
        return os;
    }

    constexpr node*
    push_back(const T& value) {
        ++size;
        if (head == nullptr) {
            node* new_node = new node{nullptr, value, nullptr};
            new_node->prev = new_node->next = new_node;
            return head = new_node;
        }
        node* new_node = new node{head->prev, value, head};
        head->prev->next = new_node;
        head->prev = new_node;
        return head;
    }

    constexpr node*
    push_front(const T& value) {
        return head = push_back(value)->prev;
    }

    constexpr node*
    get_node(int64_t index) {
        if (head == nullptr) {
            return nullptr;
        }
        index = (index < 0 ? -1 : 1) * (static_cast<uint64_t>(index < 0 ? -index : index) % size);
        if (index == 0) {
            return head;
        }
        node* node = head;
        for (int64_t i = 0; i < (index < 0 ? -index : index); ++i) {
            node = index < 0 ? node->prev : node->next;
        }
        return node;
    }

    constexpr T
    pop_node(int64_t index) {
        if (head == nullptr) {
            return T{};
        }
        --size;
        node* node = get_node(index);
        node->prev->next = node->next;
        node->next->prev = node->prev;
        T value = node->value;
        delete node;
        return value;
    }

    node*
    max_node() {
        if (head == nullptr) {
            return nullptr;
        }
        node* max = head;
        for (node* curr = head->next; curr != head; curr = curr->next) {
            if (max->value < curr->value) {
                max = curr;
            }
        }
        return max;
    }

    node*
    min_node() {
        if (head == nullptr) {
            return nullptr;
        }
        node* min = head;
        for (node* curr = head->next; curr != head; curr = curr->next) {
            if (min->value > curr->value) {
                min = curr;
            }
        }
        return min;
    }

    constexpr node*
    insert_node(int64_t index, const T& value, bool left = true) {
        ++size;
        if (head == nullptr) {
            node* new_node = new node{nullptr, value, nullptr};
            new_node->prev = new_node->next = new_node;
            return head = new_node;
        }
        node* dst = get_node(index);
        node* new_node = new node{dst, value, dst};
        if (left) {
            dst->prev->next = new_node;
            new_node->prev = dst->prev;
            dst->prev = new_node;
        } else {
            dst->next->prev = new_node;
            new_node->next = dst->next;
            dst->next = new_node;
        }

        return new_node;
    }

    // Concatenates the other doubly circular linked list at the given index to the right.
    // Default index -1 means the end of the list.
    DoublyCircularLinkedList<T>*
    concatenate(const DoublyCircularLinkedList<T>& other, int64_t index = -1) {
        if (head == nullptr) {
            if (other.head == nullptr) {
                return nullptr;
            }
            push_back(other.head->value);
            for (node* curr = other.head->next; curr != other.head; curr = curr->next) {
                push_back(curr->value);
            }
            return this;
        }
        size += other.size;
        node* dst_node = get_node(index);
        node* new_node = new node{dst_node, other.head->value, dst_node->next};
        dst_node->next->prev = new_node;
        dst_node->next = new_node;
        dst_node = new_node;
        for (node* curr = other.head->next; curr != other.head; curr = curr->next) {
            new_node = new node{dst_node, curr->value, dst_node->next};
            dst_node->next->prev = new_node;
            dst_node->next = new_node;
            dst_node = new_node;
        }
        return this;
    }

    DoublyCircularLinkedList<T>*
    sort(bool ascending = true) {
        if (head == nullptr) {
            return this;
        }
        T key;
        for (node *prev, *curr = head->next; curr != head; curr = curr->next) {
            key = curr->value;
            prev = curr->prev;

            while (prev != head->prev and (ascending ? prev->value > key : prev->value < key)) {
                prev->next->value = prev->value;
                prev = prev->prev;
            }
            prev->next->value = key;
        }

        return this;
    }

    static constexpr int
    cmp(const DoublyCircularLinkedList<T>& a, const DoublyCircularLinkedList<T>& b) {
        if (a.head == nullptr and b.head == nullptr) {
            return 0;
        }
        if (a.head == nullptr) {
            return -1;
        }
        if (b.head == nullptr) {
            return 1;
        }
        if (a.head->value == b.head->value) {
            node *node_a, *node_b;
            for (node_a = a.head->next, node_b = b.head->next; node_a != a.head and node_b != b.head;
                 node_a = node_a->next, node_b = node_b->next) {
                if (node_a->value != node_b->value) {
                    return node_a->value < node_b->value ? -1 : 1;
                }
            }
            if (node_a == a.head and node_b == b.head) {
                return 0;
            }
            if (node_a == a.head) {
                return -1;
            }
            if (node_b == b.head) {
                return 1;
            }
        }
        return a.head->value < b.head->value ? -1 : 1;
    }

    friend constexpr bool
    operator==(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) == 0;
    }

    friend constexpr bool
    operator!=(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) != 0;
    }

    friend constexpr bool
    operator<(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) < 0;
    }

    friend constexpr bool
    operator>(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) > 0;
    }

    friend constexpr bool
    operator<=(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) <= 0;
    }

    friend constexpr bool
    operator>=(const DoublyCircularLinkedList& lhs, const DoublyCircularLinkedList& rhs) {
        return cmp(lhs, rhs) >= 0;
    }
};
