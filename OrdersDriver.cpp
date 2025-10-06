#include "Orders.h"
#include <iostream>

void testOrdersLists() {
    OrdersList list;

    Deploy deploy; 
    Advance advance; 
    Bomb bomb; 
    Blockade blockade; 
    Airlift airlift; 
    Negotiate negotiate;
    list.add(&deploy); 
    list.add(&advance); 
    list.add(&bomb); 
    list.add(&blockade); 
    list.add(&airlift); 
    list.add(&negotiate);

    std::cout << "-----Initial Orders-----\n"; 
    list.display();

    std::cout << "\n-----Executing Deploy Order-----\n"; 
    deploy.execute();
    list.display();

    std::cout << "\n-----Executing All Orders-----\n"; 
    list.executeAllOrders(); 
    list.display();

    std::cout << "\n-----Move Bomb to Position 0-----\n"; 
    list.move(2,0); 
    list.display();

    std::cout << "\n-----Remove Negotiate-----\n"; 
    list.remove(5); 
    list.display();


}

// int main() {
//     testOrdersLists();
//     return 0;
// }
