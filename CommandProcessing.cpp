#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include "CommandProcessing.h"
#include "LoggingObserver.h"

// 
// CommandProcessor class implementation
//

// default constructor
CommandProcessor::CommandProcessor() {
    // initialize an empty list of commands
    commands = new std::list<Command>();
}

// copy constructor
CommandProcessor::CommandProcessor(const CommandProcessor& processor) {
    commands = new std::list<Command>(*(processor.commands));
}

// assignment operator
CommandProcessor& CommandProcessor::operator=(const CommandProcessor& processor) {
    if (this != &processor){
        delete commands;
        commands = new std::list<Command>(*(processor.commands));
    }
    return *this;
}

// stream insertion operator
std::ostream& operator<<(std::ostream& out, const CommandProcessor& processor){
    out << "CommandProcessor with " << processor.commands->size() << " commands.";
    return out;
}

// destructor
CommandProcessor::~CommandProcessor() {
    delete commands;
}

// readCommand method gets user input and saves it as a Command object
void CommandProcessor::readCommand() {
    std::string input;
    std::cout << "Enter command: ";
    // reads the entire line including spaces
    std::getline(std::cin, input);
    // create a new Command object and add it to the list
    saveCommand(input);
}

// saveCommand method saves the command into the list
void CommandProcessor::saveCommand(const std::string& command) {
    Command cmd = Command(command);
    commands->push_back(cmd);
    // notify the subject
    notify(*this);
}

// this is a getter method, it retrieves the next command from the list and returns its string
std::string CommandProcessor::getCommand(const std::string& gameState) {
    readCommand();
    // if the list is not empty, retrieve the first command
    if (!commands->empty()){
        // the command is removed from the list after being retrieved
        Command* cmd = new Command(commands->front());   

        // create a logObserver and attach it to the current command object.
        // this will need to be removed and deleted just before the Command object is deleted
        logObserver observer;
        cmd->attach(observer);
        

        //IMPORTANT: because the command is removed, when getCommand is called it should also log
        // the command's effect
        commands->pop_front();
        // validate the command before returning
        bool isValid = validate(cmd, gameState);

        // once validate, detach the observer
        cmd->detach(observer);

        std::string commandStr = cmd->getCommand();
        std::string baseCommand = commandStr;
        size_t space = commandStr.find(' ');

        if (space != std::string::npos){
            baseCommand = commandStr.substr(0, space);
        }

        if (isValid){
            // log the effect of the command cmd
            delete cmd;
            return baseCommand;
        }
        // if the command is invalid, return "invalid"
        else {
            // log the effect of the command cmd
            delete cmd;
            return "invalid";
        }
    }
    return "";
}

// validate method checks if the command is valid
bool CommandProcessor::validate(Command* cmd, const std::string& currentState) {
    // Get command string and split into command and argument
    std::string commandStr = cmd->getCommand();
    std::string baseCommand = commandStr;
    std::string argument = "";
    size_t space = commandStr.find(' ');
    
    if (space != std::string::npos) {
        baseCommand = commandStr.substr(0, space);
        argument = commandStr.substr(space + 1);
    }

    bool isValid = false;
    std::string effectMsg;

    // Validate based on current state
    if (currentState == "start") {
        if (baseCommand == "loadmap") {
            isValid = true;
            effectMsg = "Loading map file: " + argument;
        } else {
            effectMsg = "Invalid command in start state. Expected 'loadmap <filename>'";
        }
    }
    else if (currentState == "maploaded") {
        if (baseCommand == "validatemap") {
            isValid = true;
            effectMsg = "Validating map";
        } else if (baseCommand == "loadmap") {
            isValid = true;
            effectMsg = "Loading additional map: " + argument;
        } else {
            effectMsg = "Invalid command in maploaded state. Expected 'validatemap' or 'loadmap <filename>'";
        }
    }
    else if (currentState == "mapvalidated") {
        if (baseCommand == "addplayer") {
            isValid = true;
            effectMsg = "Adding player: " + argument;
        } else {
            effectMsg = "Invalid command in mapvalidated state. Expected 'addplayer <playername>'";
        }
    }
    else if (currentState == "playersadded") {
        if (baseCommand == "addplayer") {
            isValid = true;
            effectMsg = "Adding additional player: " + argument;
        } else if (baseCommand == "gamestart") {
            isValid = true;
            effectMsg = "Starting game";
        } else {
            effectMsg = "Invalid command in playersadded state. Expected 'addplayer <playername>' or 'gamestart'";
        }
    }
    else if (currentState == "win") {
        if (baseCommand == "replay") {
            isValid = true;
            effectMsg = "Restarting game";
        } else if (baseCommand == "quit") {
            isValid = true;
            effectMsg = "Ending game";
        } else {
            effectMsg = "Invalid command in win state. Expected 'replay' or 'quit'";
        }
    }

    // Save the effect message to the Command object
    cmd->saveEffect(effectMsg);
    return isValid;
}

// this function will create a string(command) that will be save in "gamelog.txt"
std::string CommandProcessor::stringToLog(){
    std::string string = "Command: ";
    // for now use the command at the from of the list
    string += commands->back().getCommand();
    return string;
}

//
// Command class implementation
//

// default constructor
Command::Command() {
    // initialize command and effect to empty strings
    command = new std::string("");
    effect = new std::string("");
}

// constructor to initialize command
Command::Command(const std::string& cmd) {
    command = new std::string(cmd);
    effect = new std::string("");
}

// copy constructor
Command::Command(const Command& cmd) {
    command = new std::string(*(cmd.command));
    effect = new std::string(*(cmd.effect));
}

// assignment operator
Command& Command::operator=(const Command& cmd) {
    if (this != &cmd){
        delete command;
        delete effect;
        command = new std::string(*(cmd.command));
        effect = new std::string(*(cmd.effect));
    }
    return *this;
}

// stream insertion operator
std::ostream& operator<<(std::ostream& out, const Command& cmd) {
    out << "Command: " << *(cmd.command) << ", Effect: " << *(cmd.effect);
    return out;
}

// destructor
Command::~Command() {
    delete command;
    delete effect;
}

// saveEffect method saves the effect of the command
void Command::saveEffect(const std::string& eff) {
    delete effect;
    effect = new std::string(eff);
    // notify the Observer
    notify(*this);
}

// simple getter method to get the command string
std::string Command::getCommand() const {
    return *command;
}

// simple getter method to get the command effect
std::string Command::getEffect() const {
    return *effect;
}

// this function will create a string(effect) that will be save in "gamelog.txt"
std::string Command::stringToLog(){
    std::string string = "Command's Effect: ";
    string += getEffect();
    return string;
}

//
// FileCommandProcessorAdapter class implementation
//

//
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string& filename) : CommandProcessor() {
    flr = new FileLineReader(filename);
}

// copy constructor
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& adapter) 
    : CommandProcessor(adapter) {
    flr = new FileLineReader(*(adapter.flr));
}

// assignment operator
FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& adapter) {
    if (this != &adapter){
        CommandProcessor::operator=(adapter);
        delete flr;
        flr = new FileLineReader(*(adapter.flr));
    }
    return *this;
}

// stream insertion operator
std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& adapter) {
    out << "FileCommandProcessorAdapter using file: " << *(adapter.flr);
    return out;
}

// destructor
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete flr;
}   

// readCommand method overridden to read from file
void FileCommandProcessorAdapter::readCommand() {
    std::string line = flr->readLineFromFile();
    if (!line.empty()){
        saveCommand(line);
    }
}

std::string FileCommandProcessorAdapter::getCommand(const std::string& gameState) {
    readCommand();
    // if the list is not empty, retrieve the first command
    std::list<Command>* commandList = getCommands();  // Use protected getter
    if (!commandList->empty()) {
        Command* cmd = new Command(commandList->front());
        commandList->pop_front();

        // create a logObserver and attach it to the current command object.
        // this will need to be removed and deleted just before the Command object is deleted
        logObserver observer;
        cmd->attach(observer);
        
        bool isValid = validate(cmd, gameState);

        cmd->detach(observer);

        std::string commandStr = cmd->getCommand();
        std::string baseCommand = commandStr;
        size_t space = commandStr.find(' ');

        if (space != std::string::npos){
            baseCommand = commandStr.substr(0, space);
        }
        
        // Print the command's effect using the getter
        std::cout << "Effect: " << cmd->getEffect() << std::endl;
        
        if (isValid) {
            delete cmd;
            return baseCommand;
        } else {
            delete cmd;
            return "invalid";
        }
    }
    return "";
}

//
// FileLineReader class implementation
//

// constructor
FileLineReader::FileLineReader(const std::string& fname) {
    filename = new std::string(fname);
    file_.open(*filename);
}

// copy constructor
FileLineReader::FileLineReader(const FileLineReader& flr) {
    filename = new std::string(*(flr.filename));
    file_.open(*filename);
}   

// assignment operator
FileLineReader& FileLineReader::operator=(const FileLineReader& flr) {
    if (this != &flr){
        // Close current file before deleting and re-opening
        if (file_.is_open()) {
            file_.close();
        }
        delete filename;
        filename = new std::string(*(flr.filename));
        // CRITICAL: Open the file for the current instance
        file_.open(*filename); 
    }
    return *this;
}

// stream insertion operator
std::ostream& operator<<(std::ostream& out, const FileLineReader& flr) {
    out << "FileLineReader reading from file: " << *(flr.filename);
    return out;
}

// destructor
FileLineReader::~FileLineReader() {
    delete filename;
}

// readLineFromFile method reads a line from the file
std::string FileLineReader::readLineFromFile() {
    std::string line;
    
    if (std::getline(file_, line)){
        return line;
    }

    return "";


    /* Use static file stream to maintain state between calls
    static std::ifstream file;
    static bool isFirstCall = true;
    
    // Only open the file on first call
    if (isFirstCall) {
        file.open(*filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << *filename << std::endl;
            return "";
        }
        isFirstCall = false;
    }
    
    std::string line;
    if (std::getline(file, line)) {
        return line;
    }
    
    // Reset the file stream when we reach the end
    file.close();
    isFirstCall = true;
    return "";*/
}
