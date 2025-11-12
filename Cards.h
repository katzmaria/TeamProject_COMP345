#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include <string>

class Order; 

class Card {
public:
    Card(const std::string& type);
    Card(const Card& other);                 
    Card& operator=(const Card& other);      
    ~Card();

    std::string getType() const;
    Order* play();                           

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
    Card* draw(Hand* hand);  // draw random card and put it in the hand of the player

    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);

private:
    std::vector<Card*>* cards;  // collection of cards in the deck
};

// free function for main 
void testCards();  

#endif
