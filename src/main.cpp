#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <cstdarg>

#include <unistd.h>
#include <sys/wait.h>
#include "VMInstance.h"
#include "Logger.h"
#include "json/json.h"

using namespace std;

void when_exit() {
    LOG_V("Program will exit now");
    Logger::close_stream();
}

// Check target_one is EITHER target_two OR target_three. Using bit operator
#define COMPARE_STRING(target_two, target_three) ((!strcmp(argv[i], target_two)) | (!strcmp(argv[i], target_three)))

// Test range of argument, returns 1 if i in range, 0 when not in range.
#define test_range(cur_i) (cur_i+1 < argc) ? 1 : 0

#define error_exit(argument) {\
    cerr << "Value is not defined argument " << argument << endl;\
    LOG_E("Value is not defined for key: " + string(argument));\
    return (EXIT_FAILURE);\
}

class GlobalInfo {
public:
    static const int TURN_ON = 1;
    static const int TURN_OFF = 0;
public:
    GlobalInfo() {
        this->json_path = filesystem::path("/");
        this->machine_identifier = -1;
        this->ssh_use = -1;
        this->turn_state = -100;
    }
    filesystem::path json_path;
    int machine_identifier;
    int ssh_use;
    int turn_state;
};

/**
 * Argument Supported:
 * -c | --config_path
 * -n | --number
 * on/off
 * -s | --ssh
 * -dl | --disable_log
 */
int main(int argc, char** argv) {
    // Initiate Logger
    time_t current_time;
    time(&current_time);
    char buffer_time[128];
    strftime(buffer_time, sizeof(buffer_time), "%Y%m%d-%H%M%S", localtime(&current_time));
    string file_name(buffer_time);
    if (!Logger::initiate_stream("/tmp/LOG_" + file_name + ".log")) {
        cerr << "Initiating logger failed." << endl;
        cerr << "Ignoring logger feature." << endl;
    }
    
    VMInstance vm_instance;
    LOG_V("Starting Program " + string(argv[0]));
    // We need to close stream when abnormal exit is called.
    atexit(when_exit);

    // Global Variable
    GlobalInfo global_info;
    
    // Check Argument
    for (int i = 1; i < argc; i++) {
        string message = "Args[" + to_string(i) + "]: " + argv[i];
        LOG_V(message);
        if (COMPARE_STRING("-c", "--config_path") == 1) {
            if (test_range(i)) {
                i++;
                global_info.json_path = filesystem::absolute(filesystem::path(argv[i]));
                // if there is NO json file, return error.
                if (!filesystem::exists(global_info.json_path)) {
                    cerr << "Specified json path " << global_info.json_path.string() << " not found." << endl;
                    return EXIT_FAILURE;
                }
                LOG_V("Json Configuration Path is set to: " + global_info.json_path.string());
            } else {
                // error
                error_exit("--config_path | -c");
            }
        } else if (COMPARE_STRING("-n", "--number") == 1) {
            if (test_range(i)) {
                i++;
                global_info.machine_identifier = atoi(argv[i]);
                LOG_V("Machine Identifier is set to: " + string(argv[i]));
            } else {
                error_exit("-n | --number");
            }
        } else if (COMPARE_STRING("-s", "--ssh")) {
            global_info.ssh_use = true;
            LOG_V("Using SSH after turning on Virtual Machine.");
        } else if (!strcmp(argv[i], "on")) {
            global_info.turn_state = global_info.TURN_ON;
            LOG_V("Global Turn state is set to ON");
        } else if (!strcmp(argv[i], "off")) {
            global_info.turn_state = global_info.TURN_OFF;
            LOG_V("Global Turn state is set to OFF");
        } else if (COMPARE_STRING("-dl", "--disable_log")) {
        } else {
            // undefined.
            cerr << "Undefined Argument " << argv[i] << endl;
            LOG_E("Undefined Argument is detected: " + string(argv[i]));
            return EXIT_FAILURE;
        }
    }

    // Check Argument
    if (global_info.json_path == filesystem::path("/")) {
        cout << "Json Path is not specified, by default, this program will use installation path" << endl;
        LOG_E("Json path is not defined, therefore using /Users/KangDroid/Desktop/kdr_vm_tools/sample.json for default json path.");
        global_info.json_path = filesystem::path("/Users/KangDroid/Desktop/kdr_vm_tools/sample.json");
    }
    if (global_info.machine_identifier == -1) {
        cout << "Machine Identifier is not defined, by default, this program will use 0." << endl;
        LOG_E("Machine Identifier is not defined, setting value to 0");
        global_info.machine_identifier = 0;
    }
    if (global_info.ssh_use == -1) {
        cout << "SSH is not defined, by default, this program will not change shell to ssh one" << endl;
        LOG_E("SSH is not defined, setting value to false[not-using].");
        global_info.ssh_use = true;
    }
    if (global_info.turn_state != global_info.TURN_ON && global_info.turn_state != global_info.TURN_OFF) {
        cerr << "Turning State is not defined." << endl;
        LOG_E("Turning state should be either on or off. Program did not detected information of turning state.");
        LOG_E("See above log for detailed - argument input.");
        return EXIT_FAILURE;
    }

    // Read JSON
    Json::Value root;
    Json::Reader json_reader;
    ifstream ifs(global_info.json_path);
    if (!json_reader.parse(ifs, root)) {
        cerr << "Cannot parse json!" << endl;
        return EXIT_FAILURE;
    }
    // Init VMX
    vm_instance.init_vmx_username(filesystem::path(root["machine"][global_info.machine_identifier]["machine_path"].asString()));
    
    // TURN ON/OFF
    if (global_info.turn_state == global_info.TURN_ON) {
        if (global_info.ssh_use) {
            // Init SSH
            vm_instance.set_ssh_user(root["machine"][global_info.machine_identifier]["machine_login"].asString());
        }

        if (!vm_instance.turn_on_vm()) {
            cerr << "Turning on VM returned an error." << endl;
            LOG_E("turn_on_vm returned false. See above log for more details.");
            return EXIT_FAILURE;
        }
    } else if (global_info.turn_state == global_info.TURN_OFF) {
        if (!vm_instance.turn_off_vm()) {
            cerr << "Turning off VM returned an error" << endl;
            LOG_E("turn_off_vm returned false. See above log for more details.");
            return EXIT_FAILURE;
        }
    }
    return 0;
}