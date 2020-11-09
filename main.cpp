#include <iostream>
#include <filesystem>

#include <unistd.h>
#include <sys/wait.h>
#include "VMInstance.h"
#include "Logger.h"

using namespace std;

void when_exit() {
    LOG_V("Program will exit now");
    Logger::close_stream();
}

int main(int argc, char** argv) {
    Logger::initiate_stream("Log.txt");
    VMInstance vm_instance;
    LOG_V("Starting Program " + string(argv[0]));
    // We need to close stream when abnormal exit is called.
    atexit(when_exit);
    
    // Verbose Argument
    for (int i = 0; i < argc; i++) {
        string message = "Args[" + to_string(i) + "]: " + argv[i];
        LOG_V(message);
    }

    if (argc < 2) {
        LOG_E("Program needs at least 2 or more argument to run.");
        cerr << "Usage: " << argv[0] << " on/off" << " ssh_username"<< endl;
        exit(EXIT_FAILURE);
    }

    // Turn ON/OFF?
    if (!strcmp(argv[1], "on")) {
        LOG_V("Program will TURN ON VM Instance.");
        if (argc == 3) {
            // there is ssh username
            LOG_V("SSH Username found: " + string(argv[2]));
            vm_instance.set_ssh_user(string(argv[2]));
        }
        if (!vm_instance.turn_on_vm()) {
            cerr << "Turning on VM returned an error." << endl;
            LOG_E("turn_on_vm returned false. See above log for more details.");
            return EXIT_FAILURE;
        }
    } else if (!strcmp(argv[1], "off")) {
        LOG_V("Program will TURN OFF VM Instance.");
        // turn off
        if (!vm_instance.turn_off_vm()) {
            cerr << "Turning off VM returned an error" << endl;
            LOG_E("turn_off_vm returned false. See above log for more details.");
            return EXIT_FAILURE;
        }
    } else {
        cerr << "Unknown Argument: " << argv[1] << endl;
        LOG_E("Unknown Argument Found: " + string(argv[1]));
        exit(EXIT_FAILURE);
    }
    return 0;
}