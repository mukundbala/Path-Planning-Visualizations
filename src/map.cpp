#include <map.hpp>

BaseMap::BaseMap(std::shared_ptr<AppData>  my_data) : data_{my_data}
{
    map_x_ = data_->getMapX();
    map_y_ = data_->getMapY();
    control_pane_width_ = data_->getControlPaneWidth();
    //these are widths, converting this to 0 indexed values requires minus 1
    x_lower_ = control_pane_width_;
    y_lower_ = 0;
    x_upper_ = map_x_ + control_pane_width_;
    y_upper_ = map_y_;
    planner_name_ = data_->getChosenPlanner();
    map_name_ = data_->getChosenMap();
    toupper(map_name_[0]);
    dark_theme_.setDefault("themes/Black.txt");
    map_window_.create(sf::VideoMode(x_upper_,y_upper_,32),planner_name_);
    map_window_gui_.setTarget(map_window_);
    clicks_ = 0;
    program_state_ = MapState::NotStarted;
    states_ = {"Not Started", "Prep Complete" , "Undo Selections" , "Start Planner", "Planning Complete" , "Show Path" , "Show All" , "Quit"};

    createControlPane();

    LINE_COLOR = sf::Color::White;
    POINT_COLOR = sf::Color::Cyan;
    WINDOW_COLOR = sf::Color::Black;
    START_COLOR = sf::Color::Green;
    END_COLOR = sf::Color::Magenta;
    PATH_COLOR = sf::Color::Yellow;
    CONTROL_PANE_COLOR = sf::Color::Yellow;

    std::cout<<"[ContinuousMap]:Base Map Built!\n";
}


BaseMap::~BaseMap()
{
    data_.reset();
}

void BaseMap::createControlPane()
{
    this->map_window_gui_.removeAllWidgets();
    
    this->message_label = tgui::Label::create();
    message_label->setRenderer(dark_theme_.getRenderer("Label"));
    std::string message_label_text = "Please select start point(right mouse click)\nand end point (left mouse click)";
    message_label->setText(message_label_text);
    message_label->setSize(180,280);
    message_label->setPosition(0,0);
    message_label->setTextSize(20);
    
    this->start_planning_button=tgui::Button::create();
    start_planning_button -> setSize(160,80);
    start_planning_button -> setPosition(20,300);
    start_planning_button -> setText("StartPlanner");
    start_planning_button -> setTextSize(15);
    start_planning_button -> onMousePress([&]{this->program_state_ = MapState::StartPlanner;});
    
    this->undo_selections_button=tgui::Button::create();
    undo_selections_button -> setSize(160,80);
    undo_selections_button -> setPosition(20,400);
    undo_selections_button -> setText("Undo Selections");
    undo_selections_button -> setTextSize(15);
    undo_selections_button -> onMousePress([&]{this->program_state_ = MapState::UndoSelections;});

    this->confirm_points_button = tgui::Button::create();
    confirm_points_button -> setSize(160,80);
    confirm_points_button -> setPosition(20,500);
    confirm_points_button -> setText("Confirm Points?");
    confirm_points_button -> setTextSize(15);
    confirm_points_button -> onMousePress([&]{this->program_state_ = MapState::ConfirmPoints;});

    this->show_path_button=tgui::Button::create();
    show_path_button -> setSize(160,80);
    show_path_button -> setPosition(20,600);
    show_path_button -> setText("Show Path");
    show_path_button -> setTextSize(15);
    show_path_button -> onMousePress([&]{this->program_state_ = MapState::ShowPath;});

    this->show_all_button = tgui::Button::create();
    show_all_button -> setSize(160,80);
    show_all_button -> setPosition(20,700);
    show_all_button -> setText("Show All");
    show_all_button -> setTextSize(15);
    show_all_button -> onMousePress([&]{this->program_state_ = MapState::ShowAll;});

    this->quit_button=tgui::Button::create();
    quit_button -> setSize(160,80);
    quit_button -> setPosition(20,800);
    quit_button -> setText("Quit!");
    quit_button -> setTextSize(15);
    quit_button -> onMousePress([&]{this->program_state_ = MapState::Quit;});
    
    control_pane_background.setPosition(0,0);
    control_pane_background.setSize(sf::Vector2f(control_pane_width_,map_y_));
    control_pane_background.setFillColor(sf::Color::Yellow);

    map_window_gui_.add(message_label);
    map_window_gui_.add(undo_selections_button,"UndoSelections");
    map_window_gui_.add(confirm_points_button,"ConfirmPoints");
    map_window_gui_.add(start_planning_button,"StartPlanning");
    map_window_gui_.add(show_path_button,"ShowPath");
    map_window_gui_.add(show_all_button,"ShowAll");
    map_window_gui_.add(quit_button,"QuitPlan");
}

void BaseMap::drawControlPane()
{
    map_window_.draw(control_pane_background);
    map_window_gui_.draw();
}

void BaseMap::showCurrentState()
{
    unsigned short n  = static_cast<unsigned short>(program_state_);
    std::cout<<"["+map_name_+"]:"+states_[n]<<"\n";
}

void BaseMap::drawPoint(const sf::Vector2f &pt,double radius,const sf::Color &color)
{
    sf::CircleShape point_to_draw;
    point_to_draw.setFillColor(color);
    point_to_draw.setRadius(radius);
    point_to_draw.setOrigin(point_to_draw.getRadius(),point_to_draw.getRadius());
    point_to_draw.setPosition(sf::Vector2f(pt.x,pt.y));
    this->map_window_.draw(point_to_draw);
}

void BaseMap::drawPoint(const Vec2D &pt,double radius,const sf::Color &color)
{
    sf::CircleShape point_to_draw;
    point_to_draw.setFillColor(color);
    point_to_draw.setRadius(radius);
    point_to_draw.setOrigin(point_to_draw.getRadius(),point_to_draw.getRadius());
    point_to_draw.setPosition(sf::Vector2f(pt.x(),pt.y()));
    this->map_window_.draw(point_to_draw);
}

void BaseMap::drawLine(const sf::Vector2f &pt1, const sf::Vector2f &pt2, const sf::Color &color)
{
    sf::Vertex line[2]=
    {
        sf::Vertex(sf::Vector2f(pt1.x,pt1.y),color),
        sf::Vertex(sf::Vector2f(pt2.x,pt2.y),color)
    };
    this->map_window_.draw(line,2,sf::Lines);
}

void BaseMap::drawLine(const Vec2D &pt1, const Vec2D &pt2, const sf::Color &color)
{
    sf::Vertex line[2]=
    {
        sf::Vertex(sf::Vector2f(pt1.x(),pt1.y()),color),
        sf::Vertex(sf::Vector2f(pt2.x(),pt2.y()),color)
    };
    this->map_window_.draw(line,2,sf::Lines);
}

ContinuousMap::ContinuousMap(std::shared_ptr<AppData> my_data):BaseMap(my_data)
{
    num_circle_obs = my_data->circle_obs_array.array.size();
    num_rect_obs = my_data->circle_obs_array.array.size();
    num_total_obs = num_circle_obs + num_rect_obs;
    start_pt_.setCoords(-1,-1);
    start_pt_.cost(-1);
    start_pt_.idx(-1);
    end_pt_.setCoords(-1,-1);
    end_pt_.cost(-1);
    end_pt_.idx(-1);
    point_radius_ = 3.0;
    if (planner_name_ == "RRT")
    {
        continuous_planner_ = std::make_shared<RRT>(data_);
        std::cout<<"RRT planner Initialized!\n";
    }

    else if (planner_name_ == "RRTStar")
    {
        continuous_planner_ = std::make_shared<RRTStar>(data_);
        std::cout<<"RRTStar planner Initialized!\n";
    }

    else if (planner_name_ == "RRTMarch")
    {
        continuous_planner_ = std::make_shared<RRTMarch>(data_);
        std::cout<<"RRTMarch planner Initialized!\n";
    }
    std::cout<<"[ContinuousMap]:Continuous Map Built!\n";
}

ContinuousMap::~ContinuousMap()
{
    map_window_gui_.removeAllWidgets();
    map_window_.close();
}


void ContinuousMap::run()
{
    while (map_window_.isOpen())
    {
        sf::Event event;
        while (map_window_.pollEvent(event))
        {
            map_window_gui_.handleEvent(event);
            if (event.type == sf::Event::Closed)
            {
                this->~ContinuousMap();
            }
            if (event.type == sf::Event::MouseButtonPressed && program_state_==MapState::NotStarted)
            {
                //bool is_point_valid = true;
                sf::Vector2f mouse_position(event.mouseButton.x,event.mouseButton.y);
                if (event.mouseButton.button == sf::Mouse::Left && start_pt_.x()<0 && mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                {
                    //we ensure that the right button has not been clicked before, so trying to click it multiple times will not work
                    if (mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                    start_pt_.setCoords(mouse_position.x,mouse_position.y);
                    start_pt_.idx(0);
                    start_pt_.cost(0);
                    clicks_++;
                }

                else if (event.mouseButton.button == sf::Mouse::Right && end_pt_.x()<0 && mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                {
                    end_pt_.setCoords(mouse_position.x,mouse_position.y);
                    end_pt_.idx(0);
                    end_pt_.cost(0);
                    clicks_++;
                }
                
            }
        }
        if (program_state_ == MapState::NotStarted)
        {
            message_label -> setText("Please select start point (right mouse click) and end point (left mouse click)");
            
            //disabling buttons to prevent evil user inputs
            //the only way the user can progress is to select the start and end points or quit
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);

            //enabling only selected buttons
            undo_selections_button -> setEnabled(true);
            confirm_points_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            if (clicks_<=2)
            {
                if (start_pt_.x()>=0)
                {
                    drawPoint(start_pt_,point_radius_,START_COLOR);
                }
                if (end_pt_.x()>=0)
                {
                    drawPoint(end_pt_,point_radius_,END_COLOR);
                }
            }
            map_window_.display();
        }
        else if (program_state_ == MapState::UndoSelections)
        {
            message_label -> setText("Right click for start point, left click for end point. Once selected, you may confirm your selection or undo your selection");
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            //enabling only selected buttons
            undo_selections_button -> setEnabled(true);
            confirm_points_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            start_pt_.setCoords(-1,-1);
            start_pt_.cost(-1);
            start_pt_.idx(-1);
            end_pt_.setCoords(-1,-1);
            clicks_ = 0;
            program_state_ = MapState::NotStarted;
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            map_window_.display();
        }

        else if (program_state_ == MapState::ConfirmPoints)
        {
            if (start_pt_.x() == -1 || end_pt_.x() == -1)
            {
                message_label -> setText("Please finish selecting start point and end point!");
                program_state_ = MapState::NotStarted;
            }
            else
            {
                message_label -> setText("Start and end points confirmed!. Preparing Planner!");
                //now we can transition into PrepComplete
                data_->setStart(start_pt_);
                data_->setEnd(end_pt_);
                continuous_planner_->updateBasePlannerStartEnd();
                continuous_planner_->updateDerivedPlannerTree();
                std::cout<<"[ContinuousMap]: Start and End points updated in planner\n";
                program_state_ = MapState::PrepComplete;
            }

            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            map_window_.display();
        }

        else if (program_state_ == MapState::PrepComplete)
        {
            message_label -> setText("Planner Prepared!. You may start the planner.");
            //disabling buttons to prevent evil user inputs
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            //enabling only selected buttons
            start_planning_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            map_window_.display();
        }
        
        else if (program_state_ == MapState::StartPlanner)
        {
            message_label -> setText("Planning using " + planner_name_ + " now!");
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            //enabling only selected buttons
            quit_button -> setEnabled(true);

            continuous_planner_->plan();
            
            if (continuous_planner_->returnPlannerState(0) == ContinuousPlannerState::PathFound)
            {
                program_state_ = MapState::PlanningComplete;
                std::cout<<"[ContinuousMap]: Planning Complete\n";
            }
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawNodes();
            drawEdges();
            drawPath(); //this will return if no path is found
            map_window_.display();
            //showCurrentState();
        }
        else if (program_state_ == MapState::PlanningComplete)
        {
            message_label -> setText("A path has been found!");
            
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            //enabling only selected buttons
            show_path_button -> setEnabled(true);
            show_all_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            continuous_planner_->plan();

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawNodes();
            drawEdges();
            drawPath(); //this will return if no path is found
            map_window_.display();
            //showCurrentState();
        }

        else if (program_state_ == MapState::ShowPath)
        {
            message_label -> setText("Highlighting Path!");
            
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            show_path_button -> setEnabled(false);
            //enabling only selected buttons
            show_all_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            continuous_planner_->plan();

            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawPath(); //this will return if no path is found
            // drawPath();
            map_window_.display();
            //showCurrentState();
        }

        else if (program_state_ == MapState::ShowAll)
        {
            message_label -> setText("Showing search space and path!");

            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            show_all_button -> setEnabled(false);
            //enabling only selected buttons
            show_path_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            continuous_planner_->plan();

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawNodes();
            drawEdges();
            drawPath();
            map_window_.display();
            //showCurrentState();
        }

        else if (program_state_ == MapState::Quit)
        {
            message_label -> setText("Ending planner now. Goodbye!");
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            sf::sleep(sf::seconds(1));
            map_window_.display();
            break;
        }
    }
    
}

void ContinuousMap::drawStartEnd()
{
    drawPoint(this->start_pt_,this->point_radius_,START_COLOR);
    drawPoint(this->end_pt_,this->point_radius_,END_COLOR);
}

void ContinuousMap::drawObstacles()
{
    for (auto& rect_obs : data_->rect_obs_array.array)
    {
        map_window_.draw(rect_obs.shape);
    }

    for (auto& circle_obs : data_->circle_obs_array.array)
    {
        map_window_.draw(circle_obs.shape);
    }
}

void ContinuousMap::drawNodes()
{
    for (auto &node : continuous_planner_->returnTree())
    {
        if (node.idx() == 0){continue;}
        if (node.idx() == continuous_planner_->returnTreeSize()-1){continue;}
        drawPoint(node, point_radius_ , POINT_COLOR);
    }
}

void ContinuousMap::drawEdges()
{
    std::vector<Vec2D> tree_copy = continuous_planner_->returnTree();
    std::vector<Vec2D> parent_copy = continuous_planner_->returnParent();
    unsigned long int current_tree_size = continuous_planner_->returnTreeSize();
    for (unsigned long int i=1 ; i<current_tree_size ; ++i)
    {
        drawLine(tree_copy[i],parent_copy[i],LINE_COLOR);
    }
}
void ContinuousMap::drawPath()
{
    std::vector<Vec2D> path_copy = continuous_planner_->returnPath();
    if (path_copy.empty())
    {
        return;
    }
    for (unsigned long int i = 0 ; i<path_copy.size()-1 ; ++i)
    {
        drawLine(path_copy[i],path_copy[i+1],PATH_COLOR);
    }
}

/// Discrete Map Implementation

DiscreteMap::DiscreteMap(std::shared_ptr<AppData> my_data) : BaseMap(my_data)
{
    num_obs = my_data->rect_obs_array.array.size();
    resolution_ = my_data->getResolution();
    num_cols_ = my_data->getMapX() / resolution_.first;
    num_rows_ = my_data->getMapY() / resolution_.second;
    possible_points_ = num_cols_ * num_rows_;
    grid_state_.reserve(possible_points_);
    for (auto &obs : my_data->rect_obs_array.array)
    {
        sf::Vector2f pos = obs.getPosition();
        Node node = GlobalToGrid(pos);
        int flat = flattenGrid(node);
        grid_state_[flat] = 1;
    }
    resolution_v2f_ = {(float)resolution_.first , (float)resolution_.second};
    Vec2D start_global_coords(-1,-1);
    Vec2D end_global_coords(-1,-1);
    start_node_.setGlobalCoords(start_global_coords);
    start_node_.setGridCoords(-1,-1);
    end_node_.setGlobalCoords(end_global_coords);
    end_node_.setGridCoords(-1,-1);
    chosen_discrete_planner = planner_name_;
    // if (planner_name_ == "BFS")
    // {
    //     discrete_planner_ = std::make_shared<BFS>(data_);
    // }
}

DiscreteMap::~DiscreteMap()
{
    map_window_gui_.removeAllWidgets();
    map_window_.close();
}

void DiscreteMap::run()
{
    while (map_window_.isOpen())
    {
        sf::Event event;
        while (map_window_.pollEvent(event))
        {
            map_window_gui_.handleEvent(event);
            if (event.type == sf::Event::Closed)
            {
                this->~DiscreteMap();
            }
            if (event.type == sf::Event::MouseButtonPressed && program_state_==MapState::NotStarted)
            {
                sf::Vector2f mouse_position(event.mouseButton.x,event.mouseButton.y);
                bool mouse_in_range = mouse_position.x >= x_lower_ && mouse_position.x < x_upper_ && mouse_position.y >= y_lower_ && mouse_position.y < y_upper_;
                if (mouse_in_range)
                {
                    if (event.mouseButton.button == sf::Mouse::Left && start_node_.x()<0)
                    {
                        start_node_ = GlobalToGrid(mouse_position);
                        Vec2D global = GridToGlobal(start_node_);
                        start_node_.setGlobalCoords(global);
                        start_node_.cost(0);
                        int start_flat = flattenGrid(start_node_);
                        if (!grid_state_.at(start_flat))
                        {
                            grid_state_[start_flat] = 1; //occupied
                            clicks_++;
                        }
                        else
                        {
                            std::cout<<"[DiscreteMap]: Choose Unoccupied Cell!\n";
                            start_node_.clear();
                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right && end_node_.x()<0)
                    {
                        end_node_ = GlobalToGrid(mouse_position);
                        Vec2D global = GridToGlobal(end_node_);
                        end_node_.setGlobalCoords(global);
                        end_node_.cost(0);
                        int end_flat = flattenGrid(end_node_);
                        if (!grid_state_.at(end_flat))
                        {
                            grid_state_[end_flat] = 1; //occupied
                            clicks_++;
                        }
                        else
                        {
                            std::cout<<"[DiscreteMap]: Choose Unoccupied Cell!\n";
                            end_node_.clear();
                        }
                    }
                }
            }
        } // End of callback processing

        if (program_state_ == MapState::NotStarted)
        {
            message_label -> setText("Please select start point (right mouse click) and end point (left mouse click)");
            
            //disabling buttons to prevent evil user inputs
            //the only way the user can progress is to select the start and end points or quit
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);

            //enabling only selected buttons
            undo_selections_button -> setEnabled(true);
            confirm_points_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawGrids();

            if (clicks_ <= 2)
            {
                if (start_node_.x()>=0)
                {
                    drawRect(start_node_ , resolution_v2f_ , START_COLOR);
                }

                if (end_node_.x() >=0)
                {
                    drawRect(end_node_ , resolution_v2f_ , END_COLOR);
                }
            }
            map_window_.display();
        }

        else if (program_state_ == MapState::UndoSelections)
        {
            message_label -> setText("Right click for start point, left click for end point. Once selected, you may confirm your selection or undo your selection");
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            //enabling only selected buttons
            undo_selections_button -> setEnabled(true);
            confirm_points_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            //actions at this state
            int start_flat = flattenGrid(start_node_);
            int end_flat = flattenGrid(end_node_);
            grid_state_[start_flat] = 0;
            grid_state_[end_flat] = 0;
            start_node_.clear();
            end_node_.clear();
            clicks_ = 0;
            program_state_ = MapState::NotStarted;

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawGrids();
            map_window_.display();
        }

        else if (program_state_ == MapState::ConfirmPoints)
        {
            if (start_node_.x() == -1 || end_node_.x() == -1)
            {
                message_label -> setText("Please finish selecting start and end points!");
                program_state_ = MapState::NotStarted;
            }
            else
            {
                message_label -> setText("Start and end nodes confirmed!. Preparing Planner!");
                data_->setStartNode(start_node_);
                data_->setEndNode(end_node_);
                // discrete_planner_->updateBasePlannerStartEndNode();
                // discrete_planner_->updateDerivedPlannerTree(grid_state_);
                std::cout<<"[DiscreteMap]: Start and End points updated in planner\n";
                program_state_ = MapState::PrepComplete;
            }
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawGrids();
            map_window_.display();
        }

        else if (program_state_ == MapState::PrepComplete)
        {
            message_label -> setText("Planner Prepared!. You may start the planner.");
            //disabling buttons to prevent evil user inputs
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            confirm_points_button-> setEnabled(false);
            //enabling only selected buttons
            start_planning_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawGrids();
            map_window_.display();
        }

        else if (program_state_ == MapState::Quit)
        {
            message_label -> setText("Ending planner now. Goodbye!");
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            sf::sleep(sf::seconds(1));
            map_window_.display();
            break;
        }
    }
}

void DiscreteMap::drawGrids()
{
    for (int i=x_lower_ + resolution_.first; i < x_upper_ ; i += resolution_.first)
    {
        sf::Vector2f pt1(i,y_lower_);
        sf::Vector2f pt2(i,y_upper_-1);
        sf::Vertex vertical_line[]=
        {
            sf::Vertex(pt1),
            sf::Vertex(pt2)
        };
        this->map_window_.draw(vertical_line,2,sf::Lines);
    }
    for (int i=y_lower_+resolution_.second; i<y_upper_ ; i+=resolution_.second)
    {
        sf::Vector2f pt1(x_lower_,i);
        sf::Vector2f pt2(x_upper_-1,i);
        sf::Vertex horizontal_line[]=
        {
            sf::Vertex(pt1),
            sf::Vertex(pt2)
        };
        this->map_window_.draw(horizontal_line,2,sf::Lines);
    }
}

void DiscreteMap::drawRect(Node &selected_node , const sf::Vector2f &size , const sf::Color& color)
{
    //given some centre point, we need to draw the rectangle
    sf::RectangleShape rect;
    rect.setOrigin(size.x / 2 , size.y / 2);
    Vec2D position_v2d = selected_node.getGlobalCoords();
    sf::Vector2f position = utils::Vec2DToV2f(position_v2d);
    rect.setPosition(position);
    rect.setFillColor(color);
    rect.setSize(size);
    map_window_.draw(rect);
}

void DiscreteMap::drawStartEnd()
{
    drawRect(start_node_ , resolution_v2f_ , START_COLOR);
    drawRect(end_node_ , resolution_v2f_ , END_COLOR);
}

void DiscreteMap::drawObstacles()
{
    for (auto& rect_obs : data_->rect_obs_array.array)
    {
        map_window_.draw(rect_obs.shape);
    }
}

Node DiscreteMap::GlobalToGrid(Vec2D &vec)
{
    Node grid_node;
    int x = (vec.x() - x_lower_) / resolution_.first;
    int y = (vec.y() - y_lower_) / resolution_.second;

    x = x == (x_upper_ / resolution_.first) ?  x - 1 : x;
    y = y == (y_upper_ / resolution_.second) ? y - 1 : y;

    grid_node.setGridCoords(x,y);
    return grid_node;
}

Node DiscreteMap::GlobalToGrid(sf::Vector2f &vec)
{
    Node grid_node;
    int x = (vec.x - x_lower_) / resolution_.first;
    int y = (vec.y - y_lower_) / resolution_.second;

    x = x == (x_upper_ / resolution_.first) ?  x - 1 : x;
    y = y == (y_upper_ / resolution_.second) ? y - 1 : y;

    grid_node.setGridCoords(x,y);
    return grid_node;
}

Vec2D DiscreteMap::GridToGlobal(Node &node)
{
    Vec2D global_coords; //origin
    global_coords.x(x_lower_ + (node.x() * resolution_.first) + (resolution_.first / 2));
    global_coords.y(y_lower_ + (node.y() * resolution_.second) + (resolution_.second / 2));
    return global_coords;
}

int DiscreteMap::flattenGrid(Node &node)
{
    int flattened = (num_cols_ * node.y()) + node.x();
    return flattened;
}
//to do: drawNodes, drawEdges , drawPath