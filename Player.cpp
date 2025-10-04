
#include "Player.h"
#include <algorithm>
#include <iostream>
// #include "Cards.h" 
// #include "Orders.h" 
#include "Map.h" 


// default constructor 
Player::Player()
: name_(new std::string("Unnamed")),
territories_(new std::vector<Territory*>()),
hand_(nullptr),
orders_(nullptr) {}

// parametized constructor 
Player::Player(const std::string& name)
: name_(new std::string(name)),
territories_(new std::vector<Territory*>()),
hand_(nullptr),
orders_(nullptr) {}


// copy constructor *********** NEEDS CHANGING 
Player::Player(const Player& other)
: name_(new std::string(*other.name_)),
  territories_(new std::vector<Territory*>(*other.territories_)), // shallow copy of Territory* list
  hand_(other.hand_ ? new Hand(*other.hand_) : nullptr),
  orders_(other.orders_ ? new OrdersList(*other.orders_) : nullptr)
{}

// Operation handling "=" ****** MIGHT NEED CHANGING 
Player& Player::operator=(const Player& other) {
    if (this == &other) return *this; // self-assignment guard

    // copy simple members
    *name_ = *other.name_;
    *territories_ = *other.territories_; // shallow copy of Territory* list

    // delete old hand/orders and deep-copy new ones
    delete hand_;
    delete orders_;

    hand_ = other.hand_ ? new Hand(*other.hand_) : nullptr;
    orders_ = other.orders_ ? new OrdersList(*other.orders_) : nullptr;

    return *this;
}

//Destructor
Player::~Player() {
delete name_;
delete territories_;
delete hand_; // Player owns its Hand
delete orders_; // Player owns its OrdersList
}

// get and set name 
const std::string& Player::name() 

const { return *name_; }

// get and set hand 
Hand* Player::hand() const { return hand_; }

void Player::setHand(Hand* h) {
if (h == hand_) return;
delete hand_;
hand_ = h; // take ownership
}

// get and set orders 
OrdersList* Player::orders() const { return orders_; }

        // deletes old orders list and makes player responsible for the new one
void Player::setOrders(OrdersList* ol) {
if (ol == orders_) return;
delete orders_;
orders_ = ol; // assign new list 
}

// add and remove territories 
void Player::addTerritory(Territory* t) {
if (!t) return;
if (std::find(territories_->begin(), territories_->end(), t) == territories_->end())
territories_->push_back(t);
}


void Player::removeTerritory(Territory* t) {
if (!t) return;
auto it = std::remove(territories_->begin(), territories_->end(), t);
territories_->erase(it, territories_->end());
}

// gets the pointer to the list of territories 
const std::vector<Territory*>* Player::territories() const { 
    return territories_; 
}

// attack and defend methods
std::vector<Territory*> Player::toDefend() const {
std::vector<Territory*> result;
const auto& own = *territories_;
result.insert(result.end(), own.begin(), own.begin() + (own.size() / 2));// For now: return first half of owned territories (arbitrary per spec)
return result;
}

std::vector<Territory*> Player::toAttack() const {
std::vector<Territory*> result;
const auto& own = *territories_;
result.insert(result.end(), own.begin() + (own.size() / 2), own.end());// For now: return second half of owned territories (placeholder)
return result;
}


// UNSURE EXACTLY WHAT TO DO HERE 
// Order* Player::issueOrder(const std::string& kind, Territory* source, Territory* target, int armies) {
// if (!orders_) orders_ = new OrdersList();


// // In Part 3, replace this factory with your concrete Order subclasses.
// Order* created = OrdersList::Make(kind, this, source, target, armies); // e.g., "deploy", "advance", etc.
// if (created) {
// orders_->add(created);
// return created;
// }
// return nullptr;
// }

// toString() 

std::ostream& operator<<(std::ostream& os, const Player& p) {
os << "Player{" << *p.name_ << "} owns " << p.territories_->size() << " territories, "
<< (p.hand_ ? "has a hand" : "no hand") << ", and "
<< (p.orders_ ? "has an orders list" : "no orders list");
return os;
}