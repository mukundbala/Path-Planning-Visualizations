#ifndef MAP_HPP
#define MAP_HPP

#include <point.hpp>
#include <obstacles.hpp>
#include <data.hpp>
#include <continuous_planner.hpp>
#include <RRT.hpp>
#include <RRTStar.hpp>
#include <RRTMarch.hpp>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>
/*
Map classes should only handle display related tasks
- Drawing obstacles
- Drawing grids (for discrete)
- Drawing obstacles
- Drawing the path generated by the planner
*/
enum class MapState:unsigned short
{
    NotStarted,
    UndoSelections,
    ConfirmPoints,
    PrepComplete,
    StartPlanner,
    PlanningComplete,
    ShowPath,
    ShowAll,
    Quit,
};

class BaseMap
{
protected:
//resources
std::shared_ptr<AppData> data_;
sf::RenderWindow map_window_;
tgui::GuiSFML map_window_gui_;
tgui::Theme dark_theme_;

//data pertaining to the map
int map_x_;
int map_y_;
int control_pane_width_;
int window_x_;
int window_y_;
int clicks_;

std::string planner_name_;
std::string map_name_;

MapState program_state_;
std::vector<std::string> states_; //string representation of program states

public:
BaseMap(std::shared_ptr<AppData> my_data);
~BaseMap();

void createControlPane();
void drawControlPane();
void showCurrentState();

tgui::Label::Ptr message_label;
tgui::Button::Ptr undo_selections_button;
tgui::Button::Ptr start_planning_button;
tgui::Button::Ptr show_path_button;
tgui::Button::Ptr show_all_button;
tgui::Button::Ptr quit_button;
tgui::Button::Ptr confirm_points_button;
sf::RectangleShape control_pane_background;

sf::Color LINE_COLOR;
sf::Color POINT_COLOR;
sf::Color WINDOW_COLOR;
sf::Color START_COLOR;
sf::Color END_COLOR;
sf::Color PATH_COLOR;
sf::Color CONTROL_PANE_COLOR;

};

class ContinuousMap:public BaseMap
{
private:
int num_circle_obs, num_rect_obs, num_total_obs;
std::shared_ptr<ContinuousPlanner> continuous_planner_;
Vec2D start_pt_;
Vec2D end_pt_;
double point_radius_;
public:
    ContinuousMap(std::shared_ptr<AppData> my_data);
    ~ContinuousMap();
    
    void run();

    void drawPoint(const sf::Vector2f &pt, double radius, const sf::Color& color);
    void drawPoint(const Vec2D &pt,double radius, const sf::Color& color);
    void drawLine(const sf::Vector2f &pt1, const sf::Vector2f &pt2, const sf::Color& color);
    void drawLine(const Vec2D &pt1, const Vec2D &pt2, const sf::Color& color);
    
    void drawStartEnd();
    void drawObstacles();
    void drawNodes();
    void drawEdges();
    void drawPath();
};

class DiscreteMap:public BaseMap
{
private:
int num_obs;
std::string chosen_discrete_planner;

public:
    DiscreteMap(std::shared_ptr<AppData> my_data);
    ~DiscreteMap();

    void run();
};

#endif //MAP_HPP