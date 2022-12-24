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

    int gui_window_x_;
    int gui_window_y_;
    int control_pane_width;
    int map_y_;
    int map_x_; //width of the map. This will be the only thing a user can control
    
    std::pair<int,int> resolution_; //resolution of discrete planner
    double EPS;
    int num_random_obstacles;
    YAML::Node loader;
public:
    AppData();
    
    std::string getChosenMap();
    
    std::string getChosenPlanner();

    int getGUIWindowX()const;
    int getGUIWindowY()const;
    int getControlPaneWidth()const;
    int getMapY()const;
    int getMapX()const;
    int getNumRandomObs()const;
    double getEPS()const;
    std::pair<int,int> getResolution()const;

    void setChosenMap(std::string type);
    
    void setChosenPlanner(std::string type);

    std::vector<std::string> continuous_planners;
    
    std::vector<std::string> discrete_planners;

    ObstacleArray<RectangleObstacle,std::vector> rect_obs_array;

    ObstacleArray<CircleObstacle,std::vector> circle_obs_array;
};


#endif //DATA_HPP