#ifndef ORDERS_H
#define ORDERS_H

#include <iostream>
#include <vector>
#include <string>


class Player;
class Territory;


// Order Base Class
class Order {
protected:
    std::string orderName;
    bool executed;
    std::string action;

public:
    Order(const std::string& n);
    virtual ~Order() = default;

    std::string getOrderName();
    std::string getAction();
    bool isExecuted();

    void setOrderName(std::string& n);
    void setAction(const std::string& e);
    void setExecuted(bool e);

    virtual bool validate();
    virtual void execute();

    friend std::ostream& operator<<(std::ostream& out, const Order& order);
};

// Deploy Order
class Deploy : public Order {
private:
    Player* owner_;
    Territory* target_;
    int armies_;

public:
    Deploy();
    Deploy(Player* owner, Territory* target, int armies);
    bool validate() override;
    void execute() override;
};

// Advance Order
class Advance : public Order {
private:
    Player* owner_;
    Territory* source_;
    Territory* target_;
    int armies_;

public:
    Advance();
    Advance(Player* owner, Territory* source, Territory* target, int armies);
    bool validate() override;
    void execute() override;
};

// Bomb Order
class Bomb : public Order {
private:
    Player* owner_;
    Territory* target_;

public:
    Bomb();
    Bomb(Player* owner, Territory* target);
    bool validate() override;
    void execute() override;
};

// Blockade Order
class Blockade : public Order {
private:
    Player* owner_;
    Territory* target_;
    static Player* neutralPlayer_; // Static Neutral player

public:
    Blockade();
    Blockade(Player* owner, Territory* target);
    bool validate() override;
    void execute() override;
    static Player* getNeutralPlayer(); // Get or create Neutral player
};

// Airlift Order
class Airlift : public Order {
private:
    Player* owner_;
    Territory* source_;
    Territory* target_;
    int armies_;

public:
    Airlift();
    Airlift(Player* owner, Territory* source, Territory* target, int armies);
    bool validate() override;
    void execute() override;
};

// Negotiate Order
class Negotiate : public Order {
private:
    Player* owner_;
    Player* targetPlayer_;

public:
    Negotiate();
    Negotiate(Player* owner, Player* targetPlayer);
    bool validate() override;
    void execute() override;
};

// OrdersList Class
class OrdersList {
private:
    std::vector<Order*> orders;

public:
    void add(Order* o);
    void remove(int i);
    void move(int currentPosition, int newPosition);
    void display();
    void executeAllOrders();
    int size() const;
    Order* get(int i) const;
};

#endif

