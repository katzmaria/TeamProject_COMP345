#include "Orders.h"
#include "Map.h"
#include "Player.h"  // Add this line
#include "Cards.h"   // Add this line
// Order Class
Order::Order(const std::string& n) {
    orderName = n;
    executed = false;
    action = "";
}

// Getters Functions
std::string Order::getOrderName()  { 
    return orderName; 
}
std::string Order::getAction() { 
    return action; 
}
bool Order::isExecuted() { 
    return executed; 
}

// Setters Functions
void Order::setOrderName( std::string& n) { 
    orderName = n; 
}
void Order::setAction(const std::string& e) {
    action = e;
}

void Order::setExecuted(bool e) {
     executed = e; 
    }

/* Determing whether the order is valid or not. */
void Order::execute() {
    if (validate()) {
        executed = true;
        action = orderName + " executed.";
    } else {
        executed = false;
        action = orderName + " failed.";
    }
}
 bool Order::validate() {
    return false; 
}

/* 

Function called when using cout << order.

*/
std::ostream& operator<<(std::ostream& out, const Order& order) {
    out << order.orderName;
    if (order.executed){ 
        out << " || " << order.action;
    }
    return out;
}

//  Orders Subclasses 
Deploy::Deploy()
    : Order("Deploy"), owner_(nullptr), target_(nullptr), armies_(0) {}


Deploy::Deploy(Player* owner, Territory* target, int armies)
    : Order("Deploy"), owner_(owner), target_(target), armies_(armies) {}

bool Deploy::validate() {
    if (!owner_) return false;  // Changed from player
    if (!target_) return false;  // Changed from targetTerritory
    
    // Check player owns the territory
    const auto* terrs = owner_->territories();  // Changed from player
    if (!terrs) return false;
    
    bool ownsTerritory = false;
    for (Territory* t : *terrs) {
        if (t == target_) {  // Changed from targetTerritory
            ownsTerritory = true;
            break;
        }
    }
    
    if (!ownsTerritory) return false;
    
    // Check player has enough reinforcements
    if (owner_->getReinforcementPool() < armies_) return false;  // Changed from player and armies
    
    return true;
}

void Deploy::execute() {
    if (!validate()) {
        setAction("Deploy order invalid.");
        return;
    }

    // In Part 4 add armies to target_ here.
    setAction("Deploy " + std::to_string(armies_) +
          " armies to " + target_->name);

    executed = true;
}

// BOMB CLASS
Bomb::Bomb() : Order("Bomb"), owner_(nullptr), target_(nullptr) {}

Bomb::Bomb(Player* owner, Territory* target) 
    : Order("Bomb"), owner_(owner), target_(target) {}

bool Bomb::validate() { 
    if (!owner_ || !target_) return false;
    
    // Check target is NOT owned by player
    const auto* terrs = owner_->territories();
    if (terrs) {
        for (Territory* t : *terrs) {
            if (t == target_) {
                setAction("Invalid: Cannot bomb own territory");
                return false;
            }
        }
    }
    
    // Check player has bomb card (implement when Hand::hasCard exists)
    
    return true; 
}

void Bomb::execute() {
    if (!validate()) {
        setAction("Bomb order invalid.");
        setExecuted(false);
        return;
    }

    setAction("Bombed " + target_->name);
    setExecuted(true);
}

// ADVANCE CLASS
Advance::Advance() : Order("Advance"), owner_(nullptr), source_(nullptr), target_(nullptr), armies_(0) {}

Advance::Advance(Player* owner, Territory* source, Territory* target, int armies)
    : Order("Advance"), owner_(owner), source_(source), target_(target), armies_(armies) {}

bool Advance::validate() {
    if (!owner_ || !source_ || !target_) return false;
    
    // Check player owns source territory
    const auto* terrs = owner_->territories();
    if (!terrs) return false;
    
    bool ownsSource = false;
    for (Territory* t : *terrs) {
        if (t == source_) {
            ownsSource = true;
            break;
        }
    }
    
    if (!ownsSource) {
        setAction("Invalid: Player does not own source territory");
        return false;
    }
    
    // Check source has enough armies (implement in Part 4)
    
    return true;
}

void Advance::execute() {
    if (!validate()) {
        setAction("Advance order invalid.");
        setExecuted(false);
        return;
    }

    setAction("Advanced " + std::to_string(armies_) + " armies from " + 
              source_->name + " to " + target_->name);
    setExecuted(true);
}

// BLOCKADE CLASS
Blockade::Blockade() : Order("Blockade"), owner_(nullptr), target_(nullptr) {}

Blockade::Blockade(Player* owner, Territory* target)
    : Order("Blockade"), owner_(owner), target_(target) {}

bool Blockade::validate() {
    if (!owner_ || !target_) return false;
    
    // Check player owns the territory
    const auto* terrs = owner_->territories();
    if (!terrs) return false;
    
    bool ownsTerritory = false;
    for (Territory* t : *terrs) {
        if (t == target_) {
            ownsTerritory = true;
            break;
        }
    }
    
    if (!ownsTerritory) {
        setAction("Invalid: Player does not own target territory");
        return false;
    }
    
    // Check player has blockade card (implement when Hand::hasCard exists)
    
    return true;
}

void Blockade::execute() {
    if (!validate()) {
        setAction("Blockade order invalid.");
        setExecuted(false);
        return;
    }

    setAction("Blockaded " + target_->name);
    setExecuted(true);
}

// AIRLIFT CLASS
Airlift::Airlift() : Order("Airlift"), owner_(nullptr), source_(nullptr), target_(nullptr), armies_(0) {}

Airlift::Airlift(Player* owner, Territory* source, Territory* target, int armies)
    : Order("Airlift"), owner_(owner), source_(source), target_(target), armies_(armies) {}

bool Airlift::validate() {
    if (!owner_ || !source_ || !target_) return false;
    
    // Check player owns both territories
    const auto* terrs = owner_->territories();
    if (!terrs) return false;
    
    bool ownsSource = false;
    bool ownsTarget = false;
    
    for (Territory* t : *terrs) {
        if (t == source_) ownsSource = true;
        if (t == target_) ownsTarget = true;
    }
    
    if (!ownsSource || !ownsTarget) {
        setAction("Invalid: Player must own both territories");
        return false;
    }
    
    // Check player has airlift card (implement when Hand::hasCard exists)
    
    return true;
}

void Airlift::execute() {
    if (!validate()) {
        setAction("Airlift order invalid.");
        setExecuted(false);
        return;
    }

    setAction("Airlifted " + std::to_string(armies_) + " armies from " +
              source_->name + " to " + target_->name);
    setExecuted(true);
}

// NEGOTIATE CLASS
Negotiate::Negotiate() : Order("Negotiate"), owner_(nullptr), targetPlayer_(nullptr) {}

Negotiate::Negotiate(Player* owner, Player* targetPlayer)
    : Order("Negotiate"), owner_(owner), targetPlayer_(targetPlayer) {}

bool Negotiate::validate() {
    if (!owner_ || !targetPlayer_) return false;
    
    // Can't negotiate with yourself
    if (owner_ == targetPlayer_) {
        setAction("Invalid: Cannot negotiate with yourself");
        return false;
    }
    
    // Check player has negotiate card (implement when Hand::hasCard exists)
    
    return true;
}

void Negotiate::execute() {
    if (!validate()) {
        setAction("Negotiate order invalid.");
        setExecuted(false);
        return;
    }

    setAction("Negotiated with " + targetPlayer_->name());
    setExecuted(true);
}

/* OrdersList  */

/* Adding an Order in the OrdersList */
void OrdersList::add(Order* o) { 
    orders.push_back(o);
}

/* Removing an Order in the OrdersList */
void OrdersList::remove(int i) {
    if(i >= 0 && i < orders.size()){ 
    orders.erase(orders.begin() + i);
    }
}

/* Moving an Order in the OrdersList 
int currentPosition: chosen order
int newPosition: position to move the chosen order to
*/
void OrdersList::move(int currentPosition, int newPosition) {
    if(currentPosition >= 0 && currentPosition < orders.size() && newPosition >= 0 && newPosition < orders.size() && currentPosition != newPosition) {
        Order* tempOrder = orders[currentPosition];
        orders.erase(orders.begin() + currentPosition);
        orders.insert(orders.begin() + newPosition, tempOrder);
    }
}
/* Displaying all orders in the OrderList */
void OrdersList::display() {
    for(int i = 0; i < orders.size(); i++) 
    std::cout << i << ": " << *orders[i] << '\n';
}

/* Executing all orders in the OrderList */
void OrdersList::executeAllOrders() {
    for (int i = 0; i < orders.size(); i++) {
        orders[i]->execute();
    }
}

int OrdersList::size() const {
    return static_cast<int>(orders.size());
}

Order* OrdersList::get(int i) const {
    if (i < 0 || i >= static_cast<int>(orders.size())) {
        return nullptr;
    }
    return orders[i];
}
