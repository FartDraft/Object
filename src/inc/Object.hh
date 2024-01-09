#include <sstream>
#include "DoublyCircularLinkedList.hh"
#include "Fraction.hh"

struct Object {
    virtual std::string name(void) const = 0;
    virtual size_t id(void) const = 0;
    virtual Object* copy(void) const = 0;
    virtual void from_string(std::string s) = 0;
    virtual std::string to_string(void) const = 0;
    virtual ~Object() = default;

    bool
    operator==(const Object& other) const {
        return typeid(*this) == typeid(other);
    }

    size_t
    operator+(const Object& other) const {
        return this->id() + other.id();
    }
};

class FractionDB : public Object, public Fraction {
  public:
    using Fraction::Fraction;

    std::string
    name(void) const override {
        return "Fraction";
    }

    size_t
    id(void) const override {
        return typeid(*this).hash_code();
    }

    FractionDB*
    copy(void) const override {
        FractionDB* new_ptr = new FractionDB(*this);
        return new_ptr;
    }

    void
    from_string(std::string s) override {
        this->assign(s);
    }

    std::string
    to_string(void) const override {
        return std::string(*this);
    }

    ~FractionDB() = default;
};

template <typename T>
class DoublyCircularLinkedListDB : public Object, public DoublyCircularLinkedList<T> {
  public:
    using DoublyCircularLinkedList<T>::DoublyCircularLinkedList;

    std::string
    name(void) const override {
        return "DoublyCircularLinkedList";
    }

    size_t
    id(void) const override {
        return typeid(*this).hash_code();
    }

    DoublyCircularLinkedListDB<T>*
    copy(void) const override {
        DoublyCircularLinkedListDB<T>* new_ptr = new DoublyCircularLinkedListDB<T>(*this);
        return new_ptr;
    }

    void
    from_string(std::string s) override {
        DoublyCircularLinkedListDB<std::string> new_;
        std::string tmp = "";
        for (char c : s) {
            if (c == ' ') {
                new_.push_back(tmp);
                tmp = "";
            } else {
                tmp += c;
            }
        }
    }

    std::string
    to_string(void) const override {
        struct node {
            node* prev;
            T value;
            node* next;
        };

        std::stringstream result;

        result << "Head <-> ";
        if (this->head != nullptr) {
            result << this->head->value << " <-> ";
            for (node* curr = (node*)this->head->next; curr != (node*)this->head; curr = curr->next) {
                result << curr->value << " <-> ";
            }
        }
        result << "Head";
        return result.str();
    }

    ~DoublyCircularLinkedListDB() { this->clear(); };
};
