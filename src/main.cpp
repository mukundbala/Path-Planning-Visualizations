#include <iostream>
#include <point.hpp>
#include <obstacles.hpp>
#include <TGUI/TGUI.hpp>
#include <gui.hpp>
int main() {
    // sf::RenderWindow test_window{{800, 600}, "TGUI window with SFML"};
    // tgui::GuiSFML gui{test_window};
    // tgui::Theme::setDefault("themes/Black.txt");
    // // tabs->setTabHeight(30);
    // // tabs->setPosition(70, 40);
    // // tabs->add("Tab - 1");
    // // tabs->add("Tab - 2");
    // // tabs->add("Tab - 3");
    // // gui.add(tabs);
    // // gui.mainLoop();
    // 
std::shared_ptr<AppData> data= std::make_shared<AppData>();
PlannerGUI gui (data);
gui.run();
}