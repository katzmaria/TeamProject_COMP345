#include "playerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

#include <algorithm>
#include <iostream>
#include <set>

// PlayerStrategy (base stratetgy)

//default constructor 
PlayerStrategy::PlayerStrategy()
    : name_(new std::string("UnnamedStrategy")) {}

    //copy const 
PlayerStrategy::PlayerStrategy(const PlayerStrategy& other)
    : name_(new std::string(*other.name_)) {}

    //copy ass - deep copy name from other 
PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        delete name_;
        name_ = new std::string(*other.name_);
    }
    return *this;
}
//destructor
PlayerStrategy::~PlayerStrategy() {
    delete name_;
}
//getter for strat name 
const std::string& PlayerStrategy::name() const {
    return *name_;
}

// Utility helpers for sorting by armies
static bool cmpArmiesAsc(const Territory* a, const Territory* b) {
    return a->armies < b->armies;
}

static bool cmpArmiesDesc(const Territory* a, const Territory* b) {
    return a->armies > b->armies;
}

// HumanPlayerStrategy
HumanPlayerStrategy::HumanPlayerStrategy() {
    *name_ = "Human";
}
//copy const
HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other)
    : PlayerStrategy(other) {}

//copy assignemnt
HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}
//destructor 
HumanPlayerStrategy::~HumanPlayerStrategy() = default;

//deep copy clone
PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}
// returns first half of owned terrs
std::vector<Territory*> HumanPlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) return result;

    const auto& own = *terrs;
    result.insert(result.end(), own.begin(), own.begin() + (own.size() / 2));
    return result;
}

//returns second half of owned terrs 
std::vector<Territory*> HumanPlayerStrategy::toAttack(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) return result;

    const auto& own = *terrs;
    result.insert(result.end(), own.begin() + (own.size() / 2), own.end());
    return result;
}

// This is old Player::issueOrder, moved here and adapted to use Player*
Order* HumanPlayerStrategy::issueOrder(
    Player* player,
    const std::string& kind,
    Deck* deck,
    const std::vector<Player*>* allPlayers
) {
    // Ensure orders list exists
    OrdersList* ol = player->orders();
    if (!ol) {
        ol = new OrdersList();
        player->setOrders(ol);
    }

    // DEPLOY -select a certain terr and number of armies to deploy to it
    //ensure there are enough armies in reinforcement pool
    if (kind == "deploy") {
        int available = player->getAvailableReinforcements();
        if (available <= 0) {
            std::cout << player->name() << " has no available reinforcement armies.\n";
            return nullptr;
        }

        const auto* terrs = player->territories();
        if (!terrs || terrs->empty()) {
            std::cout << player->name() << " has no territories to deploy to.\n";
            return nullptr;
        }
        //ask where to deploy
        std::cout << "\nDeploy order for " << player->name() << "\n";
        std::cout << "Available reinforcements: " << available << "\n";
        std::cout << "Select a territory index:\n";

        for (std::size_t i = 0; i < terrs->size(); ++i) {
            std::cout << i << ") " << terrs->at(i)->name << "\n";
        }

        std::size_t idx;
        std::cin >> idx;
        if (idx >= terrs->size()) {
            std::cout << "Invalid territory index.\n";
            return nullptr;
        }
//how many to deploy
        int amount;
        std::cout << "How many armies to deploy? (1.." << available << "): ";
        std::cin >> amount;
        if (amount <= 0 || amount > available) {
            std::cout << "Invalid amount.\n";
            return nullptr;
        }

        Territory* target = terrs->at(idx);

        //create and exceute order
        Order* created = new Deploy(player, target, amount);
        ol->add(created);

        player->commitReinforcements(amount);

        std::cout << "\n--- Executing Deploy Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        std::cout << "Committed " << amount << " armies. Available now: "
                  << player->getAvailableReinforcements() << "\n";
        return created;
    }

    //  ADVANCE 
    //pick source and target terrs and number of armies
    //to move/attack with
    if (kind == "advance") {
        const auto* terrs = player->territories();
        if (!terrs || terrs->empty()) {
            std::cout << player->name() << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\n=== Advance order for " << player->name() << " ===\n";
        std::cout << "Select SOURCE territory (must be one you own):\n";
        for (std::size_t i = 0; i < terrs->size(); ++i) {
            std::cout << "  [" << i << "] " << terrs->at(i)->name
                      << " (armies: " << terrs->at(i)->armies << ")\n";
        }

        std::size_t srcIdx;
        std::cout << "Enter source index: ";
        std::cin >> srcIdx;
        if (srcIdx >= terrs->size()) {
            std::cout << "Invalid source territory.\n";
            return nullptr;
        }
        Territory* source = terrs->at(srcIdx);

        if (source->armies <= 0) {
            std::cout << "Source has no armies to move.\n";
            return nullptr;
        }

        std::cout << "\nSelect TARGET territory (must be adjacent to " << source->name << "):\n";
        std::cout << "Adjacent territories:\n";
        for (std::size_t i = 0; i < source->neighbors.size(); ++i) {
            Territory* n = source->neighbors[i];

            std::string ownerName = "Neutral";
            if (n->owner) {
                ownerName = n->owner->name();
            }
            std::string relation = (n->owner == player) ? "[YOUR TERRITORY]" : "[ENEMY]";

            std::cout << "  [" << i << "] " << n->name
                      << " - Owner: " << ownerName
                      << " " << relation
                      << " (armies: " << n->armies << ")\n";
        }

        std::size_t tgtIdx;
        std::cout << "Enter target index: ";
        std::cin >> tgtIdx;
        if (tgtIdx >= source->neighbors.size()) {
            std::cout << "Invalid target territory.\n";
            return nullptr;
        }
        Territory* target = source->neighbors[tgtIdx];

        int amount;
        std::cout << "\nHow many armies to advance? (1.." << source->armies << "): ";
        std::cin >> amount;
        if (amount <= 0 || amount > source->armies) {
            std::cout << "Invalid amount.\n";
            return nullptr;
        }

        //clarify if move or attack
        if (target->owner == player) {
            std::cout << "→ This will MOVE " << amount << " armies to your own territory (reinforcement)\n";
        } else {
            std::cout << "→ This will ATTACK " << target->name << " with " << amount << " armies!\n";
            std::cout << "   Battle: " << amount << " attackers vs " << target->armies << " defenders\n";
        }

        Order* created = new Advance(player, source, target, amount);
        ol->add(created);

        std::cout << "\n--- Executing Advance Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        return created;
    }

    //  AIRLIFT 
    //check fro airlift card 
    //pick source and target terrs and number of armies to airlift 
    if (kind == "airlift") {
        Hand* hand = player->hand();
        if (!hand) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }

        bool hasCard = false;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "airlift") {
                hasCard = true;
                break;
            }
        }
        //valiadte that has card
        if (!hasCard) {
            std::cout << "You don't have an airlift card!\n";
            return nullptr;
        }

        const auto* terrs = player->territories();
        if (!terrs || terrs->size() < 2) {
            std::cout << player->name() << " needs at least 2 territories for airlift.\n";
            return nullptr;
        }

        std::cout << "\nAirlift order for " << player->name() << "\n";
        std::cout << "Select SOURCE territory index:\n";
        for (std::size_t i = 0; i < terrs->size(); ++i) {
            std::cout << i << ") " << terrs->at(i)->name
                      << " (armies: " << terrs->at(i)->armies << ")\n";
        }

        std::size_t srcIdx;
        std::cin >> srcIdx;
        if (srcIdx >= terrs->size()) {
            std::cout << "Invalid source territory.\n";
            return nullptr;
        }
        Territory* source = terrs->at(srcIdx);

        std::cout << "\nSelect TARGET territory index (can be non-adjacent):\n";
        for (std::size_t i = 0; i < terrs->size(); ++i) {
            if (i != srcIdx) {
                std::cout << i << ") " << terrs->at(i)->name
                          << " (armies: " << terrs->at(i)->armies << ")\n";
            }
        }

        std::size_t tgtIdx;
        std::cin >> tgtIdx;
        if (tgtIdx >= terrs->size() || tgtIdx == srcIdx) {
            std::cout << "Invalid target territory.\n";
            return nullptr;
        }
        Territory* target = terrs->at(tgtIdx);

        int amount;
        std::cout << "How many armies to airlift? (1.." << source->armies << "): ";
        std::cin >> amount;
        if (amount <= 0 || amount > source->armies) {
            std::cout << "Invalid amount.\n";
            return nullptr;
        }

        //return airlift card to deck after use

        Card* usedCard = nullptr;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "airlift") {
                usedCard = c;
                break;
            }
        }

        if (usedCard) {
            hand->removeCard(usedCard);
            std::cout << "Used airlift card from hand.\n";
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Airlift(player, source, target, amount);
        ol->add(created);

        std::cout << "\n--- Executing Airlift Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        return created;
    }

    //  BOMB
    if (kind == "bomb") {
        Hand* hand = player->hand();
        if (!hand) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }

        bool hasCard = false;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "bomb") {
                hasCard = true;
                break;
            }
        }

        if (!hasCard) {
            std::cout << "You don't have a bomb card!\n";
            return nullptr;
        }

        const auto* terrs = player->territories();
        if (!terrs || terrs->empty()) {
            std::cout << player->name() << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\nBomb order for " << player->name() << "\n";
        std::cout << "Select TARGET territory to bomb (must be enemy territory adjacent to yours):\n";

        std::vector<Territory*> adjacentEnemies;

        for (Territory* ownedTerritory : *terrs) {
            for (Territory* neighbor : ownedTerritory->neighbors) {
                if (neighbor->owner != player) {
                    if (std::find(adjacentEnemies.begin(), adjacentEnemies.end(), neighbor)
                        == adjacentEnemies.end()) {
                        adjacentEnemies.push_back(neighbor);
                    }
                }
            }
        }

        if (adjacentEnemies.empty()) {
            std::cout << "No enemy territories adjacent to your territories!\n";
            return nullptr;
        }

        for (std::size_t i = 0; i < adjacentEnemies.size(); ++i) {
            std::string ownerName = adjacentEnemies[i]->owner ?
                adjacentEnemies[i]->owner->name() : "Neutral";
            std::cout << i << ") " << adjacentEnemies[i]->name
                      << " (armies: " << adjacentEnemies[i]->armies
                      << ", owner: " << ownerName << ")\n";
        }

        std::size_t tgtIdx;
        std::cin >> tgtIdx;
        if (tgtIdx >= adjacentEnemies.size()) {
            std::cout << "Invalid target territory.\n";
            return nullptr;
        }
        Territory* target = adjacentEnemies[tgtIdx];

        Card* usedCard = nullptr;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "bomb") {
                usedCard = c;
                break;
            }
        }

        if (usedCard) {
            hand->removeCard(usedCard);
            std::cout << "Used bomb card from hand.\n";
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Bomb(player, target);
        ol->add(created);

        std::cout << "\n--- Executing Bomb Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        return created;
    }

    //  BLOCKADE 
    //select a terr you own and triple its armies , make it neutral
    if (kind == "blockade") {
        Hand* hand = player->hand();
        if (!hand) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }

        bool hasCard = false;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "blockade") {
                hasCard = true;
                break;
            }
        }

        if (!hasCard) {
            std::cout << "You don't have a blockade card!\n";
            return nullptr;
        }

        const auto* terrs = player->territories();
        if (!terrs || terrs->empty()) {
            std::cout << player->name() << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\nBlockade order for " << player->name() << "\n";
        std::cout << "Select TARGET territory to blockade (must be one you own):\n";

        for (std::size_t i = 0; i < terrs->size(); ++i) {
            std::cout << i << ") " << terrs->at(i)->name
                      << " (armies: " << terrs->at(i)->armies << ")\n";
        }

        std::size_t tgtIdx;
        std::cin >> tgtIdx;
        if (tgtIdx >= terrs->size()) {
            std::cout << "Invalid target territory.\n";
            return nullptr;
        }
        Territory* target = terrs->at(tgtIdx);

        Card* usedCard = nullptr;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "blockade") {
                usedCard = c;
                break;
            }
        }

        if (usedCard) {
            hand->removeCard(usedCard);
            std::cout << "Used blockade card from hand.\n";
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Blockade(player, target);
        ol->add(created);

        std::cout << "\n--- Executing Blockade Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        return created;
    }

    //  NEGOTIATE 
    //build list of other players to negotiate with
    if (kind == "negotiate") {
        Hand* hand = player->hand();
        if (!hand) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }

        bool hasCard = false;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "negotiate") {
                hasCard = true;
                break;
            }
        }

        if (!hasCard) {
            std::cout << "You don't have a negotiate card!\n";
            return nullptr;
        }

        if (!allPlayers || allPlayers->size() < 2) {
            std::cout << "Not enough players to negotiate with!\n";
            return nullptr;
        }

        std::cout << "\nNegotiate order for " << player->name() << "\n";
        std::cout << "Select target PLAYER to negotiate with:\n";

        std::vector<Player*> otherPlayers;
        for (Player* p : *allPlayers) {
            if (p != player) {
                otherPlayers.push_back(p);
            }
        }

        if (otherPlayers.empty()) {
            std::cout << "No other players to negotiate with!\n";
            return nullptr;
        }

        for (std::size_t i = 0; i < otherPlayers.size(); ++i) {
            std::cout << i << ") " << otherPlayers[i]->name() << "\n";
        }

        std::size_t playerIdx;
        std::cin >> playerIdx;
        if (playerIdx >= otherPlayers.size()) {
            std::cout << "Invalid player index.\n";
            return nullptr;
        }
        Player* targetPlayer = otherPlayers[playerIdx];

        Card* usedCard = nullptr;
        for (Card* c : hand->getCards()) {
            if (c->getType() == "negotiate") {
                usedCard = c;
                break;
            }
        }

        if (usedCard) {
            hand->removeCard(usedCard);
            std::cout << "Used negotiate card from hand.\n";
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Negotiate(player, targetPlayer);
        ol->add(created);

        std::cout << "\n--- Executing Negotiate Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";

        return created;
    }

    std::cout << "Unknown order type: " << kind << "\n";
    return nullptr;
}

// aggressive player strategy implementation
AggressivePlayerStrategy::AggressivePlayerStrategy() {
    *name_ = "Aggressive";
}

// copy constructor
AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other)
    : PlayerStrategy(other) {}

// copy assignment
AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

// destructor
AggressivePlayerStrategy::~AggressivePlayerStrategy() = default;

//clone deep copy
PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

// return owned terrs sorted by descending armies
std::vector<Territory*> AggressivePlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    result.assign(terrs->begin(), terrs->end());
    std::sort(result.begin(), result.end(), cmpArmiesDesc);
    return result;
}
//returns enemy neighbors of strongest terr
std::vector<Territory*> AggressivePlayerStrategy::toAttack(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    //Find strongest territory and return enemy neighbors
    if (terrs->empty()) return result;
    Territory* strongest = *std::max_element(terrs->begin(), terrs->end(), cmpArmiesAsc);

    for (Territory* n : strongest->neighbors) {
        if (n->owner != player) {
            result.push_back(n);
        }
    }
    return result;
}

// issue order reimplementation
//only deoploys and advances armies from strongest terr
Order* AggressivePlayerStrategy::issueOrder(
    Player* player,
    const std::string& kind,
    Deck* deck,
    const std::vector<Player*>* allPlayers
) {
    (void)deck;
    (void)allPlayers;

    OrdersList* ol = player->orders();
    if (!ol) {
        ol = new OrdersList();
        player->setOrders(ol);
    }

    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) {
        std::cout << "[Aggressive] " << player->name() << " has no territories.\n";
        return nullptr;
    }

    Territory* strongest = *std::max_element(terrs->begin(), terrs->end(), cmpArmiesAsc);

    // this deploy method deploys all available reinforcements to strongest territory
    if (kind == "deploy") {
        int available = player->getAvailableReinforcements();
        if (available <= 0) {
            std::cout << "[Aggressive] No available reinforcements.\n";
            return nullptr;
        }

        std::cout << "[Aggressive] Deploying " << available
                  << " armies to strongest territory " << strongest->name << "\n";

        Order* created = new Deploy(player, strongest, available);
        ol->add(created);
        player->commitReinforcements(available);
        created->execute();
        std::cout << created->getAction() << "\n";
        return created;
    }

    //advance method attacks from strongest territory to weakest adjacent enememy territory
    if (kind == "advance") {
        // pick enemy neighbor with weakest armies
        Territory* targetEnemy = nullptr;
        for (Territory* n : strongest->neighbors) {
            if (n->owner != player) {
                if (!targetEnemy || n->armies < targetEnemy->armies) {
                    targetEnemy = n;
                }
            }
        }

        if (!targetEnemy) {
            std::cout << "[Aggressive] No adjacent enemies to attack.\n";
            return nullptr;
        }

        //move all but one to leave soome defense 
        int amount = strongest->armies - 1;
        if (amount <= 0) {
            std::cout << "[Aggressive] Strongest territory has no spare armies.\n";
            return nullptr;
        }

        std::cout << "[Aggressive] Attacking " << targetEnemy->name
                  << " from " << strongest->name << " with " << amount << " armies.\n";

        Order* created = new Advance(player, strongest, targetEnemy, amount);
        ol->add(created);
        created->execute();
        std::cout << created->getAction() << "\n";
        return created;
    }

    // ignore other orders
    std::cout << "[Aggressive] Ignoring non-deploy/advance order kind: " << kind << "\n";
    return nullptr;
}

// benevolent player strategy implementation
BenevolentPlayerStrategy::BenevolentPlayerStrategy() {
    *name_ = "Benevolent";
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other)
    : PlayerStrategy(other) {}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

BenevolentPlayerStrategy::~BenevolentPlayerStrategy() = default;

PlayerStrategy* BenevolentPlayerStrategy::clone() const {
    return new BenevolentPlayerStrategy(*this);
}
//returns owned terrs sroteed weakest to strongest 
std::vector<Territory*> BenevolentPlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    result.assign(terrs->begin(), terrs->end());
    std::sort(result.begin(), result.end(), cmpArmiesAsc);
    return result;
}

std::vector<Territory*> BenevolentPlayerStrategy::toAttack(Player* /*player*/) const {
    // Never attacks
    return {};
}

//can only deploy and redistribute armies from its own terrs
Order* BenevolentPlayerStrategy::issueOrder(
    Player* player,
    const std::string& kind,
    Deck* /*deck*/,
    const std::vector<Player*>* /*allPlayers*/
) {
    OrdersList* ol = player->orders();
    if (!ol) {
        ol = new OrdersList();
        player->setOrders(ol);
    }

    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) {
        std::cout << "[Benevolent] " << player->name() << " has no territories.\n";
        return nullptr;
    }

    // put everyyhing on weakest 
    if (kind == "deploy") {
        int available = player->getAvailableReinforcements();
        if (available <= 0) {
            std::cout << "[Benevolent] No available reinforcements.\n";
            return nullptr;
        }

        Territory* weakest = *std::min_element(terrs->begin(), terrs->end(), cmpArmiesAsc);
        std::cout << "[Benevolent] Deploying " << available
                  << " armies to weakest territory " << weakest->name << "\n";

        Order* created = new Deploy(player, weakest, available);
        ol->add(created);
        player->commitReinforcements(available);
        created->execute();
        std::cout << created->getAction() << "\n";
        return created;
    }

    // move armies from strongest to weakest terr
    if (kind == "advance") {
        // find neighbor with biggest army difference
        Territory* source = nullptr;
        Territory* target = nullptr;

        // find source and target pair to move armies from most to least defended
        for (Territory* t : *terrs) {
            for (Territory* n : t->neighbors) {
                if (n->owner == player && t->armies > n->armies) {
                    if (!source || (t->armies - n->armies) > (source->armies - target->armies)) {
                        source = t;
                        target = n;
                    }
                }
            }
        }

        if (!source || !target) {
            std::cout << "[Benevolent] No internal reinforcement move available.\n";
            return nullptr;
        }

        //move half the armies from source to target
        int amount = (source->armies) / 2;
        if (amount <= 0) {
            std::cout << "[Benevolent] Source has too few armies.\n";
            return nullptr;
        }

        std::cout << "[Benevolent] Moving " << amount << " armies from "
                  << source->name << " to " << target->name << "\n";

        Order* created = new Advance(player, source, target, amount);
        ol->add(created);
        created->execute();
        std::cout << created->getAction() << "\n";
        return created;
    }

    std::cout << "[Benevolent] Ignoring non-deploy/advance order kind: " << kind << "\n";
    return nullptr;
}


// neutral player strategy implementation
NeutralPlayerStrategy::NeutralPlayerStrategy() {
    *name_ = "Neutral";
}
// copy constructor
NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other)
    : PlayerStrategy(other) {}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}
// destructor
NeutralPlayerStrategy::~NeutralPlayerStrategy() = default;

PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}
// never defends or attacks
std::vector<Territory*> NeutralPlayerStrategy::toDefend(Player* /*player*/) const {
    return {};
}

std::vector<Territory*> NeutralPlayerStrategy::toAttack(Player* /*player*/) const {
    return {};
}
// does literally nothing
Order* NeutralPlayerStrategy::issueOrder(
    Player* player,
    const std::string& kind,
    Deck* deck,
    const std::vector<Player*>* allPlayers
) {
    (void)player;
    (void)kind;
    (void)deck;
    (void)allPlayers;

    std::cout << "[Neutral] Player does nothing.\n";
    return nullptr;
}


// cheater player strategy implementation
CheaterPlayerStrategy::CheaterPlayerStrategy() {
    *name_ = "Cheater";
}

// copy constructor
CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other)
    : PlayerStrategy(other) {}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}


// destructor
CheaterPlayerStrategy::~CheaterPlayerStrategy() = default;

PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}


// cheater defends all its territories without special order
std::vector<Territory*> CheaterPlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;
    result.assign(terrs->begin(), terrs->end());
    return result;
}

// cheater attacks and automatically wins all adjacent enemy territories 
std::vector<Territory*> CheaterPlayerStrategy::toAttack(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    std::set<Territory*> unique;
    for (Territory* t : *terrs) {
        for (Territory* n : t->neighbors) {
            if (n->owner != player) {
                unique.insert(n);
            }
        }
    }
    result.assign(unique.begin(), unique.end());
    return result;
}

// performs the cheat itself
// conquers all adjacent enemy territories 
void CheaterPlayerStrategy::performCheat(Player* player) {
    std::vector<Territory*> targets = toAttack(player);

    for (Territory* t : targets) {
        Player* oldOwner = t->owner;
        if (oldOwner && oldOwner != player) {
            oldOwner->removeTerritory(t);
        }
        t->owner = player;
        player->addTerritory(t);

        std::cout << "[Cheater] " << player->name()
                  << " automatically conquered " << t->name << "\n";
    }
}

// issue order reimplementation for cheater
//just performs the cheat 
Order* CheaterPlayerStrategy::issueOrder(
    Player* player,
    const std::string& kind,
    Deck* deck,
    const std::vector<Player*>* allPlayers
) {
    (void)kind;
    (void)deck;
    (void)allPlayers;

    std::cout << "[Cheater] Performing cheating conquest.\n";
    // perform the cheat
    performCheat(player);
    return nullptr;
}
