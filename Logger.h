#ifndef __ERRORLOGGER_H__
#define __ERRORLOGGER_H__

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define LOG_E(to_message) Logger::log_e(__LINE__, __func__, to_message, false)
#define LOG_V(to_message) Logger::log_v(__LINE__, __func__, to_message, false)

class Logger {
public:
    static ofstream save;
    static string log_full;
    // Log_E for Error logging
    static void log_e(int line, string fcode, string output, bool is_command);

    // Log_V for verbose logging
    static void log_v(int line, string fcode, string output, bool is_command);

    // Timer function
    static string get_cur_time();

    // Initiate ofstream
    static bool initiate_stream(string path);

    // Close ofstream
    static bool close_stream();

    static void print_all() {
        cout << log_full << endl;
    }
};
#endif // __ERRORLOGGER_H__