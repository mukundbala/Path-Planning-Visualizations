#include <map.hpp>

BaseMap::BaseMap(std::shared_ptr<AppData>  my_data) : data_{my_data}
{
    map_x_ = data_->getMapX();
    map_y_ = data_->getMapY();
    control_pane_width_ = data_->getControlPaneWidth();
    window_x_ = map_x_ + control_pane_width_;
    window_y_ = map_y_;
    planner_ = data_->getChosenPlanner();

    map_window_.create(sf::VideoMode(window_x_,window_y_,32),planner_);
    map_window_gui_.setTarget(map_window_);
    
    program_state_ = State::NotStarted;
    prep_complete_ = false;
    plan_complete_ = false;

    LINE_COLOR = sf::Color::White;
    POINT_COLOR = sf::Color::Cyan;
    WINDOW_COLOR = sf::Color::Black;
    START_COLOR = sf::Color::Green;
    END_COLOR = sf::Color::Magenta;
    PATH_COLOR = sf::Color::Yellow;

    std::cout<<"[ContinuousMap]:Base Map Built!\n";
}


BaseMap::~BaseMap()
{
    data_.reset();
}

void BaseMap::drawBaseScreen()
{
    this->map_window_gui_.removeAllWidgets();
    this->select_start_button=tgui::Button::create();
    select_start_button -> setSize(160,80);
    select_start_button -> setPosition(20,100);
    select_start_button -> setText("Select Start Point!");
    select_start_button -> setTextSize(15);
    select_start_button -> onMousePress([&]{program_state_ = State::SelectStart;});

    this->select_end_button=tgui::Button::create();
    select_end_button -> setSize(160,80);
    select_end_button -> setPosition(20,200);
    select_end_button -> setText("Select End Point!");
    select_end_button -> setTextSize(15);
    select_end_button -> onMousePress([&]{this->program_state_ = State::SelectEnd;});

    this->undo_selections_button=tgui::Button::create();
    undo_selections_button -> setSize(160,80);
    undo_selections_button -> setPosition(20,300);
    undo_selections_button -> setText("Undo Selections!");
    undo_selections_button -> setTextSize(15);
    undo_selections_button -> onMousePress([&]{this->program_state_ = State::UndoSelections;});

    this->start_plan_button=tgui::Button::create();
    start_plan_button -> setSize(160,80);
    start_plan_button -> setPosition(20,400);
    start_plan_button -> setText("Start Planning!");
    start_plan_button -> setTextSize(15);
    start_plan_button -> onMousePress([&]{this->program_state_ = State::StartPlan;});

    this->show_path_button=tgui::Button::create();
    show_path_button -> setSize(160,80);
    show_path_button -> setPosition(20,500);
    show_path_button -> setText("Start Planning!");
    show_path_button -> setTextSize(15);
    show_path_button -> onMousePress([&]{this->program_state_ = State::StartPlan;});

    this->quit_button=tgui::Button::create();
    quit_button -> setSize(160,80);
    quit_button -> setPosition(20,600);
    quit_button -> setText("Quit!");
    quit_button -> setTextSize(15);
    quit_button -> onMousePress([&]{this->program_state_ = State::End;});

    sf::RectangleShape control_pane_background;
    control_pane_background.setPosition(0,0);
    control_pane_background.setSize(sf::Vector2f(control_pane_width_,map_y_));
    control_pane_background.setFillColor(sf::Color::Yellow);
    
    map_window_gui_.add(select_start_button,"SelectStart");
    map_window_gui_.add(select_end_button,"SelectEnd");
    map_window_gui_.add(undo_selections_button,"UndoSelections");
    map_window_gui_.add(start_plan_button,"StartPlan");
    map_window_gui_.add(show_path_button,"ShowPath");
    map_window_gui_.add(quit_button,"QuitPlan");

    map_window_.draw(control_pane_background);
    map_window_gui_.draw();
    
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

            if (!prep_complete_ && event.type == sf::Event::MouseButtonPressed)
            {
                bool valid_pos = true;
                Vec2D mouse_pos(event.mouseButton.x,event.mouseButton.y,-1,-1);
                if (mouse_pos.x()<control_pane_width_ || mouse_pos.x()>=map_x_ || mouse_pos.y()<0 || mouse_pos.y()>=map_y_)
                {
                    valid_pos = false;
                }
                if (valid_pos)
                {
                    if (program_state_ == State::SelectStart)
                    {
                        start_pt_.setCoords(mouse_pos.x(),mouse_pos.y());
                        start_pt_.cost(0);
                        start_pt_.idx(0);
                        std::cout<<"[Map]: Start point set!\n";
                    }
                    else if (program_state_ == State::SelectEnd)
                    {
                        end_pt_.setCoords(mouse_pos.x(),mouse_pos.y());
                        std::cout<<"[Map]: End point set!\n";
                    }
                }

                if (start_pt_.x()>=0 && end_pt_.x()>=0)
                {
                    prep_complete_ = true;
                }
            }
        }
        if (program_state_ == State::UndoSelections)
        {
            start_pt_.setCoords(-1,-1);
            start_pt_.cost(-1);
            start_pt_.idx(-1);
            end_pt_.setCoords(-1,-1);
            prep_complete_ = false;
            std::cout<<"[Map]:Points have been reset!\n";
        }
        drawBaseScreen();
        drawObstacles();
        drawStartEnd();
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
    std::cout<<start_pt_.x()<<" "<<start_pt_.y()<<"\n";
    std::cout<<end_pt_.x()<<" "<<end_pt_.y()<<"\n";
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