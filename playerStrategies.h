#pragma once

#include <vector>
#include <string>
#include <iosfwd>

class Player;
class Territory;
class Deck;
class Order;

// Base abstract strategy
class PlayerStrategy {
public:
    PlayerStrategy(); //default constructor
    PlayerStrategy(const PlayerStrategy& other); //copy const
    PlayerStrategy& operator=(const PlayerStrategy& other); //assignemnt op
    
    //all derived strategies should have virtual destructor 
    //so deleting playerStrategy calls proper derived class
    virtual ~PlayerStrategy();

    // clone for deep copy 
    //concrete strat returns heap-allocated copy of itself
    virtual PlayerStrategy* clone() const = 0;


    //list territories to defend/attack
    virtual std::vector<Territory*> toDefend(Player* player) const = 0;
    virtual std::vector<Territory*> toAttack(Player* player) const = 0;

    // Return the created order (or nullptr)same spirit as Player::issueOrder
    //kind describes the type (deploy, advance etc)
    virtual Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) = 0;

    const std::string& name() const;

protected:
    std::string* name_;   // getter for strat name
};

// Human Player Strategy
//always interacts with user 
class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy();
    HumanPlayerStrategy(const HumanPlayerStrategy& other);
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other);
    ~HumanPlayerStrategy() override;

    PlayerStrategy* clone() const override;

    
    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;
};

// Aggressive Player Strategy
//strongest territory focus, attacks aggressively 
class AggressivePlayerStrategy : public PlayerStrategy {
public:

    AggressivePlayerStrategy();
    AggressivePlayerStrategy(const AggressivePlayerStrategy& other);
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other);
    ~AggressivePlayerStrategy() override;

    PlayerStrategy* clone() const override;

    //attack = neighbours of strongest teritory
    //defend strongest territories
    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    
    //deploy on strongest, advance from stongest to weakest
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;
};

// Benevolent Player Strategy
//never attacks, only reinforces weakest territories
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy();
    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other);
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other);
    ~BenevolentPlayerStrategy() override;

    PlayerStrategy* clone() const override;

    //defend weakest terr
    std::vector<Territory*> toDefend(Player* player) const override;
    
    //return empty list
    std::vector<Territory*> toAttack(Player* player) const override;
    
    //deploy on weakest, move troops between your terr
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;
};

// Neutral Player Strategy
//does nothing
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy();
    NeutralPlayerStrategy(const NeutralPlayerStrategy& other);
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other);
    ~NeutralPlayerStrategy() override;

    PlayerStrategy* clone() const override;

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;
};

// Cheater Player Strategy
//automatically conquers adjacent enemy terrs
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy();
    CheaterPlayerStrategy(const CheaterPlayerStrategy& other);
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other);
    ~CheaterPlayerStrategy() override;

    PlayerStrategy* clone() const override;

    //defend whatever it has
    std::vector<Territory*> toDefend(Player* player) const override;
    
    //attack all adjacent 
    std::vector<Territory*> toAttack(Player* player) const override;
    
    //issue order that immediately conquers adjacent terrs
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;

private:
//do the cheating logic
    void performCheat(Player* player);
};
