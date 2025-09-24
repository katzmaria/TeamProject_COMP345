#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include <string>

class Order; // forward declare (from Part 3)

class Card {
public:
    Card(const std::string& type);
    Card(const Card& other);                 // copy constructor
    Card& operator=(const Card& other);      // assignment operator
    ~Card();

    std::string getType() const;
    Order* play();                           // simulate play: create order + return card to deck

    friend std::ostream& operator<<(std::ostream& out, const Card& card);

private:
    std::string* type;
};

class Hand {
public:
    Hand();
    Hand(const Hand& other);
    Hand& operator=(const Hand& other);
    ~Hand();

    void addCard(Card* card);
    void removeCard(Card* card);
    std::vector<Card*> getCards() const;

    friend std::ostream& operator<<(std::ostream& out, const Hand& hand);

private:
    std::vector<Card*>* cards;  // collection of cards in hand
};

class Deck {
public:
    Deck();
    Deck(const Deck& other);
    Deck& operator=(const Deck& other);
    ~Deck();

    void addCard(Card* card);
    Card* draw(Hand* hand);  // draw random card → put in hand

    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);

private:
    std::vector<Card*>* cards;  // collection of cards in deck
};

void testCards();  // driver free function

#endif
