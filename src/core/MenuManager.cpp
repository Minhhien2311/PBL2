#include "include/core/MenuManager.h"

void app(){
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
}