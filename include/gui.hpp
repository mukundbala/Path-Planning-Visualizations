#ifndef GUI_HPP
#define GUI_HPP
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <memory>
#include <random>
#include <algorithm>
#include <obstacles.hpp>
#include <data.hpp>
#include <yaml-cpp/yaml.h>

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
    sf::Vector2f default_rectobs_size_; //used for generating obstacles in random mode
    double default_circobs_radius_; //used for generating obstacles in random mode
    std::pair<int,int> resolution_;
    double EPS;
    bool run_state_;

public:
    PlannerGUI(std::shared_ptr<AppData> data);
    ~ PlannerGUI() noexcept;
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

    void GenerateGrids();
    void GenerateRandomDiscrete(std::vector<int> &discrete_point_tracker);
    bool checkDiscretePoint(const Vec2D &grid_space_coord,std::vector<int> &discrete_point_tracker, int map_size_x);

    bool DataTest(); 
    enum class Actions:unsigned short
    {
        NotStarted,
        DrawCircle,
        DrawRectangle,
        ResetLastCircle,
        ResetLastRectangle,
        ResetAll,
        Random,
        Done,
    };
    //widgets

};


#endif // GUI_HPP