
#include <vector>
#include <string>
#include <iosfwd>

// Forward declarations (implemented elsewhere)
class Territory;
class Hand;
class OrdersList;
class Order;

class Player {
private:
    std::string*                name_;
    std::vector<Territory*>*    territories_; // vector of territories (not owned by Player)
    Hand*                       hand_;        // owned by Player
    OrdersList*                 orders_;      // owned by Player

public:
    // Constructors / Rule of Three
    Player();                                     // default constructor
    explicit Player(const std::string& name);     // parameterized constructor
    Player(const Player& other);                  // copy constructor
    Player& operator=(const Player& other);       // copy assignment
    ~Player();                                    // destructor

    // Name
    const std::string& name() const;
    void setName(const std::string& n);

    // Hand
    Hand* hand() const;
    void setHand(Hand* h);                        // takes ownership

    // Orders
    OrdersList* orders() const;
    void setOrders(OrdersList* ol);               // takes ownership

    // Territories owned
    const std::vector<Territory*>* territories() const;
    void addTerritory(Territory* t);
    void removeTerritory(Territory* t);

    // Planning helpers (stub logic for now)
    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;

    // Create and enqueue an order by kind ("deploy","advance","bomb","blockade","airlift","negotiate")
    // (Parameters omitted here to match current Orders.cpp/simple factories;
    //  you can overload later if you add typed parameters.)
    Order* issueOrder(const std::string& kind);

    // Printing
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
