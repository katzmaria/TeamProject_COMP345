#include "Orders.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>

void testOrdersLists() {
    OrdersList list;

    Deploy deploy; 
    list.add(&deploy); 


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

void testOrderExecution() {
    std::cout << "\n========================================\n";
    std::cout << "TESTING ORDER EXECUTION\n";
    std::cout << "========================================\n";

    
    Player* player1 = new Player("Player1");
    Player* player2 = new Player("Player2");
    
    
    std::vector<std::string> emptyNeighbors;
    Territory* t1 = new Territory(1, "Territory1", 0, 0, emptyNeighbors);
    Territory* t2 = new Territory(2, "Territory2", 0, 0, emptyNeighbors);
    Territory* t3 = new Territory(3, "Territory3", 0, 0, emptyNeighbors);
    Territory* t4 = new Territory(4, "Territory4", 0, 0, emptyNeighbors);
    Territory* t5 = new Territory(5, "Territory5", 0, 0, emptyNeighbors);
    
    
    t1->addNeighbor(t2);
    t2->addNeighbor(t1);
    t2->addNeighbor(t3);
    t3->addNeighbor(t2);
    t3->addNeighbor(t4);
    t4->addNeighbor(t3);
    t4->addNeighbor(t5);
    t5->addNeighbor(t4);
    
    
    t1->setOwner(player1);
    player1->addTerritory(t1);
    t1->armies = 10;
    
    t2->setOwner(player1);
    player1->addTerritory(t2);
    t2->armies = 15;
    
    t3->setOwner(player2);
    player2->addTerritory(t3);
    t3->armies = 8;
    
    t4->setOwner(player2);
    player2->addTerritory(t4);
    t4->armies = 12;
    
    t5->setOwner(player2);
    player2->addTerritory(t5);
    t5->armies = 5;
    
    
    player1->setReinforcementPool(10);
    player2->setReinforcementPool(10);
    
    std::cout << "\n--- Initial Setup ---\n";
    std::cout << "Player1 owns: Territory1 (10 armies), Territory2 (15 armies)\n";
    std::cout << "Player2 owns: Territory3 (8 armies), Territory4 (12 armies), Territory5 (5 armies)\n";

    
    std::cout << "\n========================================\n";
    std::cout << "TEST 1: Deploy Order Validation\n";
    std::cout << "========================================\n";
    
    Deploy* deploy1 = new Deploy(player1, t1, 5);
    if (deploy1->validate()) {
        std::cout << "[PASS] Deploy order is valid\n";
        deploy1->execute();
        std::cout << "Result: " << deploy1->getAction() << "\n";
    }
    player1->setReinforcementPool(5);
    
    Deploy* deploy2 = new Deploy(player1, t3, 5);
    if (!deploy2->validate()) {
        std::cout << "[PASS] Deploy correctly rejected (not owned)\n";
    }
    
   
    std::cout << "\n========================================\n";
    std::cout << "TEST 2: Advance Order - Movement\n";
    std::cout << "========================================\n";
    
    Advance* advance1 = new Advance(player1, t1, t2, 5);
    if (advance1->validate()) {
        std::cout << "[PASS] Advance order validates\n";
        advance1->execute();
        std::cout << "Result: " << advance1->getAction() << "\n";
    }

   
    std::cout << "\n========================================\n";
    std::cout << "TEST 3: Territory Conquest\n";
    std::cout << "========================================\n";
    
    player1->setConqueredThisTurn(false);
    Advance* advance2 = new Advance(player1, t2, t3, 15);
    if (advance2->validate()) {
        advance2->execute();
        if (t3->owner == player1) {
            std::cout << "[PASS] Territory ownership transferred\n";
            std::cout << "[PASS] Conquered flag set: " << player1->hasConqueredThisTurn() << "\n";
        }
    }

    
    std::cout << "\n========================================\n";
    std::cout << "TEST 4: Card Reward for Conquest\n";
    std::cout << "========================================\n";
    
    Deck* deck = new Deck();
    Hand* hand1 = new Hand();
    player1->setHand(hand1);
    
    if (player1->hasConqueredThisTurn()) {
        deck->draw(hand1);
        std::cout << "[PASS] Player draws card for conquest\n";
    }

   
    std::cout << "\n========================================\n";
    std::cout << "TEST 5: Bomb Order\n";
    std::cout << "========================================\n";
    
    int armiesBefore = t4->armies;
    Bomb* bomb1 = new Bomb(player1, t4);
    if (bomb1->validate()) {
        std::cout << "[PASS] Bomb order validates\n";
        bomb1->execute();
        if (t4->armies == armiesBefore / 2) {
            std::cout << "[PASS] Half of armies removed\n";
        }
    }
    
    Bomb* bomb2 = new Bomb(player1, t1);
    if (!bomb2->validate()) {
        std::cout << "[PASS] Bomb correctly rejected (own territory)\n";
    }

    
    std::cout << "\n========================================\n";
    std::cout << "TEST 6: Airlift Order\n";
    std::cout << "========================================\n";
    
    Airlift* airlift1 = new Airlift(player1, t1, t3, 3);
    if (airlift1->validate()) {
        std::cout << "[PASS] Airlift validates (non-adjacent OK)\n";
        airlift1->execute();
    }
    
    Airlift* airlift2 = new Airlift(player1, t1, t4, 3);
    if (!airlift2->validate()) {
        std::cout << "[PASS] Airlift correctly rejected (not owned)\n";
    }

   
    std::cout << "\n========================================\n";
    std::cout << "TEST 7: Negotiate Order\n";
    std::cout << "========================================\n";
    
    Negotiate* negotiate1 = new Negotiate(player1, player2);
    if (negotiate1->validate()) {
        std::cout << "[PASS] Negotiate validates\n";
        negotiate1->execute();
    }
    
    Advance* advance3 = new Advance(player1, t3, t4, 5);
    if (advance3->validate()) {
        advance3->execute();
        if (!advance3->isExecuted()) {
            std::cout << "[PASS] Attack prevented by diplomacy\n";
        }
    }

   
    std::cout << "\n========================================\n";
    std::cout << "TEST 8: Blockade Order\n";
    std::cout << "========================================\n";
    
    int armiesBeforeBlockade = t1->armies;
    Blockade* blockade1 = new Blockade(player1, t1);
    if (blockade1->validate()) {
        std::cout << "[PASS] Blockade validates\n";
        blockade1->execute();
        if (t1->owner->name() == "Neutral") {
            std::cout << "[PASS] Ownership transferred to Neutral\n";
        }
        if (t1->armies == armiesBeforeBlockade * 2) {
            std::cout << "[PASS] Armies doubled\n";
        }
    }

    
    std::cout << "\n========================================\n";
    std::cout << "ALL TESTS COMPLETED\n";
    std::cout << "========================================\n";

    
    delete deploy1; delete deploy2;
    delete advance1; delete advance2; delete advance3;
    delete bomb1; delete bomb2;
    delete airlift1; delete airlift2;
    delete negotiate1;
    delete blockade1;
    delete player1; delete player2;
    delete t1; delete t2; delete t3; delete t4; delete t5;
    delete deck;
}
