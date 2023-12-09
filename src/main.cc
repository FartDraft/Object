#include <iostream>
#include "inc/DoublyCircularLinkedList.hh"
#include "inc/Fraction.hh"
#include "inc/Object.hh"

int
main() {
    FractionDB fraction = 11.0 / 5;
    int arr[] = {1, 2, 3, 4, 5};
    DoublyCircularLinkedListDB<int> list{arr, 5};

    std::cout << "fraction: " << fraction << std::endl << "list: " << list << std::endl;

    Object* objects[] = {&fraction, &list};

    for (Object* object : objects) {
        std::cout << "Name: " << object->name() << std::endl;
    }

    return 0;
}
