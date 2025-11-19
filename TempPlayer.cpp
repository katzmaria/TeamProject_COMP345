#include "Player.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include "Orders.h" 
#include "Map.h"   
#include "Cards.h"   
#include "PlayerStrategies.h"


//default const
Player::Player()
    : name_(new std::string("Unnamed")),
      territories_(new std::vector<Territory*>()),
      hand_(nullptr),
      orders_(nullptr),
      reinforcementPool_(new int(0)),
      committedReinforcements_(new int(0)),
      conqueredThisTurn_(new bool(false)),
      diplomaticRelations_(new std::set<Player*>()),
      strategy_(nullptr) // NEW, inside the list
{}


//param constructor
Player::Player(const std::string& name)
    : name_(new std::string(name)),
      territories_(new std::vector<Territory*>()),
      hand_(nullptr),
      orders_(nullptr),
      reinforcementPool_(new int(0)),
      committedReinforcements_(new int(0)),
      conqueredThisTurn_(new bool(false)),
      diplomaticRelations_(new std::set<Player*>()),
      strategy_(nullptr) // NEW
{}



// copy constructor (deep copy for Hand/OrdersList; shallow for Territory* list)
Player::Player(const Player& other)
    : name_(new std::string(*other.name_)),
      territories_(new std::vector<Territory*>(*other.territories_)),
      hand_(other.hand_ ? new Hand(*other.hand_) : nullptr),
      orders_(other.orders_ ? new OrdersList(*other.orders_) : nullptr),
      reinforcementPool_(new int(*other.reinforcementPool_)),
      committedReinforcements_(new int(*other.committedReinforcements_)),
      conqueredThisTurn_(new bool(*other.conqueredThisTurn_)),
      diplomaticRelations_(new std::set<Player*>(*other.diplomaticRelations_)),
      strategy_(other.strategy_ ? other.strategy_->clone() : nullptr)  // NEW
{}

      
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
    *diplomaticRelations_ = *other.diplomaticRelations_;

    // NEW: copy strategy
    delete strategy_;
    strategy_ = other.strategy_ ? other.strategy_->clone() : nullptr;

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
    delete diplomaticRelations_;
    
    delete strategy_;  // NEW

}

//implement strategy accessors
PlayerStrategy* Player::strategy() const {
    return strategy_;
}

void Player::setStrategy(PlayerStrategy* s) {
    if (strategy_ == s) return;
    delete strategy_;
    strategy_ = s; // take ownership
}



// getters/setters 
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
    if (*reinforcementPool_ < 0) *reinforcementPool_ = 0;  // just incase negative val
}

// territory management
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
std::vector<Territory*> Player::toDefend() const {
    if (!strategy_) {
        // fallback: no strategy -> empty list
        return {};
    }
    // Strategy might not need to modify the player, so we cast away const safely
    return strategy_->toDefend(const_cast<Player*>(this));
}

std::vector<Territory*> Player::toAttack() const {
    if (!strategy_) {
        return {};
    }
    return strategy_->toAttack(const_cast<Player*>(this));
}

Order* Player::issueOrder(const std::string& kind, Deck* deck, const std::vector<Player*>* allPlayers) {
    if (!strategy_) {
        std::cout << "Player " << name() << " has no strategy set; cannot issue order.\n";
        return nullptr;
    }
    return strategy_->issueOrder(this, kind, deck, allPlayers);
}



// toString()
std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "Player{" << *p.name_ << "} owns " << p.territories_->size() << " territories, "
       << (p.hand_ ? "has a hand" : "no hand") << ", and "
       << (p.orders_ ? "has an orders list" : "no orders list");
    return os;
}

// Get available reinforcements pool (minus what's been committed this turn)
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

// Add diplomatic relation (for negotiate orders)
void Player::addDiplomaticRelation(Player* player) {
    if (player && player != this) {
        diplomaticRelations_->insert(player);
    }
}

// Check if there's a diplomatic relation with a player
bool Player::hasDiplomaticRelation(Player* player) const {
    return diplomaticRelations_->find(player) != diplomaticRelations_->end();
}

// Clear diplomatic relations (call at start of turn)
void Player::clearDiplomaticRelations() {
    diplomaticRelations_->clear();
}

