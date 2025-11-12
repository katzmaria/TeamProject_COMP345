#include "Player.h"
#include <algorithm>
#include <iostream>
#include <map>
#include "Orders.h" 
#include "Map.h"   
#include "Cards.h"   

//default const
Player::Player()
    : name_(new std::string("Unnamed")),
      territories_(new std::vector<Territory*>()),
      hand_(nullptr),
      orders_(nullptr),
      reinforcementPool_(new int(0)),
      committedReinforcements_(new int(0)),
      conqueredThisTurn_(new bool(false)) {}
//param constructor
Player::Player(const std::string& name)
    : name_(new std::string(name)),
      territories_(new std::vector<Territory*>()),
      hand_(nullptr),
      orders_(nullptr),
      reinforcementPool_(new int(0)),
      committedReinforcements_(new int(0)),
      conqueredThisTurn_(new bool(false)) {}

// copy constructor (deep copy for Hand/OrdersList; shallow for Territory* list)
Player::Player(const Player& other)
    : name_(new std::string(*other.name_)),
      territories_(new std::vector<Territory*>(*other.territories_)),
      hand_(other.hand_ ? new Hand(*other.hand_) : nullptr),
      orders_(other.orders_ ? new OrdersList(*other.orders_) : nullptr),
      reinforcementPool_(new int(*other.reinforcementPool_)),
      committedReinforcements_(new int(*other.committedReinforcements_)),
      conqueredThisTurn_(new bool(*other.conqueredThisTurn_)) {}


// copy assignment
Player& Player::operator=(const Player& other) {
    if (this == &other) return *this;

    // copy simple members (reusing existing allocations)
    *name_ = *other.name_;
    *territories_ = *other.territories_; // shallow copy of Territory* list

    // replace owned pointer members
    delete hand_;
    delete orders_;
    hand_   = other.hand_   ? new Hand(*other.hand_)         : nullptr;
    orders_ = other.orders_ ? new OrdersList(*other.orders_) : nullptr;
    *reinforcementPool_ = *other.reinforcementPool_;
    *committedReinforcements_ = *other.committedReinforcements_;
    *conqueredThisTurn_ = *other.conqueredThisTurn_;
    return *this;
}

// destructor
Player::~Player() {
    delete name_;
    delete territories_;
    delete hand_;
    delete orders_;
    delete reinforcementPool_;
    delete committedReinforcements_;
    delete conqueredThisTurn_;
}

// ----- getters/setters -----
const std::string& Player::name() const { 
    return *name_; 
}

void Player::setName(const std::string& n) {
    *name_ = n;
}

Hand* Player::hand() const { 
    return hand_; 
}

void Player::setHand(Hand* h) {
    if (h == hand_) return;
    delete hand_;
    hand_ = h; // take ownership
}

OrdersList* Player::orders() const { 
    return orders_; 
}

void Player::setOrders(OrdersList* ol) {
    if (ol == orders_) return;
    delete orders_;
    orders_ = ol; // take ownership
}

int Player::getReinforcementPool() const {
    return *reinforcementPool_;
}

void Player::setReinforcementPool(int value) {
    *reinforcementPool_ = value;
}

void Player::addReinforcements(int delta) {
    *reinforcementPool_ += delta;
    if (*reinforcementPool_ < 0) *reinforcementPool_ = 0;  // just incase
}

// ----- territory management -----
void Player::addTerritory(Territory* t) {
    if (!t) return;
    auto& vec = *territories_;
    if (std::find(vec.begin(), vec.end(), t) == vec.end()) {
        vec.push_back(t);
        std::cout << "DEBUG: Added territory " << t->name << " to " << *name_ 
                  << ". Total territories: " << vec.size() << "\n";
    }
}

void Player::removeTerritory(Territory* t) {
    if (!t) return;
    auto& vec = *territories_;
    auto it = std::remove(vec.begin(), vec.end(), t);
    vec.erase(it, vec.end());
}

const std::vector<Territory*>* Player::territories() const {
    return territories_;
}

// ----- battle planning stubs (placeholder logic) -----
std::vector<Territory*> Player::toDefend() const {
    std::vector<Territory*> result;
    const auto& own = *territories_;
    if (own.empty()) return result;
    result.insert(result.end(), own.begin(), own.begin() + (own.size() / 2));
    return result;
}

std::vector<Territory*> Player::toAttack() const {
    std::vector<Territory*> result;
    const auto& own = *territories_;
    if (own.empty()) return result;
    result.insert(result.end(), own.begin() + (own.size() / 2), own.end());
    return result;
}

// changed version
Order* Player::issueOrder(const std::string& kind, Deck* deck) {
    if (!orders_) {
        orders_ = new OrdersList();
    }

    if (kind == "deploy") {
        int available = getAvailableReinforcements();
        if (available <= 0) {
            std::cout << *name_ << " has no available reinforcement armies.\n";
            return nullptr;
        }

        const auto* terrs = territories();
        if (!terrs || terrs->empty()) {
            std::cout << *name_ << " has no territories to deploy to.\n";
            return nullptr;
        }

        std::cout << "\nDeploy order for " << *name_ << "\n";
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

        Order* created = new Deploy(this, target, amount);
        orders_->add(created);
        
        // Commit these reinforcements so they can't be used again this turn
        commitReinforcements(amount);
        
        // Execute immediately so armies appear on map right away
        std::cout << "\n--- Executing Deploy Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";
        
        std::cout << "Committed " << amount << " armies. Available now: " 
                  << getAvailableReinforcements() << "\n";

        return created;
    }
    
    if (kind == "advance") {
        const auto* terrs = territories();
        if (!terrs || terrs->empty()) {
            std::cout << *name_ << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\n=== Advance order for " << *name_ << " ===\n";
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
            
            std::string relation = (n->owner == this) ? "[YOUR TERRITORY]" : "[ENEMY]";
            
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
        
        // Show what will happen
        if (target->owner == this) {
            std::cout << "→ This will MOVE " << amount << " armies to your own territory (reinforcement)\n";
        } else {
            std::cout << "→ This will ATTACK " << target->name << " with " << amount << " armies!\n";
            std::cout << "   Battle: " << amount << " attackers vs " << target->armies << " defenders\n";
        }

        Order* created = new Advance(this, source, target, amount);
        orders_->add(created);
        
        // Execute immediately so battle/movement happens right away
        std::cout << "\n--- Executing Advance Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";
        
        return created;
    }
    
    if (kind == "airlift") {
        // Check if player has the airlift card
        if (!hand_) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }
        
        bool hasCard = false;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "airlift") {
                hasCard = true;
                break;
            }
        }
        
        if (!hasCard) {
            std::cout << "You don't have an airlift card!\n";
            return nullptr;
        }
        
        const auto* terrs = territories();
        if (!terrs || terrs->size() < 2) {
            std::cout << *name_ << " needs at least 2 territories for airlift.\n";
            return nullptr;
        }

        std::cout << "\nAirlift order for " << *name_ << "\n";
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

        // Find and remove the airlift card from hand
        Card* usedCard = nullptr;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "airlift") {
                usedCard = c;
                break;
            }
        }
        
        if (usedCard) {
            hand_->removeCard(usedCard);
            std::cout << "Used airlift card from hand.\n";
            
            // Return card to deck
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Airlift(this, source, target, amount);
        orders_->add(created);
        
        // Execute immediately
        std::cout << "\n--- Executing Airlift Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";
        
        return created;
    }
    
    if (kind == "bomb") {
        // Check if player has the bomb card
        if (!hand_) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }
        
        bool hasCard = false;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "bomb") {
                hasCard = true;
                break;
            }
        }
        
        if (!hasCard) {
            std::cout << "You don't have a bomb card!\n";
            return nullptr;
        }
        
        const auto* terrs = territories();
        if (!terrs || terrs->empty()) {
            std::cout << *name_ << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\nBomb order for " << *name_ << "\n";
        std::cout << "Select TARGET territory to bomb (must be enemy territory adjacent to yours):\n";
        
        // Collect all enemy territories adjacent to player's territories
        std::vector<Territory*> adjacentEnemies;
        std::map<Territory*, int> territoryIndexMap;
        
        for (Territory* ownedTerritory : *terrs) {
            for (Territory* neighbor : ownedTerritory->neighbors) {
                // Check if neighbor is NOT owned by this player
                if (neighbor->owner != this) {
                    // Check if not already in list
                    if (std::find(adjacentEnemies.begin(), adjacentEnemies.end(), neighbor) == adjacentEnemies.end()) {
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
            std::string ownerName = adjacentEnemies[i]->owner ? adjacentEnemies[i]->owner->name() : "Neutral";
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

        // Find and remove the bomb card from hand
        Card* usedCard = nullptr;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "bomb") {
                usedCard = c;
                break;
            }
        }
        
        if (usedCard) {
            hand_->removeCard(usedCard);
            std::cout << "Used bomb card from hand.\n";
            
            // Return card to deck
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Bomb(this, target);
        orders_->add(created);
        
        // Execute immediately
        std::cout << "\n--- Executing Bomb Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";
        
        return created;
    }
    
    // Handle special card-based orders
    if (kind == "blockade") {
        // Check if player has the blockade card
        if (!hand_) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }
        
        bool hasCard = false;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "blockade") {
                hasCard = true;
                break;
            }
        }
        
        if (!hasCard) {
            std::cout << "You don't have a blockade card!\n";
            return nullptr;
        }
        
        const auto* terrs = territories();
        if (!terrs || terrs->empty()) {
            std::cout << *name_ << " has no territories.\n";
            return nullptr;
        }

        std::cout << "\nBlockade order for " << *name_ << "\n";
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

        // Find and remove the blockade card from hand
        Card* usedCard = nullptr;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == "blockade") {
                usedCard = c;
                break;
            }
        }
        
        if (usedCard) {
            hand_->removeCard(usedCard);
            std::cout << "Used blockade card from hand.\n";
            
            // Return card to deck
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }

        Order* created = new Blockade(this, target);
        orders_->add(created);
        
        // Execute immediately
        std::cout << "\n--- Executing Blockade Order ---\n";
        created->execute();
        std::cout << created->getAction() << "\n";
        
        return created;
    }
    
    if (kind == "negotiate") {
        // Check if player has the card
        if (!hand_) {
            std::cout << "You don't have any cards!\n";
            return nullptr;
        }
        
        bool hasCard = false;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == kind) {
                hasCard = true;
                break;
            }
        }
        
        if (!hasCard) {
            std::cout << "You don't have a " << kind << " card!\n";
            return nullptr;
        }
        
        // Find and remove the card from hand
        Card* usedCard = nullptr;
        for (Card* c : hand_->getCards()) {
            if (c->getType() == kind) {
                usedCard = c;
                break;
            }
        }
        
        if (usedCard) {
            hand_->removeCard(usedCard);
            std::cout << "Used " << kind << " card from hand.\n";
            
            // Return card to deck
            if (deck) {
                deck->addCard(usedCard);
                std::cout << "Card returned to deck.\n";
            }
        }
        
        std::cout << "Card-based orders (bomb, blockade, negotiate) are not fully implemented yet.\n";
        std::cout << "For now, only 'deploy' and 'advance' orders work.\n";
        return nullptr;
    }
    
    Order* created = nullptr;
    if      (kind == "bomb")      created = new Bomb();
    else if (kind == "blockade")  created = new Blockade();
    else if (kind == "negotiate") created = new Negotiate();

    if (created) {
        orders_->add(created);
        return created;
    }

    std::cout << "Unknown order type: " << kind << "\n";
    return nullptr;
}


// toString()
std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "Player{" << *p.name_ << "} owns " << p.territories_->size() << " territories, "
       << (p.hand_ ? "has a hand" : "no hand") << ", and "
       << (p.orders_ ? "has an orders list" : "no orders list");
    return os;
}

// Get available reinforcements (pool minus what's been committed this turn)
int Player::getAvailableReinforcements() const {
    return *reinforcementPool_ - *committedReinforcements_;
}

// Commit reinforcements during issue phase
void Player::commitReinforcements(int amount) {
    *committedReinforcements_ += amount;
}

// Reset committed at start of execute phase
void Player::resetCommitted() {
    *committedReinforcements_ = 0;
}

// Check if player conquered a territory this turn
bool Player::hasConqueredThisTurn() const {
    return *conqueredThisTurn_;
}

// Set conquered flag
void Player::setConqueredThisTurn(bool value) {
    *conqueredThisTurn_ = value;
}

