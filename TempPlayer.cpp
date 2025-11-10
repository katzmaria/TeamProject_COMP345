#include "Player.h"
#include <algorithm>
#include <iostream>
#include "Orders.h" 
#include "Map.h"   
#include "Cards.h"         
// default constructor 
Player::Player()
: name_(new std::string("Unnamed")),
  territories_(new std::vector<Territory*>()),
  hand_(nullptr),
  orders_(nullptr) {}
  reinforcementPool_(new int(0)) {}

// parameterized constructor 
Player::Player(const std::string& name)
: name_(new std::string(name)),
  territories_(new std::vector<Territory*>()),
  hand_(nullptr),
  orders_(nullptr) {}
  reinforcementPool_(new int(0)) {}

// copy constructor (deep copy for Hand/OrdersList; shallow for Territory* list)
Player::Player(const Player& other)
: name_(new std::string(*other.name_)),
  territories_(new std::vector<Territory*>(*other.territories_)),
  hand_(other.hand_ ? new Hand(*other.hand_) : nullptr),
  orders_(other.orders_ ? new OrdersList(*other.orders_) : nullptr)
  reinforcementPool_(new int(*other.reinforcementPool_)) 
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
    return *this;
}

// destructor
Player::~Player() {
    delete name_;
    delete territories_;
    delete hand_;
    delete orders_;
    delete reinforcementPool_;
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

// ----- simple order issuing (now works with your current Orders.cpp) -----
Order* Player::issueOrder(const std::string& kind /*, Territory* src, Territory* tgt, int armies */) {
    if (!orders_) orders_ = new OrdersList();

    Order* created = nullptr;
    if      (kind == "deploy")    created = new Deploy();
    else if (kind == "advance")   created = new Advance();
    else if (kind == "bomb")      created = new Bomb();
    else if (kind == "blockade")  created = new Blockade();
    else if (kind == "airlift")   created = new Airlift();
    else if (kind == "negotiate") created = new Negotiate();

    if (created) {
        orders_->add(created);
        return created;
    }
    return nullptr;
}

// toString()
std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "Player{" << *p.name_ << "} owns " << p.territories_->size() << " territories, "
       << (p.hand_ ? "has a hand" : "no hand") << ", and "
       << (p.orders_ ? "has an orders list" : "no orders list");
    return os;
}
