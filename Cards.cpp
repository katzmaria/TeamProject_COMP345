#include "Cards.h"
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <random>
#include <ctime>

// -------------------- Card --------------------
Card::Card(const std::string& type) {
    this->type = new std::string(type);
}

Card::Card(const Card& other) {
    type = new std::string(*other.type);
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        delete type;
        type = new std::string(*other.type);
    }
    return *this;
}

Card::~Card() {
    delete type;
}

std::string Card::getType() const {
    return *type;
}

Order* Card::play() {
    std::cout << "Playing card: " << *type << std::endl;
    
    // TODO: integrate with Orders system in Part 3

    
    return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Card& card) {
    out << "Card(" << *card.type << ")";
    return out;
}

// -------------------- Hand --------------------
Hand::Hand() {
    cards = new std::vector<Card*>();
}

Hand::Hand(const Hand& other) {
    cards = new std::vector<Card*>();
    for (auto c : *other.cards) {
        cards->push_back(new Card(*c));
    }
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        for (auto c : *cards) delete c;
        delete cards;
        cards = new std::vector<Card*>();
        for (auto c : *other.cards) {
            cards->push_back(new Card(*c));
        }
    }
    return *this;
}

Hand::~Hand() {
    for (auto c : *cards) delete c;
    delete cards;
}

void Hand::addCard(Card* card) {
    cards->push_back(card);
}

void Hand::removeCard(Card* card) {
    auto it = std::find(cards->begin(), cards->end(), card);
    if (it != cards->end()) {
        delete *it;
        cards->erase(it);
    }
}

std::vector<Card*> Hand::getCards() const {
    return *cards;
}

std::ostream& operator<<(std::ostream& out, const Hand& hand) {
    out << "Hand: ";
    for (auto c : *hand.cards) out << *c << " ";
    return out;
}

// -------------------- Deck --------------------
Deck::Deck() {
    cards = new std::vector<Card*>();
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }

}

Deck::Deck(const Deck& other) {
    cards = new std::vector<Card*>();
    for (auto c : *other.cards) {
        cards->push_back(new Card(*c));
    }
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        for (auto c : *cards) delete c;
        delete cards;
        cards = new std::vector<Card*>();
        for (auto c : *other.cards) {
            cards->push_back(new Card(*c));
        }
    }
    return *this;
}

Deck::~Deck() {
    for (auto c : *cards) delete c;
    delete cards;
}

void Deck::addCard(Card* card) {
    cards->push_back(card);
}

Card* Deck::draw(Hand* hand) {
    if (cards->empty()) return nullptr;

    // Simple random using rand()
    int index = rand() % cards->size();
    Card* drawn = cards->at(index);

    // give ownership to hand
    hand->addCard(drawn);

    // remove from deck
    cards->erase(cards->begin() + index);

    std::cout << "Drew card: " << *drawn << std::endl;
    return drawn;
}

std::ostream& operator<<(std::ostream& out, const Deck& deck) {
    out << "Deck: ";
    for (auto c : *deck.cards) out << *c << " ";
    return out;
}
