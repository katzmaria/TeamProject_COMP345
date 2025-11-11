#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <vector>
#include <string>


class Player;
class Territory;


class Order {
private:
    std::string orderName;
    std::string action;
    

public:
    bool executed;
    Order( const std::string& n);

    // Getters Functions
    std::string getOrderName();
    bool isExecuted();

    // Setters Functions
    void setOrderName(std::string& n);
    void setAction(const std::string& e);  
    void setExecuted(bool e);
    std::string getAction(); 
    virtual bool validate();
    virtual void execute();
    friend std::ostream& operator<<(std::ostream& out,  const Order& order);
};

class Deploy : public Order { 
    public: 
      Deploy();
    Deploy(Player* owner, Territory* target, int armies);

    bool validate() override;
    void execute() override; 

    private:
    Player* owner_;
    Territory* target_;
    int armies_;

};
class Advance : public Order { 
    public: 
    Advance(); 
    bool validate(); 
};
class Bomb : public Order { 
    public: 
    Bomb(); 
    bool validate(); 
};
class Blockade : public Order { 
    public: 
    Blockade(); 
    bool validate(); 
};
class Airlift : public Order { 
    public: 
    Airlift(); 
    bool validate(); 
};
class Negotiate : public Order { 
    public: 
    Negotiate(); 
    bool validate(); 
};

class OrdersList {
    std::vector<Order*> orders;

public:
    void add(Order* o);
    void remove(int i);
    void move(int from, int to);
    void display();
    void executeAllOrders();
    int size() const;
    Order* get(int index) const;

};

#endif

