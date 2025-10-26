#include <iostream>
#include "core/App.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

int main() {
    std::cout << "[DEBUG] main start\n";
#ifdef DATA_DIR
    std::cout << "[DEBUG] DATA_DIR macro expansion: " << STR(DATA_DIR) << "\n";
#else
    std::cout << "[DEBUG] DATA_DIR not defined\n";
#endif

    try {
        std::cout << "[DEBUG] constructing App...\n";
        App app;
        std::cout << "[DEBUG] App constructed\n";
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] std::exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[ERROR] unknown exception thrown\n";
        return 2;
    }

    std::cout << "[DEBUG] main end\n";
    return 0;
}