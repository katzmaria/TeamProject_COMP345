
#pragma once
#include <vector>
#include <string>
#include <iosfwd>


// Assuming already implementeed 
class Territory;
class Hand; 
class OrdersList; 
class Order; 


class Player {
private:
std::string* name_;
std::vector<Territory*>* territories_; // vector of territories 
Hand* hand_;
OrdersList* orders_;


public:
// constructors and destructor
Player();// --> default constructor
explicit Player(const std::string& name);//--> paramatized constructor
Player(const Player& other); // copy constructor 
Player& operator=(const Player& other); // assignment handling --> new memory same value
~Player(); // destructor 


// get methods name
const std::string& name() const;
// set method name
void setName(const std::string& n);

// getter hand 
Hand* hand() const; 
// setter hand 
void setHand(Hand* h);

// getter orders 
OrdersList* orders() const; 
//setter orders 
void setOrders(OrdersList* ol);

// territory lists --> set teritorry to attack or defend from vector list 
std::vector<Territory*> toDefend() const; 
std::vector<Territory*> toAttack() const; 


// Territories player owns  --> add or remove territory to player vector
void addTerritory(Territory* t);
void removeTerritory(Territory* t);
// territory list getter 
const std::vector<Territory*>* territories() const;


// Create an order and push to this player's OrdersList.
// assuming this is the implementation 
Order* issueOrder(const std::string& kind,
Territory* source = nullptr,
Territory* target = nullptr,
int armies = 0);


// friend function --> acts like player toString()
friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
