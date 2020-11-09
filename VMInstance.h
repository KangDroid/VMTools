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
    bool create_args();
public:
    VMInstance();
    ~VMInstance();
    bool turn_on_vm();
    bool turn_off_vm();
};
