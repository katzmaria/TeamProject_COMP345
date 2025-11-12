#pragma once

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include "LoggingObserver.h"


class Command : public ILoggable, public Subject {
    private: 
        std::string* command;
        std::string* effect;
    public:
        Command();
        Command(const std::string& cmd);
        Command(const Command& cmd);
        Command& operator=(const Command& cmd);
        friend std::ostream& operator<<(std::ostream& out, const Command& cmd);
        ~Command();
        void saveEffect(const std::string& eff);
        std::string getCommand() const;
        std::string getEffect() const;
        std::string stringToLog();
};

class CommandProcessor : public ILoggable, public Subject {
    private:
        std::list<Command>* commands;
    protected:
        std::list<Command>* getCommands() { return commands; }
        virtual void readCommand();
        void saveCommand(const std::string& command);
    public:
        CommandProcessor();
        CommandProcessor(const CommandProcessor& processor);
        CommandProcessor& operator=(const CommandProcessor& processor);
        friend std::ostream& operator<<(std::ostream& out, const CommandProcessor& processor);
        ~CommandProcessor();
        virtual std::string getCommand(const std::string& gameState);
        bool validate(Command* cmd, const std::string& currentState);
        std::string stringToLog();
};

class FileLineReader {
    private:
        std::string* filename;
        std::ifstream file_;
    public:
        FileLineReader(const std::string& fname);
        FileLineReader(const FileLineReader& flr);
        FileLineReader& operator=(const FileLineReader& flr);
        friend std::ostream& operator<<(std::ostream& out, const FileLineReader& flr);
        ~FileLineReader();
        std::string readLineFromFile();
};

class FileCommandProcessorAdapter : public CommandProcessor {
    private:
        FileLineReader* flr;
    protected:
        void readCommand() override;
    public:
        FileCommandProcessorAdapter(const std::string& filename);
        FileCommandProcessorAdapter(const FileCommandProcessorAdapter& adapter);
        FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& adapter);
        friend std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& adapter);
        ~FileCommandProcessorAdapter();
        std::string getCommand(const std::string& gameState) override;
};
