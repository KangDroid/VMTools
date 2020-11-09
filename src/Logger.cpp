#include "Logger.h"
string Logger::log_full = "";

ofstream Logger::save;

void Logger::log_e(int line, string function_name, string output, bool is_command) {
    string final_output;
    if (is_command) {
        final_output = get_cur_time() + "::" + "[E]/" + function_name + ":" + to_string(line) + ": "+ output;
    } else {
        final_output = get_cur_time() + "::" + "[E]/" + function_name + ":" + to_string(line) + ": "+ output + "\n";
    }
    
    log_full.append(final_output);
    save << final_output;
    save.flush();
}

void Logger::log_v(int line, string function_name, string output, bool is_command) {
    string final_output;
    if (is_command) {
        final_output = get_cur_time() + "::" + "[V]/" + function_name + ":" + to_string(line) + ": "+ output;
    } else {
        final_output = get_cur_time() + "::" + "[V]/" + function_name + ":" + to_string(line) + ": "+ output + "\n";
    }
    log_full.append(final_output);
    save << final_output;
    save.flush();
}

string Logger::get_cur_time() {
    time_t cur = time(NULL);
    struct tm tmp_timerst;
    char buffer[40];

    // init tm
    tmp_timerst = *localtime(&cur);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &tmp_timerst);

    return string(buffer);
}

bool Logger::initiate_stream(string path) {
    save.open(path);
    return save.is_open();
}

bool Logger::close_stream() {
    save.close();
    return !save.is_open();
}