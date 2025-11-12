#include "Orders.h"
#include "Map.h"
#include "Player.h"  // Adding this line.
#include "Cards.h"   // Adding  this line
#include <cstdlib>    
#include <ctime>     
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

/* Checking whether the order is valid or not. */
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
    if (!owner_) {
        
        return false;
    
    
    }
    if (!target_) return false;
    
    
    const auto* terrs = owner_->territories();
    if (!terrs) return false;
    
    bool ownsTerritory = false;
    for (Territory* t : *terrs) {
        if (t == target_) {
            ownsTerritory = true;
            break;
        }
    }
    
    if (!ownsTerritory) return false;
    
    
    if (owner_->getReinforcementPool() < armies_) {
        return false;
    }
    
    return true;
}

void Deploy::execute() {
    if (!validate()) {
        setAction("Deploy order invalid: target territory not owned by player or insufficient reinforcements.");
        setExecuted(false);
        return;
    }

    // Spend reinforcements during execution
    owner_->setReinforcementPool(owner_->getReinforcementPool() - armies_);
    
    // Add armies to territory
    target_->armies += armies_;
    
    setAction("Deployed " + std::to_string(armies_) + " armies to " + target_->name + 
              ". Territory now has " + std::to_string(target_->armies) + " armies.");
    setExecuted(true);
}

// BOMB CLASS
Bomb::Bomb() : Order("Bomb"), owner_(nullptr), target_(nullptr) {}

Bomb::Bomb(Player* owner, Territory* target) 
    : Order("Bomb"), owner_(owner), target_(target) {}

bool Bomb::validate() { 
    if (!owner_ || !target_) {
        setAction("Invalid: missing owner or target");
        return false;
    }
    
    
    if (target_->owner == owner_) {
        setAction("Invalid: Cannot bomb own territory");
        return false;
    }
    
    
    const auto* ownerTerrs = owner_->territories();
    if (!ownerTerrs || ownerTerrs->empty()) {
        setAction("Invalid: Player has no territories");
        return false;
    }
    
    bool isAdjacent = false;
    for (Territory* ownedTerritory : *ownerTerrs) {
       
        for (Territory* neighbor : ownedTerritory->neighbors) {
            if (neighbor == target_) {
                isAdjacent = true;
                break;
            }
        }
        if (isAdjacent) break;
    }
    
    if (!isAdjacent) {
        setAction("Invalid: Target territory is not adjacent to any territory owned by player");
        return false;
    }
    
    return true; 
}

void Bomb::execute() {
    if (!validate()) {
        setAction("Bomb order invalid.");
        setExecuted(false);
        return;
    }

    
    int originalArmies = target_->armies;
    int armiesRemoved = originalArmies / 2;
    target_->armies -= armiesRemoved;
    
    setAction("Bombed " + target_->name + "! Removed " + std::to_string(armiesRemoved) + 
              " armies (was " + std::to_string(originalArmies) + ", now " + 
              std::to_string(target_->armies) + ")");
    setExecuted(true);
}

// ADVANCE CLASS Location
Advance::Advance() : Order("Advance"), owner_(nullptr), source_(nullptr), target_(nullptr), armies_(0) {}

Advance::Advance(Player* owner, Territory* source, Territory* target, int armies)
    : Order("Advance"), owner_(owner), source_(source), target_(target), armies_(armies) {}

bool Advance::validate() {
    if (!owner_ || !source_ || !target_) {
        setAction("Invalid: missing owner, source, or target");
        return false;
    }
    
    
    const auto* terrs = owner_->territories();
    if (!terrs) {
        setAction("Invalid: player has no territories");
        return false;
    }
    
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
    
    // Check source 
    if (source_->armies < armies_) {
        setAction("Invalid: Source territory does not have enough armies");
        return false;
    }
    
    // Check target 
    bool isAdjacent = false;
    for (Territory* neighbor : source_->neighbors) {
        if (neighbor == target_) {
            isAdjacent = true;
            break;
        }
    }
    
    if (!isAdjacent) {
        setAction("Invalid: Target territory is not adjacent to source territory");
        return false;
    }
    
    return true;
}

void Advance::execute() {
    if (!validate()) {
        setAction("Advance order invalid.");
        setExecuted(false);
        return;
    }

    
    bool targetOwnedByPlayer = false;
    const auto* terrs = owner_->territories();
    if (terrs) {
        for (Territory* t : *terrs) {
            if (t == target_) {
                targetOwnedByPlayer = true;
                break;
            }
        }
    }

    if (targetOwnedByPlayer) {
        // transfering armies from source to target
        source_->armies -= armies_;
        target_->armies += armies_;
        setAction("MOVEMENT: Moved " + std::to_string(armies_) + " armies from " + 
                  source_->name + " to " + target_->name + 
                  ". Target now has " + std::to_string(target_->armies) + " armies.");
        setExecuted(true);
    } else {

        Player* defender = target_->owner;
        if (defender && owner_->hasDiplomaticRelation(defender)) {
            setAction("Invalid: Cannot attack " + target_->name + 
                      " due to diplomatic relations with " + defender->name());
            setExecuted(false);
            return;
        }
        
        
        std::string defenderName = defender ? defender->name() : "Neutral";
        int attackingArmies = armies_;
        int defendingArmies = target_->armies;
        
        std::cout << "\n*** BATTLE at " << target_->name << " ***\n";
        std::cout << owner_->name() << " attacks with " << attackingArmies << " armies\n";
        std::cout << defenderName << " defends with " << defendingArmies << " armies\n";
        
        // Removing the armies 
        source_->armies -= armies_;
        
       
        int attackerKills = 0;
        int defenderKills = 0;
        
      
        for (int i = 0; i < attackingArmies; ++i) {
            if ((rand() % 100) < 60) {  // 60% chance
                attackerKills++;
            }
        }
        
        
        for (int i = 0; i < defendingArmies; ++i) {
            if ((rand() % 100) < 70) {  // 70% chance
                defenderKills++;
            }
        }
        
        
        attackingArmies -= defenderKills;
        defendingArmies -= attackerKills;
        
        
        if (attackingArmies < 0) {
            attackingArmies = 0;
        }
        if (defendingArmies < 0){
            defendingArmies = 0;
        }
        
        std::cout << "Battle results: Attacker killed " << attackerKills << " defenders, "
                  << "Defender killed " << defenderKills << " attackers\n";
        
        if (defendingArmies == 0 && attackingArmies > 0) {
            
            target_->armies = attackingArmies;
            
            std::cout << "*** TERRITORY CONQUERED! ***\n";
            std::cout << owner_->name() << " conquers " << target_->name 
                      << " with " << attackingArmies << " surviving armies!\n";
            
          
            if (defender) {
                defender->removeTerritory(target_);
            }
            target_->setOwner(owner_);
            owner_->addTerritory(target_);
            
            
            owner_->setConqueredThisTurn(true);
            
            setAction("CONQUEST! Conquered " + target_->name + " with " + 
                      std::to_string(attackingArmies) + " surviving armies. " +
                      "Killed " + std::to_string(attackerKills) + " defenders, " +
                      "Lost " + std::to_string(defenderKills) + " attackers.");
            setExecuted(true);
        } else {
            
            target_->armies = defendingArmies;
            std::cout << "Attack FAILED! " << defendingArmies << " defenders remain.\n";
            
            setAction("ATTACK FAILED on " + target_->name + ". " +
                      std::to_string(defendingArmies) + " defenders remain. " +
                      "Killed " + std::to_string(attackerKills) + " defenders, " +
                      "Lost " + std::to_string(defenderKills) + " attackers.");
            setExecuted(true);
        }
    }
}


// BLOCKADE CLASS Location
Player* Blockade::neutralPlayer_ = nullptr;

Blockade::Blockade() : Order("Blockade"), owner_(nullptr), target_(nullptr) {}

Blockade::Blockade(Player* owner, Territory* target)
    : Order("Blockade"), owner_(owner), target_(target) {}

Player* Blockade::getNeutralPlayer() {
    if (!neutralPlayer_) {
        neutralPlayer_ = new Player("Neutral");
    }
    return neutralPlayer_;
}

bool Blockade::validate() {
    if (!owner_ || !target_) {
        setAction("Invalid: missing owner or target");
        return false;
    }
    
    
    if (target_->owner != owner_) {
        setAction("Invalid: Player does not own target territory");
        return false;
    }
    
    return true;
}

void Blockade::execute() {
    if (!validate()) {
        setAction("Blockade order invalid.");
        setExecuted(false);
        return;
    }

    int originalArmies = target_->armies;
    
    // Double the army units
    target_->armies *= 2;
    
    
    owner_->removeTerritory(target_);
    
    
    Player* neutral = getNeutralPlayer();
    target_->owner = neutral;
    neutral->addTerritory(target_);
    
    setAction("Blockaded " + target_->name + "! Doubled armies from " + 
              std::to_string(originalArmies) + " to " + std::to_string(target_->armies) + 
              ". Territory now owned by Neutral player.");
    setExecuted(true);
}

// AIRLIFT CLASS
Airlift::Airlift() : Order("Airlift"), owner_(nullptr), source_(nullptr), target_(nullptr), armies_(0) {}

Airlift::Airlift(Player* owner, Territory* source, Territory* target, int armies)
    : Order("Airlift"), owner_(owner), source_(source), target_(target), armies_(armies) {}

bool Airlift::validate() {
    if (!owner_ || !source_ || !target_) {
        setAction("Invalid: missing owner, source, or target");
        return false;
    }
    
    
    if (source_->owner != owner_) {
        setAction("Invalid: Player does not own source territory");
        return false;
    }
    
    if (target_->owner != owner_) {
        setAction("Invalid: Player does not own target territory");
        return false;
    }
    
    
    if (source_->armies < armies_) {
        setAction("Invalid: Source territory does not have enough armies");
        return false;
    }
    
    return true;
}

void Airlift::execute() {
    if (!validate()) {
        setAction("Airlift order invalid.");
        setExecuted(false);
        return;
    }

   
    source_->armies -= armies_;
    target_->armies += armies_;
    
    setAction("Airlifted " + std::to_string(armies_) + " armies from " +
              source_->name + " to " + target_->name + ". " +
              "Source now has " + std::to_string(source_->armies) + " armies, " +
              "Target now has " + std::to_string(target_->armies) + " armies.");
    setExecuted(true);
}

// NEGOTIATE CLASS
Negotiate::Negotiate() : Order("Negotiate"), owner_(nullptr), targetPlayer_(nullptr) {}

Negotiate::Negotiate(Player* owner, Player* targetPlayer)
    : Order("Negotiate"), owner_(owner), targetPlayer_(targetPlayer) {}

bool Negotiate::validate() {
    if (!owner_ || !targetPlayer_) return false;
    
    
    if (owner_ == targetPlayer_) {
        setAction("Invalid: Cannot negotiate with yourself");
        return false;
    }
    
   
    
    return true;
}

void Negotiate::execute() {
    if (!validate()) {
        setAction("Negotiate order invalid.");
        setExecuted(false);
        return;
    }

    
    owner_->addDiplomaticRelation(targetPlayer_);
    targetPlayer_->addDiplomaticRelation(owner_);
    
    setAction("Negotiated with " + targetPlayer_->name() + 
              ". No attacks can be made between " + owner_->name() + 
              " and " + targetPlayer_->name() + " for the remainder of this turn.");
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
