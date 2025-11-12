
#include <vector>
#include <string>
#include <iosfwd>
#include <set>
class Territory;
class Hand;
class OrdersList;
class Order;
class Deck;
class Card;

class Player {
private:
    std::string*                name_;
    std::vector<Territory*>*    territories_; // vector of territories (not owned by Player)
    Hand*                       hand_;        // owned by Player
    OrdersList*                 orders_;      // owned by Player
    int*                        reinforcementPool_;
    int*                        committedReinforcements_; // track armies committed during issue phase
    bool*                       conqueredThisTurn_; // track if player conquered a territory this turn
    std::set<Player*>*          diplomaticRelations_; // players you cannot attack this turn

public:

    Player();// default constructor
    explicit Player(const std::string& name);// parameterized constructor
    Player(const Player& other);// copy constructor
    Player& operator=(const Player& other);// copy assignment
    ~Player();// destructor

    // Name
    const std::string& name() const;
    void setName(const std::string& n);

    // Hand
    Hand* hand() const;
    void setHand(Hand* h);// takes ownership

    // Orders
    OrdersList* orders() const;
    void setOrders(OrdersList* ol);// takes ownership

    // Territories owned
    const std::vector<Territory*>* territories() const;
    void addTerritory(Territory* t);
    void removeTerritory(Territory* t);


    Order* issueOrder(const std::string& kind, class Deck* deck = nullptr, const std::vector<Player*>* allPlayers = nullptr);

    // Printing
    friend std::ostream& operator<<(std::ostream& os, const Player& p);

    int  getReinforcementPool() const;
    void setReinforcementPool(int value);
    void addReinforcements(int delta);   
    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;
    
    int  getAvailableReinforcements() const; // pool minus committed
    void commitReinforcements(int amount);
    void resetCommitted(); // call at start of execute phase
    
    bool hasConqueredThisTurn() const;
    void setConqueredThisTurn(bool value);
    
    // Diplomatic relations (for negotiate orders)
    void addDiplomaticRelation(Player* player);
    bool hasDiplomaticRelation(Player* player) const;
    void clearDiplomaticRelations(); // call at start of turn

};
