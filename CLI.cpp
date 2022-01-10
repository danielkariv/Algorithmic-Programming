

#include "CLI.h"

CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    // Add commands in vector in order like the text example.
    this->commands.push_back(new UploadCSV(dio));
    this->commands.push_back(new Settings(dio));
    this->commands.push_back(new Detect(dio));
    this->commands.push_back(new Results(dio));
    this->commands.push_back(new UploadAnom(dio));
    this->commands.push_back(new Exit(dio));
}

void CLI::start(){
    SharedInformation shared;
    int option = -1;
    string welcome_message = "Welcome to the Anomaly Detection Server.\nPlease choose an option:\n";
    // stop looping only when input = 5 -> when we enter 6 (Exit command).
    while(option != 5){
        this->dio->write(welcome_message);
        // print all options
        for (int i = 0; i < commands.size(); ++i) {
            string text = to_string(i+1) + "." + commands[i]->getDescription() + "\n";
            this->dio->write(text);
        }

        string input = dio->read();
        // offset first char in input string by value of char '1'.
        // will return values between 0-5 for input '1'-'6'.
        option = input[0]-'1';
        if (0<= option && 6 >= option){
            // need to see how to deliver info in or out.
            commands[option]->execute(&shared);
        }
    }
}


CLI::~CLI() {
    // delete commands we made from last to first.
    for (int i = commands.size()-1; i >= 0; --i) {
        delete commands[i];
    }
}
