#ifndef GUI_HPP
#define GUI_HPP
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <memory>
#include <random>
#include <obstacles.hpp>
#include <yaml-cpp/yaml.h>

/*
App data contains the information needed to pass information to the planner based
on user input
*/
class AppData
{
private:
    std::string chosen_map_type_; //the map type (discrete or continuous)
    std::string chosen_obstacle_mode; //custom or random
    std::string chosen_planner_; //the type of planner

    int gui_window_x_;
    int gui_window_y_;
    int control_pane_width;
    int map_y_;
    int map_x_; //width of the map. This will be the only thing a user can control
    
    std::pair<int,int> resolution_; //resolution of discrete planner
    YAML::Node loader;
public:
    AppData();
    
    std::string getChosenMap();
    
    std::string getChosenPlanner();

    int getGUIWindowX();
    int getGUIWindowY();
    int getControlPaneWidth();
    int getMapY();
    int getMapX();
    
    void setChosenMap(std::string type);
    
    void setChosenPlanner(std::string type);

    std::vector<std::string> continuous_planners;
    
    std::vector<std::string> discrete_planners;

    ObstacleArray<RectangleObstacle,std::vector> rect_obs_array;

    ObstacleArray<CircleObstacle,std::vector> circle_obs_array;

};

class PlannerGUI
{
private:
    sf::Vector2u gui_window_chooser_size_;
    sf::Vector2u gui_window_selector_size_;
    sf::RenderWindow gui_window_chooser_;
    sf::RenderWindow gui_window_selector_;
    tgui::GuiSFML gui_backend_chooser_;
    tgui::GuiSFML gui_backend_selector_;
    sf::Color background_color_;
    tgui::Theme dark_theme_;
    tgui::Theme blue_theme_;
    int screen_num;
    std::shared_ptr<AppData> app_data_;

public:
    PlannerGUI(std::shared_ptr<AppData> data);
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

    void GenerateRandomContinuous();
    void GenerateRandomDiscrete();
    
    
    
    

    enum class Actions:unsigned short
    {
        NotStarted,
        DrawCircle,
        DrawRectangle,
        ResetLastCircle,
        ResetLastRectangle,
        ResetAll,
        RandomRect,
        RandomCircle,
        RandomMix,
        Done,
    };
    //widgets

};

#endif // GUI_HPP