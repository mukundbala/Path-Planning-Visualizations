#include <iostream>
#include <point.hpp>
#include <obstacles.hpp>
#include <TGUI/TGUI.hpp>
#include <gui.hpp>
#include <data.hpp>
#include <map.hpp>
int main() {

std::shared_ptr<AppData> data= std::make_shared<AppData>();
PlannerGUI gui (data);
bool status = gui.runGui();
if (!status)
{
    return 0;
}
bool isDataOk = gui.DataTest();
if (!isDataOk)
{
    std::cout<<"Exiting App\n";
    return 0;
}
if (data->getChosenMap() == "continuous")
{
    std::shared_ptr<ContinuousMap> continuous_map = std::make_shared<ContinuousMap>(data);
    continuous_map->run();
}
else if (data->getChosenMap() == "discrete")
{
    std::shared_ptr<DiscreteMap> discrete_map = std::make_shared<DiscreteMap>(data);
    discrete_map->run();
}
else
{
    return 0;
}
return 0;
}