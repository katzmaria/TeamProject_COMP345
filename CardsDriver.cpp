#include "Cards.h"
#include <iostream>
#include <ctime>   


//testing cards function
void testCards() {
    std::cout << "=== Testing Cards ===" << std::endl;

    // Create deck with all types
    Deck deck;
    deck.addCard(new Card("bomb"));
    deck.addCard(new Card("reinforcement"));
    deck.addCard(new Card("blockade"));
    deck.addCard(new Card("airlift"));
    deck.addCard(new Card("diplomacy"));

    std::cout << deck << std::endl;

    // Creating the hand
    Hand hand;

   
    for (int i = 0; i < 3; i++) {
        deck.draw(&hand);
    }

    std::cout << hand << std::endl;

    // Play every card in the hand .
    for (auto card : hand.getCards()) {
        card->play();
    }

    std::cout << "=== End Test ===  " << std::endl;
}

void cardsDriver() {
    
    testCards();
}
