#ifndef GUI_HPP
#define GUI_HPP
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <memory>
#include <random>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include <obstacles.hpp>
#include <data.hpp>
#include <yaml-cpp/yaml.h>
enum class ChooserState:unsigned short
{
    HomeScreen,
    ChooseMapType,
    ContinuousMapSelector,
    DiscreteMapSelector,
    Done,
    Quit,
};

enum class SelectorState:unsigned short
{
    NotStarted,
    DrawCircle,
    DrawRectangle,
    ResetLastCircle,
    ResetLastRectangle,
    ResetAll,
    Random,
    SaveMap,
    LoadMap,
    Done,
    Quit,
};

class PlannerGUI
{
private:
    //gui sizes
    sf::Vector2u gui_window_chooser_size_; //size (x,y) of home window
    sf::Vector2u gui_window_selector_size_; //size (x,y) of selector window
    
    //gui windows and backend
    sf::RenderWindow gui_window_chooser_;
    sf::RenderWindow gui_window_selector_;
    tgui::GuiSFML gui_backend_chooser_;
    tgui::GuiSFML gui_backend_selector_;
    sf::Color background_color_;
    //themes
    tgui::Theme dark_theme_;
    tgui::Theme blue_theme_;
    //GUIStates
    ChooserState chooser_state_;
    SelectorState selector_state_;
    std::shared_ptr<AppData> app_data_;
    sf::Vector2f default_rectobs_size_; //used for generating obstacles in random mode
    double default_circobs_radius_; //used for generating obstacles in random mode
    std::pair<int,int> resolution_;
    double EPS;
    bool run_state_;

    //Buttons
    tgui::Label::Ptr label_title; //Home screen title
    tgui::Button::Ptr go_button; //Home screen go button
    tgui::Button::Ptr exit_button; //Home screen exit button
    sf::Texture img_texture; //Home screen img texture
    sf::Sprite img_sprite; //Home screen img sprite
    tgui::CanvasSFML::Ptr canvas;
    tgui::Label::Ptr label_description; //map type selection description
    tgui::Button::Ptr continuous_map_button; //button to select continuous map
    tgui::Button::Ptr discrete_map_button; //button to select discrete map

    std::vector<tgui::Button::Ptr> continuous_map_buttons;
    tgui::Label::Ptr continuous_planners_label;
    std::vector<tgui::Button::Ptr> discrete_map_buttons;
    tgui::Label::Ptr discrete_planners_label;

    tgui::Button::Ptr circle_obs_button;
    tgui::Button::Ptr rectangle_obs_button;
    tgui::Button::Ptr reset_last_circle_obs_button;
    tgui::Button::Ptr reset_last_rectangle_obs_button;
    tgui::Button::Ptr reset_all_obstacle_button;
    tgui::Button::Ptr random_obstacle_button;
    tgui::Button::Ptr save_map_button;
    tgui::Button::Ptr load_map_button;
    tgui::Button::Ptr done_button;
    tgui::Button::Ptr quit_button;

public:
    PlannerGUI(std::shared_ptr<AppData> data);
    ~ PlannerGUI() noexcept;
    bool runGui();
    void runMapAndPlannerChooser();
    void runObstacleSelector();
    //Screens
    void DrawHomeScreen();
    void DrawMapTypeScreen();
    void DrawDiscreteMapSelection();
    void DrawContinuousMapSelection();
    void ChoosePlannerCallback(std::string planner);
    void GenerateRandomContinuous();
    void DrawControlPane();
    void DrawSelectorWindow();
    void GenerateGrids();
    void GenerateRandomDiscrete(std::vector<int> &discrete_point_tracker);
    bool checkDiscretePoint(const Vec2D &grid_space_coord,std::vector<int> &discrete_point_tracker, int map_size_x);
    bool checkCircleOverflow(const sf::Vector2f &centre , double radius ,  double x_lower , double x_higher , double y_lower , double y_higher);
    bool DataTest(); 
    //widgets

};


#endif // GUI_HPP