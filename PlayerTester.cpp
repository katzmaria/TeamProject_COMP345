
#include <iostream>
#include "Player.h"
#include "Orders.h"
#include "Map.h"

int main() {
    std::cout << "=== PLAYER TESTER START ===" << std::endl;

    // Create a player
    Player* p1 = new Player("Justin");

    // Create some dummy territories (normally they come from a Map)
    Territory* t1 = new Territory(0, "Montreal", 0, 0, {});
    Territory* t2 = new Territory(1, "Toronto", 0, 0, {});
    Territory* t3 = new Territory(2, "Ottawa", 0, 0, {});

    // Assign them to the player
    p1->addTerritory(t1);
    p1->addTerritory(t2);
    p1->addTerritory(t3);

    // Display basic player info
    std::cout << "\n" << *p1 << std::endl;

    // Show territories to defend and to attack
    std::cout << "\nTo Defend:" << std::endl;
    for (auto* terr : p1->toDefend())
        std::cout << "  - " << terr->name << std::endl;

    std::cout << "\nTo Attack:" << std::endl;
    for (auto* terr : p1->toAttack())
        std::cout << "  - " << terr->name << std::endl;

    // Issue different types of orders
    std::cout << "\nIssuing Orders..." << std::endl;
    p1->issueOrder("deploy");
    p1->issueOrder("advance");
    p1->issueOrder("bomb");
    p1->issueOrder("blockade");
    p1->issueOrder("airlift");
    p1->issueOrder("negotiate");

    // Display all orders
    std::cout << "\nAll Orders:" << std::endl;
    p1->orders()->display();

    // Execute all orders
    std::cout << "\nExecuting All Orders..." << std::endl;
    p1->orders()->executeAllOrders();

    // Show orders after execution
    std::cout << "\nOrders After Execution:" << std::endl;
    p1->orders()->display();

    std::cout << "\n=== PLAYER TESTER END ===" << std::endl;

    delete p1;
    return 0;
}
