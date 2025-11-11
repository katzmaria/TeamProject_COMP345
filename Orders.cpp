#include "Orders.h"
#include "Map.h"
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
    return owner_ != nullptr && target_ != nullptr && armies_ > 0;
}

Advance::Advance() : Order("Advance") {

}
bool Advance::validate() { 
    return true; 
}

Bomb::Bomb() : Order("Bomb") {

}
bool Bomb::validate() { 
    return true; 
}

Blockade::Blockade() : Order("Blockade") {

}
bool Blockade::validate() { 
    return true; 
}

Airlift::Airlift() : Order("Airlift") {

}
bool Airlift::validate() { 
    return true; 
}

Negotiate::Negotiate() : Order("Negotiate") {


}
bool Negotiate::validate() { 
    return true; 
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

int OrdersList::size() const {
    return static_cast<int>(orders.size());
}

Order* OrdersList::get(int i) const {
    if (i < 0 || i >= static_cast<int>(orders.size())) {
        return nullptr;
    }
    return orders[i];
}
