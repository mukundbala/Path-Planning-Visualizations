#include <data.hpp>
AppData::AppData()
{
    chosen_map_type_= "";
    chosen_planner_= "";
    loader=YAML::LoadFile("config/config.yaml");
    gui_window_x_ = 800;
    gui_window_y_ = 600;
    control_pane_width = 200;
    map_y_ = 980;
    map_x_= loader["map_width"].as<double>();
    continuous_planners = loader["continuous_planner"].as<std::vector<std::string>>();
    discrete_planners = loader["discrete_planner"].as<std::vector<std::string>>();
    resolution_ = std::make_pair(loader["resolution"].as<std::vector<int>>()[0],loader["resolution"].as<std::vector<int>>()[1]);
    num_random_obstacles = loader["num_random_obstacles"].as<int>();
    EPS = 1e-6; //load this from the yaml file!
    if (resolution_.first != resolution_.second)
    {
        resolution_ = std::make_pair(20,20);
        std::cout<<"[AppData]: Invalid Resolution! Defaulting to (20,20)\n";
    }

    else if (resolution_.first > 60)
    {
        resolution_ = std::make_pair(20,20);
        std::cout<<"[AppData]: Invalid Resolution! Defaulting to (20,20)\n";
    }

    start_pt_.setCoords(-1,-1);
    start_pt_.idx(-1);
    start_pt_.cost(-1);
    end_pt_.setCoords(-1,-1);
    end_pt_.cost(-1);
    end_pt_.idx(-1);

    std::cout<<"[AppData]: List of planners loaded!\n";
}

AppData::~AppData()
{
    loader.~Node();
}

std::string AppData::getChosenMap()
{
    return this->chosen_map_type_;
}

std::string AppData::getChosenPlanner()
{
    return this->chosen_planner_;
}

int AppData::getGUIWindowX()const
{
    return this->gui_window_x_;
}

int AppData::getGUIWindowY()const
{
    return this->gui_window_y_;
}

int AppData::getControlPaneWidth()const
{
    return this->control_pane_width;
}

int AppData::getMapX()const
{
    return this->map_x_;
}

int AppData::getMapY()const
{
    return this->map_y_;
}

int AppData::getNumRandomObs()const
{
    return this->num_random_obstacles;
}

double AppData::getEPS()const
{
    return this->EPS;
}

Vec2D AppData::getStartPoint()const
{
    return this->start_pt_;
}

Vec2D AppData::getEndPoint()const
{
    return this->end_pt_;
}

std::pair<int,int> AppData::getResolution()const
{
    return this->resolution_;
}

void AppData::setChosenMap(std::string type)
{
    this->chosen_map_type_ = type;
}

void AppData::setChosenPlanner(std::string type)
{
    this->chosen_planner_ = type;
}

void AppData::setStart(Vec2D &start)
{
    this->start_pt_ = start;
}

void AppData::setEnd(Vec2D &end)
{
    this->end_pt_ = end;
}