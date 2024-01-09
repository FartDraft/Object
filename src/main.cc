#include <iostream>
#include "inc/Fraction.hh"
#include "inc/Object.hh"

int
main() {
    FractionDB fraction = 11.0 / 5;
    int arr[] = {1, 2, 3, 4, 5};
    DoublyCircularLinkedListDB<int> list{arr, 5};
    FractionDB fraction2 = 8.0 / 20;
    DoublyCircularLinkedListDB list2{list};
    list2.sort(false);

    std::cout << "fraction: " << fraction << std::endl << "list: " << list << std::endl;
    std::cout << "fraction2: " << fraction2 << std::endl << "list2: " << list2 << std::endl;

    Object* objects[] = {&fraction, &list, &fraction2, &list2};

    std::cout << (*objects[0] == *objects[3]) << std::endl;
    std::cout << (*objects[0] == *objects[2]) << std::endl;
    std::cout << (*objects[0] + *objects[2]) << std::endl;

    Object* new_obj = objects[0]->copy();
    std::cout << *(FractionDB*)new_obj << std::endl;

    FractionDB fraction3;
    Object* new_obj2 = &fraction3;
    new_obj2->from_string("3 14/100");
    std::cout << *(FractionDB*)new_obj2 << std::endl;
    std::cout << new_obj2->to_string() << std::endl;

    for (Object* object : objects) {
        std::cout << "Name: " << object->name() << std::endl;
        std::cout << "Id: " << object->id() << std::endl;
        std::cout << "To string: " << object->to_string() << std::endl << std::endl;
    }

    delete new_obj;
    return 0;
}
