#ifndef DATA_HPP
#define DATA_HPP
#include <string>
#include <obstacles.hpp>
#include <yaml-cpp/yaml.h>
class AppData
{
private:
    std::string chosen_map_type_; //the map type (discrete or continuous)
    std::string chosen_planner_; //the type of planner
    int gui_window_x_; //width of the window size in x
    int gui_window_y_; //length of the window size in y
    int control_pane_width; //length of the control pane
    int map_x_; //width of the map. This will be the only thing a user can control
    int map_y_; //length of the map
    
    Vec2D start_pt_; //start point of mapping
    Vec2D end_pt_; //end point of mapping

    std::pair<int,int> resolution_; //resolution of discrete planner
    double EPS_; //Epsilon to do equality for floating/double data
    int num_random_obstacles_; //number of random_obstacles
    YAML::Node loader;
public:
    AppData(); //constructor that loads all data  from yaml file

    ~AppData(); //destructor

    std::string getChosenMap(); //return name of chosen map

    std::string getChosenPlanner(); //return name of chosen planner

    int getGUIWindowX()const; //return the width of gui window

    int getGUIWindowY()const; //return the length of gui window

    int getControlPaneWidth()const; //return the width of the control pane

    int getMapX()const; //return the width of the window

    int getMapY()const; //return the length of the window

    int getNumRandomObs()const; //get the number of random obstacles

    double getEPS()const; //get epsilon

    std::pair<int,int> getResolution()const; //get the resolution of discrete map

    Vec2D getStartPoint()const; //get the start point

    Vec2D getEndPoint()const; //get the end point

    void setChosenMap(std::string type); //set chosen map name
    
    void setChosenPlanner(std::string type); //set chosen planner name

    void setStart(Vec2D &start); //set start point

    void setEnd(Vec2D &end); //set end point

    std::vector<std::string> continuous_planners; //a vector containing continuous planner names. Matches input in yaml file
    
    std::vector<std::string> discrete_planners; //a vector containing discrete planner names. Matches input in the yaml file

    ObstacleArray<RectangleObstacle,std::vector> rect_obs_array; //vector containing rectangle obstacles

    ObstacleArray<CircleObstacle,std::vector> circle_obs_array; //vector containing circle obstacles
};


#endif //DATA_HPP