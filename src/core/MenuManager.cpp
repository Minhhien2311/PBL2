#include "C:/PBL2/include/core/MenuManager.h"
#include "C:/PBL2/include/core/AccountManager.h"
#include "C:/PBL2/include/core/FlightManager.h"

void app(){
    AccountManager account_manager("C:/PBL2/data/admins.txt", "C:/PBL2/data/agents.txt");
    FlightManager flight_manager("C:/PBL2/data/flights.txt","C:/PBL2/data/flight_instances.txt");

    while (current_screen != ApplicationScreen::Exit) {
        switch (current_screen) {
            case ApplicationScreen::Login:
                ShowLoginScreen(account_manager);
                break;
            case ApplicationScreen::AdminMenu:
                ShowAdminMenu(account_manager,flight_manager);
                break;
            case ApplicationScreen::AgentMenu:
                ShowAgentMenu(account_manager,flight_manager);
        }
    }
}