#include <iostream>
#include <filesystem>
#include <vector>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/wait.h>

using namespace std;

/**
 * VM Instance has 3 state, which
 * 0: TURNED_ON | true for vm on, false for vm off.
 * 1: CONTROL_IS_GUI | true for Launch VM Application, false for NONGUI option
 * 2: CONTROL_TURN_ON | Control Boolean for whether to turn on or turn off. Used when creating argument. True for turning on VM, False for turning off vm.
 * Those values are initially set to false.
 */
#define STATE_SIZE 3

#define TURNED_ON 0
#define CONTROL_IS_GUI 1 // BIT IS DEFINED WHEN CLASS GENERATION(CONSTRUCTOR)
#define CONTROL_TURN_ON 2 // BIT IS DEFINED WHEN FUNCTION STARTS

/**
 * Default Value
 */
#define DEFAULT_VM_TYPE "fusion"

class VMInstance {
private:
    const static size_t argument_size = 40; // Limit: 40 Arguments
    const static size_t argument_buffer = 1024; // support 1024 characters per each argument
    bool state[STATE_SIZE] = {false, false, false};
    filesystem::path vmx_path;

    /**
     * VMRUN Binary path whitelist.
     * Add more if there any.
     */
    vector<filesystem::path> vmrun_whitelist = {"/Applications/VMware Fusion.app/Contents/Public/vmrun"};
    filesystem::path vmrun_path; // VMRUN Binary Path

    // VM Type, ws or fusion
    string vm_type;

    // argument of buffer --> Dynamically generated.
    char** buffer_args;

    // Functions
    bool create_args() {
        string verbose_information = "";
        buffer_args = new char*[argument_size];

        // VMRUN
        buffer_args[0] = new char[argument_buffer];
        strcpy(buffer_args[0], "vmrun");
        verbose_information += string(buffer_args[0]);

        // -T
        buffer_args[1] = new char[argument_buffer];
        strcpy(buffer_args[1], "-T");
        verbose_information += " " + string(buffer_args[1]);

        // FUSION --> VMWare Type
        buffer_args[2] = new char[argument_buffer];
        // error checking support for VM Type, using default.
        if (vm_type != "ws" && vm_type != "fusion") {
            cerr << "Using Default option for -T \"vm_type\", which is fusion." << endl;
            vm_type = string(DEFAULT_VM_TYPE);
        }
        strcpy(buffer_args[2], vm_type.c_str());
        verbose_information += " " + string(buffer_args[2]);

        // Start or Stop?
        buffer_args[3] = new char[argument_buffer];
        (this->state[CONTROL_TURN_ON]) ? strcpy(buffer_args[3], "start") : strcpy(buffer_args[3], "stop");
        verbose_information += " " + string(buffer_args[3]);

        // VMX Location
        buffer_args[4] = new char[argument_buffer];
        // TODO: Error checking - what if path is NOT VMX?
        // We need to define CXX String first so we can use quotes.
        strcpy(buffer_args[4], filesystem::absolute(this->vmx_path).c_str());
        verbose_information += " " + string(buffer_args[4]);

        // GUI Part
        buffer_args[5] = new char[argument_buffer];
        if (this->state[CONTROL_TURN_ON]) {
            (state[CONTROL_IS_GUI]) ? strcpy(buffer_args[5], "gui") : strcpy(buffer_args[5], "nogui");
        } else {
            // TODO: Soft or hard?
            strcpy(buffer_args[5], "soft");
        }
        verbose_information += " " + string(buffer_args[5]);


        cout << "Created Argument: " << verbose_information << endl;

        // Last PTR should be null at all.
        buffer_args[6] = nullptr;

        return true;
    }
public:
    VMInstance() {
        // Find VMRUN First.
        this->vmrun_path = filesystem::path("/");
        for (filesystem::path& whitelist : vmrun_whitelist) {
            if (filesystem::exists(whitelist)) {
                this->vmrun_path = whitelist;
                cout << "VMRUN Path: " << this->vmrun_path << endl;
                break;
            }
        }

        // if this state reached, that means there is no binary.
        if (this->vmrun_path == filesystem::path("/")) {
            // NO VMRUN Binary found.
            cerr << "VMRUN binary is not found on whitelist of this program. \nManually add vmrun binary path on this program!" << endl;
            exit(EXIT_FAILURE);
        }

        // for now, set gui to false
        this->state[CONTROL_IS_GUI] = false;

        // for now, set vmx to mine
        this->vmx_path = filesystem::absolute(filesystem::path("/Users/KangDroid/Virtual Machines.localized/Ubuntu 64-bit Server 20.10.vmwarevm/Ubuntu 64-bit Server 20.10.vmx"));
        if (!filesystem::exists(this->vmx_path)) {
            cerr << "VMX Not found" << endl;
            exit(EXIT_FAILURE);
        }
    }
    ~VMInstance() {
        if (buffer_args != nullptr) {
            for (int i = 0; i < argument_size; i++) {
                if (buffer_args[i] != nullptr) {
                    delete[] buffer_args[i];
                }
            }
            delete[] buffer_args;
        }
    }
    bool turn_on_vm() {
        // Both PR/CHILD should share control bits
        this->state[CONTROL_TURN_ON] = true;
        pid_t fork_id = fork();
        if (fork_id == 0) {
            create_args();
            execv(this->vmrun_path.c_str(), this->buffer_args);
        } else {
            int return_value;
            wait(&return_value);
            cout << "Process Returned: " << WEXITSTATUS(return_value) << endl;
        }

        return true;
    }

    bool turn_off_vm() {
        // Both PR/CHILD should share control bits
        this->state[CONTROL_TURN_ON] = false;
        pid_t fork_id = fork();
        if (fork_id == 0) {
            create_args();
            execv(this->vmrun_path.c_str(), this->buffer_args);
        } else {
            int return_value;
            wait(&return_value);
            cout << "Process Returned: " << WEXITSTATUS(return_value) << endl;
        }
        return true;
    }
};

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