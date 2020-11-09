#include <iostream>
#include <filesystem>

#include <unistd.h>
#include <sys/wait.h>
#include "VMInstance.h"

using namespace std;

int main(int argc, char** argv) {
    VMInstance vm_instance;
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " on/off" << endl;
        exit(EXIT_FAILURE);
    }
    if (!strcmp(argv[1], "on")) {
        vm_instance.turn_on_vm();
    } else if (!strcmp(argv[1], "off")) {
        // turn off
        vm_instance.turn_off_vm();
    } else {
        cerr << "Unknown Argument: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}