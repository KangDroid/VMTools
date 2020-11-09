#include "VMInstance.h"
bool VMInstance::create_args() {
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

VMInstance::VMInstance() {
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

VMInstance::~VMInstance() {
    if (buffer_args != nullptr) {
        for (int i = 0; i < argument_size; i++) {
            if (buffer_args[i] != nullptr) {
                delete[] buffer_args[i];
            }
        }
        delete[] buffer_args;
    }
}

bool VMInstance::turn_on_vm() {
    // PIPE Support
    int fd[2];
    char ip_buffer[16];
    pipe(fd);

    // Both PR/CHILD should share control bits
    this->state[CONTROL_TURN_ON] = true;
    pid_t fork_id = fork();
    if (fork_id == 0) {
        create_args();
        execv(this->vmrun_path.c_str(), this->buffer_args);
    } else {
        int return_value;
        wait(&return_value);
        if (WEXITSTATUS(return_value) != 0) {
            cerr << "Process Returned: " << WEXITSTATUS(return_value) << endl;
            return false;
        }
    }

    // By default, get IP Information of VM Instance.
    cout << "Getting IP Information of VM Instance.." << endl;
    fork_id = fork();
    if (fork_id == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execl(this->vmrun_path.c_str(), "vmrun", "-T" ,this->vm_type.c_str(), "getGuestIPAddress", this->vmx_path.c_str(), "-wait", NULL);
    } else {
        int return_value;
        close(fd[1]);
        wait(&return_value);
        if (WEXITSTATUS(return_value) == 0) {
            int read_val = read(fd[0], ip_buffer, 16);
            // ip_buffer contains \n, trail that one.
            string tmp_str = string(ip_buffer);
            this->ip_addr = tmp_str.substr(0, tmp_str.find('\n'));
            cout << this->ip_addr << endl;
        }
    }
    return true;
}

bool VMInstance::turn_off_vm() {
    // Both PR/CHILD should share control bits
    this->state[CONTROL_TURN_ON] = false;
    pid_t fork_id = fork();
    if (fork_id == 0) {
        create_args();
        execv(this->vmrun_path.c_str(), this->buffer_args);
    } else {
        int return_value;
        wait(&return_value);
        if (WEXITSTATUS(return_value) != 0) {
            cerr << "Process Returned: " << WEXITSTATUS(return_value) << endl;
            return false;
        }
    }
    return true;
}
