#include <iostream>
#include <filesystem>

#include <unistd.h>
#include <sys/wait.h>
#include "VMInstance.h"

using namespace std;

int main(int argc, char** argv) {
    VMInstance vm_instance;
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " on/off" << " ssh_username"<< endl;
        exit(EXIT_FAILURE);
    }

    // Turn ON/OFF?
    if (!strcmp(argv[1], "on")) {
        if (argc == 3) {
            // there is ssh username
            vm_instance.set_ssh_user(string(argv[2]));
        }
        if (!vm_instance.turn_on_vm()) {
            cerr << "Turning on VM returned an error." << endl;
            return EXIT_FAILURE;
        }
    } else if (!strcmp(argv[1], "off")) {
        // turn off
        if (!vm_instance.turn_off_vm()) {
            cerr << "Turning off VM returned an error" << endl;
            return EXIT_FAILURE;
        }
    } else {
        cerr << "Unknown Argument: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}