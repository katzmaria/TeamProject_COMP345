#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <vector>
#include <string>

class Order {
private:
    std::string orderName;
    std::string action;
    bool executed;

public:
    Order( const std::string& n);

    // Getters Functions
    std::string getOrderName()  ;
    std::string getAction() ;
    bool isExecuted() ;

    // Setters Functions
    void setOrderName(  std::string& n);
    void setAction( std::string& e);
    void setExecuted( bool e);

    virtual bool validate();
    virtual void execute();
    friend std::ostream& operator<<(std::ostream& out,  const Order& order);
};

class Deploy : public Order { 
    public: 
    Deploy(); 
    bool validate(); 

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
};

#endif

