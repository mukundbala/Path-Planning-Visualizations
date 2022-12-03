#ifndef GUI_HPP
#define GUI_HPP
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <memory>
#include <obstacles.hpp>
#include <yaml-cpp/yaml.h>

/*
App data contains the information needed to pass information to the planner based
on user input
*/
class AppData
{
private:
    std::string chosen_map_type_;
    std::string chosen_planner_;
    YAML::Node loader;
public:
    AppData();
    
    std::string getChosenMap();
    
    std::string getChosenPlanner();
    
    void setChosenMap(std::string type);
    
    void setChosenPlanner(std::string type);

    std::vector<std::string> continuous_planners;
    
    std::vector<std::string> discrete_planners;

    ObstacleArray<RectangleObstacle,std::vector> rect_obs;

    ObstacleArray<CircleObstacle,std::vector> circle_obs;

};

class PlannerGUI
{
private:
    sf::Vector2u gui_window_size_;
    sf::Vector2u selector_window_size_;
    sf::RenderWindow gui_window_;
    sf::RenderWindow gui_window_selector_;
    tgui::GuiSFML gui_backend_;
    tgui::GuiSFML gui_backend_selector_;
    sf::Color background_color_;
    tgui::Theme dark_theme_;
    tgui::Theme blue_theme_;
    int screen_num;
    std::shared_ptr<AppData> app_data_;

public:
    PlannerGUI(std::shared_ptr<AppData> data,int window_x,int window_y,int selector_window_x, int selector_window_y);
    void run();
    void runObstacleSelector();
    //callbacks
    void goCallback();
    void quitCallback();
    //Screens
    void DrawHomeScreen();
    void DrawMapTypeScreen();

    void ContinuousMapTypeCallback();
    void DiscreteMapTypeCallback();
    
    void DrawDiscreteMapSelection();
    void DrawContinuousMapSelection();
    void ChoosePlannerCallback(std::string planner);

    // void CircObsCallback();
    // void RectObsCallback();
    // void ClearLastCallback();
    // void ClearAllCallback();

    enum class Actions:unsigned short
    {
        NotStarted,
        DrawCircle,
        DrawRectangle,
        ResetLast,
        ResetAll,
    };
    //widgets

};

#endif // GUI_HPP