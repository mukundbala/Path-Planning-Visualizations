#include <map.hpp>

BaseMap::BaseMap(std::shared_ptr<AppData>  my_data) : data_{my_data}
{
    map_x_ = data_->getMapX();
    map_y_ = data_->getMapY();
    control_pane_width_ = data_->getControlPaneWidth();
    window_x_ = map_x_ + control_pane_width_;
    window_y_ = map_y_;
    planner_name_ = data_->getChosenPlanner();
    map_name_ = data_->getChosenMap();
    toupper(map_name_[0]);
    dark_theme_.setDefault("themes/Black.txt");
    map_window_.create(sf::VideoMode(window_x_,window_y_,32),planner_name_);
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

    std::cout<<"[" + map_name_+"]:Base Map Built!\n";
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
    message_label->setSize(180,180);
    message_label->setPosition(0,0);
    message_label->setTextSize(20);
    
    this->start_planning_button=tgui::Button::create();
    start_planning_button -> setSize(160,80);
    start_planning_button -> setPosition(20,200);
    start_planning_button -> setText("StartPlanner");
    start_planning_button -> setTextSize(15);
    start_planning_button -> onMousePress([&]{this->program_state_ = MapState::StartPlanner;});
    
    this->undo_selections_button=tgui::Button::create();
    undo_selections_button -> setSize(160,80);
    undo_selections_button -> setPosition(20,300);
    undo_selections_button -> setText("Undo Selections");
    undo_selections_button -> setTextSize(15);
    undo_selections_button -> onMousePress([&]{this->program_state_ = MapState::UndoSelections;});

    this->show_path_button=tgui::Button::create();
    show_path_button -> setSize(160,80);
    show_path_button -> setPosition(20,400);
    show_path_button -> setText("Show Path");
    show_path_button -> setTextSize(15);
    show_path_button -> onMousePress([&]{this->program_state_ = MapState::ShowPath;});

    this->show_all_button = tgui::Button::create();
    show_all_button -> setSize(160,80);
    show_all_button -> setPosition(20,500);
    show_all_button -> setText("Show All");
    show_all_button -> setTextSize(15);
    show_all_button -> onMousePress([&]{this->program_state_ = MapState::ShowAll;});

    this->quit_button=tgui::Button::create();
    quit_button -> setSize(160,80);
    quit_button -> setPosition(20,600);
    quit_button -> setText("Quit!");
    quit_button -> setTextSize(15);
    quit_button -> onMousePress([&]{this->program_state_ = MapState::Quit;});
    
    control_pane_background.setPosition(0,0);
    control_pane_background.setSize(sf::Vector2f(control_pane_width_,map_y_));
    control_pane_background.setFillColor(sf::Color::Yellow);

    map_window_gui_.add(message_label);
    map_window_gui_.add(undo_selections_button,"UndoSelections");
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
                if (event.mouseButton.button == sf::Mouse::Right && start_pt_.x()<0 && mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                {
                    //we ensure that the right button has not been clicked before, so trying to click it multiple times will not work
                    if (mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                    start_pt_.setCoords(mouse_position.x,mouse_position.y);
                    start_pt_.idx(0);
                    start_pt_.cost(0);
                    clicks_++;
                }

                else if (event.mouseButton.button == sf::Mouse::Left && end_pt_.x()<0 && mouse_position.x >= control_pane_width_ && mouse_position.x < map_x_+control_pane_width_ && mouse_position.y>=0 && mouse_position.y<map_y_)
                {
                    end_pt_.setCoords(mouse_position.x,mouse_position.y);
                    end_pt_.idx(0);
                    end_pt_.cost(0);
                    clicks_++;
                }
                
                if (clicks_ == 2 && program_state_ == MapState::NotStarted)
                {
                    //state transition
                    program_state_ = MapState::PrepComplete;
                }
            }
        }
        if (program_state_ == MapState::NotStarted)
        {
            message_label -> setText("Please select start point (right mouse click) and end point (left mouse click)");
            
            //disabling buttons to prevent evil user inputs
            //the only way the user can progress is to select the start and end points or quit
            undo_selections_button -> setEnabled(false);
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);

            //enabling only selected buttons
            quit_button -> setEnabled(true);

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            if (clicks_<2)
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
        }
        else if (program_state_ == MapState::UndoSelections)
        {
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            //enabling only selected buttons
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

            //showCurrentState();
        }

        else if (program_state_ == MapState::PrepComplete)
        {
            message_label -> setText("Start and end points selected. You may start the planner, or Undo Selections to reset these points");
            
            //disabling buttons to prevent evil user inputs
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);

            //enabling only selected buttons
            undo_selections_button -> setEnabled(true);
            start_planning_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();

            //showCurrentState();
        }
        
        else if (program_state_ == MapState::StartPlanner)
        {
            message_label -> setText("Planning using " + planner_name_ + " now!");

            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            //enabling only selected buttons
            quit_button -> setEnabled(true);

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawEdges();
            drawNodes();

            //showCurrentState();
        }

        else if (program_state_ == MapState::PlanningComplete)
        {
            message_label -> setText("A path has been found!");
            
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            //enabling only selected buttons
            show_path_button -> setEnabled(true);
            show_all_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawEdges();
            drawNodes();
            drawPath();

            //showCurrentState();
        }

        else if (program_state_ == MapState::ShowPath)
        {
            message_label -> setText("Highlighting Path!");
            
            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            show_path_button -> setEnabled(false);
            //enabling only selected buttons
            show_all_button -> setEnabled(true);
            quit_button -> setEnabled(true);
            
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawPath();

            //showCurrentState();
        }

        else if (program_state_ == MapState::ShowAll)
        {
            message_label -> setText("Showing search space and path!");

            //disabling buttons to prevent evil user inputs
            start_planning_button -> setEnabled(false);
            undo_selections_button -> setEnabled(false);
            show_all_button -> setEnabled(false);
            //enabling only selected buttons
            show_path_button -> setEnabled(true);
            quit_button -> setEnabled(true);

            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();
            drawStartEnd();
            drawEdges();
            drawNodes();
            drawPath();

            //showCurrentState();
        }

        else if (program_state_ == MapState::Quit)
        {
            message_label -> setText("Ending planner now. Goodbye!");
            
            //drawing our frame
            map_window_.clear();
            drawControlPane();
            drawObstacles();

            //showCurrentState();
            //do stuff here to release resources
        }
        map_window_.display();
    }
}

void ContinuousMap::drawPoint(const sf::Vector2f &pt,double radius,const sf::Color &color)
{
    sf::CircleShape point_to_draw;
    point_to_draw.setFillColor(color);
    point_to_draw.setRadius(radius);
    point_to_draw.setOrigin(point_to_draw.getRadius(),point_to_draw.getRadius());
    point_to_draw.setPosition(sf::Vector2f(pt.x,pt.y));
    this->map_window_.draw(point_to_draw);
}

void ContinuousMap::drawPoint(const Vec2D &pt,double radius,const sf::Color &color)
{
    sf::CircleShape point_to_draw;
    point_to_draw.setFillColor(color);
    point_to_draw.setRadius(radius);
    point_to_draw.setOrigin(point_to_draw.getRadius(),point_to_draw.getRadius());
    point_to_draw.setPosition(sf::Vector2f(pt.x(),pt.y()));
    this->map_window_.draw(point_to_draw);
}

void ContinuousMap::drawLine(const sf::Vector2f &pt1, const sf::Vector2f &pt2, const sf::Color &color)
{
    sf::Vertex line[2]=
    {
        sf::Vertex(sf::Vector2f(pt1.x,pt1.y),color),
        sf::Vertex(sf::Vector2f(pt2.x,pt2.y),color)
    };
    this->map_window_.draw(line,2,sf::Lines);
}

void ContinuousMap::drawLine(const Vec2D &pt1, const Vec2D &pt2, const sf::Color &color)
{
    sf::Vertex line[2]=
    {
        sf::Vertex(sf::Vector2f(pt1.x(),pt1.y()),color),
        sf::Vertex(sf::Vector2f(pt2.x(),pt2.y()),color)
    };
    this->map_window_.draw(line,2,sf::Lines);
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

void ContinuousMap::drawEdges()
{

}

void ContinuousMap::drawNodes()
{

}

void ContinuousMap::drawPath()
{

}