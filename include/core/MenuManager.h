#include "include/main_state.h"
#include "include/interface/Login_Interface.h"
#include "include/interface/Admin_Interface.h"
#include "include/interface/Agent_Interface.h"
#include <iostream>

ApplicationScreen current_screen = ApplicationScreen::Login;

void app();