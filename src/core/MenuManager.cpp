#include "C:/PBL2/include/core/MenuManager.h"
#include "C:/PBL2/include/core/AccountManager.h"

void app(){
    AccountManager account_manager("C:/PBL2/data/admins.txt", "C:/PBL2/data/agents.txt");

    while (current_screen != ApplicationScreen::Exit) {
        switch (current_screen) {
            case ApplicationScreen::Login:
                ShowLoginScreen(account_manager);
                break;
            case ApplicationScreen::AdminMenu:
                ShowAdminMenu(account_manager);
                break;
            case ApplicationScreen::AgentMenu:
                ShowAgentMenu();
            case ApplicationScreen::Exit:
                exit(0);
        }
    }
}