#include "DoublyCircularLinkedList.hh"
#include "Fraction.hh"

struct Object {
    virtual std::string name(void) = 0;
};

class FractionDB : public Object, public Fraction {
    using Fraction::Fraction;

    std::string
    name(void) {
        return "Fraction";
    }
};

template <typename T>
class DoublyCircularLinkedListDB : public Object, public DoublyCircularLinkedList<T> {
    using DoublyCircularLinkedList<T>::DoublyCircularLinkedList;

    std::string
    name(void) {
        return "DoublyCircularLinkedList";
    }
};
