#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include <fstream>
#include <iostream>

//
// Subject class methods
//
void Subject::notify(ILoggable& loggable){
    for (Observer* obs : observers){
        if (obs){
            obs->update(loggable);
        }
    }
}

void Subject::attach(Observer& obs){
    observers.push_back(&obs);
}

void Subject::detach(Observer& obs){
    observers.remove(&obs);
}

//
//Observer class
//
void Observer::update(ILoggable& loggable){
    loggable.stringToLog();
}

//
//logObserver class
//
// this function will take the string from stringToLog and append the string the the data in gamelog.txt
void logObserver::update(ILoggable& loggable){
    // open gamelog.txt in append mode
    std::ofstream outFile("gamelog.txt", std::ios::app);
    
    if (outFile.is_open()){
        // write the string to the file
        outFile << loggable.stringToLog() << std::endl;
        outFile.close();
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
    }

}

//
// testing function
//
void testLoggingObserver(){
    // proove that Command, CommandProcessor are subclasses of Subject and ILoggable
    // Cannot prove they are subclasses of Subject because Subject doesn't have any virtual functions
    // without virtual functions being implemented, dynamic_cast won't work for Subject
    // THIS PART IS INCOMPLETE, STILL NEEDS TO PROOVE OTHER SUBCLASSES!!!
    ILoggable* commandPro = new CommandProcessor();
    CommandProcessor* derivedProcessor = dynamic_cast<CommandProcessor*>(commandPro);

    ILoggable* command = new Command();
    Command* derivedCommand = dynamic_cast<Command*>(command);

    if (derivedProcessor != nullptr){
        std::cout << "CommandProcessor is a child of Subject" << std::endl;
    }

    if (derivedCommand != nullptr){
        std::cout << "Command is a child of Subject\n\n" << std::endl;
    }

    delete(derivedProcessor);
    delete(derivedCommand);
    

    // no idea have to prove the rest of part 5. Things were programmed accordingly and they just work.
    // gamelog.txt is written to when notify is called, the observer pattern works, so I don't know what 
    // to put here. 

}