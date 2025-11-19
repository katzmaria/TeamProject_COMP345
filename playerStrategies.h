#pragma once

#include <vector>
#include <string>
#include <iosfwd>

class Player;
class Territory;
class Deck;
class Order;

// ========================
// Base abstract strategy
// ========================
class PlayerStrategy {
public:
    PlayerStrategy();
    PlayerStrategy(const PlayerStrategy& other);
    PlayerStrategy& operator=(const PlayerStrategy& other);
    virtual ~PlayerStrategy();

    // Polymorphic clone for deep copy
    virtual PlayerStrategy* clone() const = 0;

    // Strategy API
    virtual std::vector<Territory*> toDefend(Player* player) const = 0;
    virtual std::vector<Territory*> toAttack(Player* player) const = 0;

    // Return the created order (or nullptr) – same spirit as your Player::issueOrder
    virtual Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) = 0;

    const std::string& name() const;

protected:
    std::string* name_;   // pointer (consistent with COMP345 pointer style)
};

// ========================
// Human Player Strategy
// ========================
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

// ========================
// Aggressive Player Strategy
// ========================
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy();
    AggressivePlayerStrategy(const AggressivePlayerStrategy& other);
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other);
    ~AggressivePlayerStrategy() override;

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

// ========================
// Benevolent Player Strategy
// ========================
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy();
    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other);
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other);
    ~BenevolentPlayerStrategy() override;

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

// ========================
// Neutral Player Strategy
// ========================
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

// ========================
// Cheater Player Strategy
// ========================
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy();
    CheaterPlayerStrategy(const CheaterPlayerStrategy& other);
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other);
    ~CheaterPlayerStrategy() override;

    PlayerStrategy* clone() const override;

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    Order* issueOrder(
        Player* player,
        const std::string& kind,
        Deck* deck,
        const std::vector<Player*>* allPlayers
    ) override;

private:
    void performCheat(Player* player);
};
