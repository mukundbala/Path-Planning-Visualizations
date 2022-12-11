#include <iostream>
#include <point.hpp>
#include <obstacles.hpp>
#include <TGUI/TGUI.hpp>
#include <gui.hpp>
int main() {
std::shared_ptr<AppData> data= std::make_shared<AppData>();
PlannerGUI gui (data);
gui.run();
bool isDataOk = gui.DataTest();
if (!isDataOk)
{
    std::cout<<"Exiting App\n";
    return 0;
}
}