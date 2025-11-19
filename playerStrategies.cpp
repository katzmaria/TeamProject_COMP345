#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"

#include <algorithm>
#include <iostream>
#include <set>

// ========================
// PlayerStrategy (base)
// ========================
PlayerStrategy::PlayerStrategy()
    : name_(new std::string("UnnamedStrategy")) {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other)
    : name_(new std::string(*other.name_)) {}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        delete name_;
        name_ = new std::string(*other.name_);
    }
    return *this;
}

PlayerStrategy::~PlayerStrategy() {
    delete name_;
}

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

// ========================
// HumanPlayerStrategy
// ========================
HumanPlayerStrategy::HumanPlayerStrategy() {
    *name_ = "Human";
}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other)
    : PlayerStrategy(other) {}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

HumanPlayerStrategy::~HumanPlayerStrategy() = default;

PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}

std::vector<Territory*> HumanPlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) return result;

    const auto& own = *terrs;
    result.insert(result.end(), own.begin(), own.begin() + (own.size() / 2));
    return result;
}

std::vector<Territory*> HumanPlayerStrategy::toAttack(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs || terrs->empty()) return result;

    const auto& own = *terrs;
    result.insert(result.end(), own.begin() + (own.size() / 2), own.end());
    return result;
}

// This is basically your old Player::issueOrder, moved here and adapted to use Player*
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

    // ==================== DEPLOY ====================
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

        int amount;
        std::cout << "How many armies to deploy? (1.." << available << "): ";
        std::cin >> amount;
        if (amount <= 0 || amount > available) {
            std::cout << "Invalid amount.\n";
            return nullptr;
        }

        Territory* target = terrs->at(idx);

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

    // ==================== ADVANCE ====================
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

    // ==================== AIRLIFT ====================
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

    // ==================== BOMB ====================
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

    // ==================== BLOCKADE ====================
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

    // ==================== NEGOTIATE ====================
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

// ========================
// AggressivePlayerStrategy
// ========================
AggressivePlayerStrategy::AggressivePlayerStrategy() {
    *name_ = "Aggressive";
}

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other)
    : PlayerStrategy(other) {}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

AggressivePlayerStrategy::~AggressivePlayerStrategy() = default;

PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

std::vector<Territory*> AggressivePlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    result.assign(terrs->begin(), terrs->end());
    std::sort(result.begin(), result.end(), cmpArmiesDesc);
    return result;
}

std::vector<Territory*> AggressivePlayerStrategy::toAttack(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;

    // choose enemy neighbors of strongest territory
    if (terrs->empty()) return result;
    Territory* strongest = *std::max_element(terrs->begin(), terrs->end(), cmpArmiesAsc);

    for (Territory* n : strongest->neighbors) {
        if (n->owner != player) {
            result.push_back(n);
        }
    }
    return result;
}

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

    // ===== deploy: dump everything on strongest =====
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

    // ===== advance: always attack from strongest towards enemies if possible =====
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

    // For simplicity, aggressive strategy ignores card-based kinds here
    std::cout << "[Aggressive] Ignoring non-deploy/advance order kind: " << kind << "\n";
    return nullptr;
}

// ========================
// BenevolentPlayerStrategy
// ========================
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

    // ===== deploy: put everything on weakest =====
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

    // ===== advance: move from stronger owned territory to weaker owned neighbor =====
    if (kind == "advance") {
        // find pair (source, target) both owned by player where source has more armies than target
        Territory* source = nullptr;
        Territory* target = nullptr;

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

// ========================
// NeutralPlayerStrategy
// ========================
NeutralPlayerStrategy::NeutralPlayerStrategy() {
    *name_ = "Neutral";
}

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other)
    : PlayerStrategy(other) {}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

NeutralPlayerStrategy::~NeutralPlayerStrategy() = default;

PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}

std::vector<Territory*> NeutralPlayerStrategy::toDefend(Player* /*player*/) const {
    return {};
}

std::vector<Territory*> NeutralPlayerStrategy::toAttack(Player* /*player*/) const {
    return {};
}

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

// ========================
// CheaterPlayerStrategy
// ========================
CheaterPlayerStrategy::CheaterPlayerStrategy() {
    *name_ = "Cheater";
}

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other)
    : PlayerStrategy(other) {}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
    }
    return *this;
}

CheaterPlayerStrategy::~CheaterPlayerStrategy() = default;

PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}

std::vector<Territory*> CheaterPlayerStrategy::toDefend(Player* player) const {
    std::vector<Territory*> result;
    const auto* terrs = player->territories();
    if (!terrs) return result;
    result.assign(terrs->begin(), terrs->end());
    return result;
}

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
    performCheat(player);
    // No actual Order object is created in this simple implementation
    return nullptr;
}
