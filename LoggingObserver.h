#pragma once

#include <string>
#include <list>

class ILoggable {
    public:
        virtual std::string stringToLog() = 0;
};

class Observer {
    public: 
        virtual void update(ILoggable& loggable);
};

class logObserver : public Observer {
    public: 
        void update(ILoggable& loggable);
};

// I think this program only uses 1 observer, so there is not need to save multiple in a vector
class Subject {
    public: 
        void notify(ILoggable& loggable);
        void attach(Observer& obs);
        void detach(Observer& obs);
    private:
        std::list<Observer*> observers;
};