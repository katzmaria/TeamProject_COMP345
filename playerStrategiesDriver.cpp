#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include <iostream>
#include <vector>

// Helper to print a list of territories
static void printTerritoryList(const std::string& label, const std::vector<Territory*>& terrs) {
    std::cout << label << " [";
    for (std::size_t i = 0; i < terrs.size(); ++i) {
        std::cout << terrs[i]->name << " (armies: " << terrs[i]->armies << ")";
        if (i + 1 < terrs.size()) std::cout << ", ";
    }
    std::cout << "]\n";
}

// Free function required by the assignment
void testPlayerStrategies() {
    std::cout << "=== testPlayerStrategies() ===\n\n";

    // -----------------------------------------------------------------
    // 1. Create a tiny "map" with 4 territories and link neighbors
    // -----------------------------------------------------------------
    Territory* t1 = new Territory(1, "Alpha", 0, 0, std::vector<std::string>{});
    Territory* t2 = new Territory(2, "Beta",  1, 0, std::vector<std::string>{});
    Territory* t3 = new Territory(3, "Gamma", 2, 0, std::vector<std::string>{});
    Territory* t4 = new Territory(4, "Delta", 3, 0, std::vector<std::string>{});

    // Make a simple chain: Alpha - Beta - Gamma - Delta
    t1->neighbors.push_back(t2);
    t2->neighbors.push_back(t1);
    t2->neighbors.push_back(t3);
    t3->neighbors.push_back(t2);
    t3->neighbors.push_back(t4);
    t4->neighbors.push_back(t3);

    // -----------------------------------------------------------------
    // 2. Create players and assign strategies
    // -----------------------------------------------------------------
    Player human("HumanPlayer");
    Player aggro("AggroBot");
    Player bene("BeneBot");
    Player neutral("NeutralBot");
    Player cheater("CheatBot");

    human.setStrategy(new HumanPlayerStrategy());
    aggro.setStrategy(new AggressivePlayerStrategy());
    bene.setStrategy(new BenevolentPlayerStrategy());
    neutral.setStrategy(new NeutralPlayerStrategy());
    cheater.setStrategy(new CheaterPlayerStrategy());

    std::cout << "Human uses: "   << human.strategy()->name()   << "\n";
    std::cout << "Aggro uses: "   << aggro.strategy()->name()   << "\n";
    std::cout << "Bene uses: "    << bene.strategy()->name()    << "\n";
    std::cout << "Neutral uses: " << neutral.strategy()->name() << "\n";
    std::cout << "Cheater uses: " << cheater.strategy()->name() << "\n\n";

    // -----------------------------------------------------------------
    // 3. Assign ownership and armies to territories
    // -----------------------------------------------------------------
    // Human owns Alpha & Beta
    t1->owner = &human;  t1->armies = 5;
    t2->owner = &human;  t2->armies = 2;
    human.addTerritory(t1);
    human.addTerritory(t2);

    // AggroBot owns Gamma
    t3->owner = &aggro;  t3->armies = 8;
    aggro.addTerritory(t3);

    // BeneBot owns Delta
    t4->owner = &bene;   t4->armies = 1;
    bene.addTerritory(t4);

    // CheaterBot owns nothing initially, but is adjacent (through others)
    // it will try to cheat-conquer neighbors of any territories it owns

    // -----------------------------------------------------------------
    // 4. Show toDefend / toAttack behavior
    // -----------------------------------------------------------------
    std::cout << "--- toDefend / toAttack demonstrations ---\n\n";

    printTerritoryList("Human toDefend: ", human.toDefend());
    printTerritoryList("Human toAttack: ", human.toAttack());
    std::cout << "\n";

    printTerritoryList("AggroBot toDefend (strongest first): ", aggro.toDefend());
    printTerritoryList("AggroBot toAttack (neighbors of strongest): ", aggro.toAttack());
    std::cout << "\n";

    printTerritoryList("BeneBot toDefend (weakest first): ", bene.toDefend());
    printTerritoryList("BeneBot toAttack (should be empty): ", bene.toAttack());
    std::cout << "\n";

    printTerritoryList("NeutralBot toDefend (does nothing): ", neutral.toDefend());
    printTerritoryList("NeutralBot toAttack (does nothing): ", neutral.toAttack());
    std::cout << "\n";

    printTerritoryList("CheaterBot toDefend (its territories): ", cheater.toDefend());
    printTerritoryList("CheaterBot toAttack (adjacent enemies): ", cheater.toAttack());
    std::cout << "\n";

    // -----------------------------------------------------------------
    // 5. Show issueOrder() behavior for each strategy
    // -----------------------------------------------------------------
    std::cout << "\n--- issueOrder() demonstrations ---\n\n";

    // Give some reinforcements to players for deploy demos
    aggro.setReinforcementPool(5);
    bene.setReinforcementPool(4);

    // Aggressive: dumps all reinforcements on its strongest territory and attacks
    std::cout << "[AggroBot] Before orders: Gamma has " << t3->armies << " armies\n";
    aggro.issueOrder("deploy", nullptr, nullptr);   // auto, no user input
    aggro.issueOrder("advance", nullptr, nullptr);  // auto attack if possible
    std::cout << "[AggroBot] After orders: Gamma has " << t3->armies
              << " armies (check neighbors for changes too)\n\n";

    // Benevolent: reinforces weakest and only moves armies between its own territories
    std::cout << "[BeneBot] Before orders: Delta has " << t4->armies << " armies\n";
    bene.issueOrder("deploy", nullptr, nullptr);    // auto reinforce weakest
    bene.issueOrder("advance", nullptr, nullptr);   // auto internal reinforcement if possible
    std::cout << "[BeneBot] After orders: Delta has " << t4->armies << " armies\n\n";

    // Cheater: automatically conquers all adjacent enemy territories
    std::cout << "[CheatBot] Before cheating, owns " 
              << cheater.territories()->size() << " territories.\n";
    cheater.issueOrder("advance", nullptr, nullptr);  // kind is ignored in our impl
    std::cout << "[CheatBot] After cheating, owns " 
              << cheater.territories()->size() << " territories.\n\n";

    // Human: would interactively ask user for input.
    // Uncomment to test manually (when you actually run the program and type):
    //
    // std::cout << "[Human] Now demonstrating interactive issueOrder(\"deploy\")...\n";
    // human.setReinforcementPool(3);
    // human.issueOrder("deploy", nullptr, nullptr);
    //

    // -----------------------------------------------------------------
    // 6. Dynamic strategy change demo
    // -----------------------------------------------------------------
    std::cout << "Changing NeutralBot strategy to Aggressive at runtime...\n";
    neutral.setStrategy(new AggressivePlayerStrategy());
    std::cout << "NeutralBot now uses: " << neutral.strategy()->name() << "\n";

    // After giving it a territory & reinforcements, it will behave like aggro
    Territory* t5 = new Territory(5, "Epsilon", 4, 0, std::vector<std::string>{});
    t5->owner = &neutral;
    t5->armies = 6;
    neutral.addTerritory(t5);
    neutral.setReinforcementPool(3);

    printTerritoryList("NeutralBot (now Aggressive) toDefend: ", neutral.toDefend());
    neutral.issueOrder("deploy", nullptr, nullptr);

    // -----------------------------------------------------------------
    // 7. Cleanup
    // -----------------------------------------------------------------
    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;

    std::cout << "\n=== end of testPlayerStrategies() ===\n\n";
}
