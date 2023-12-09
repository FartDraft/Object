#include <iostream>
#include "inc/DoublyCircularLinkedList.hh"
#include "inc/Fraction.hh"

int
main() {
    Fraction fraction = 11.0 / 5;
    int arr[] = {1, 2, 3, 4, 5};
    DoublyCircularLinkedList<int> list{arr, 5};

    std::cout << "fraction: " << fraction << std::endl << "list: " << list << std::endl;

    return 0;
}
