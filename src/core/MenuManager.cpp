#include "include/main_state.h"
#include "include/interface/Login_Interface.h"
#include "include/interface/Admin_Interface.h"
#include "include/interface/Agent_Interface.h"
#include <iostream>

ApplicationScreen current_screen = ApplicationScreen::Login;

int main() {

    while (current_screen != ApplicationScreen::Exit) {
        switch (current_screen) {
            case ApplicationScreen::Login:
                ShowLoginScreen();
                break;
            case ApplicationScreen::AdminMenu:
                ShowAdminMenu();
                break;
            case ApplicationScreen::AgentMenu:
                ShowAgentMenu();
        }
    }
    return 0;
}