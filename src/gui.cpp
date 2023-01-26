#include <gui.hpp>

PlannerGUI::PlannerGUI(std::shared_ptr<AppData> data)
{   
    app_data_ = data; //accessing data app data
    
    //setting all necessary app data
    gui_window_chooser_size_.x = app_data_->getGUIWindowX();
    gui_window_chooser_size_.y = app_data_->getGUIWindowY();
    gui_window_selector_size_.x = app_data_->getControlPaneWidth() + app_data_->getMapX();
    gui_window_selector_size_.y = app_data_->getMapY();
    resolution_ = app_data_->getResolution();
    EPS=app_data_->getEPS();
    
    background_color_ = sf::Color::Red;
    dark_theme_.setDefault("themes/Black.txt");
    
    chooser_state_ = ChooserState::HomeScreen;
    selector_state_ = SelectorState::NotStarted;
    default_circobs_radius_ = 10;
    default_rectobs_size_ = {(float)default_circobs_radius_*2,(float)default_circobs_radius_*2};
    run_state_ = true;

    //creating the Buttons and Labels
    label_title = tgui::Label::create();
    label_title->setText("Path Planning Visualizations");
    label_title->setTextSize(50);
    label_title->setPosition(50,100);

    go_button = tgui::Button::create();
    go_button->setPosition(350,460);
    go_button->setSize(150,120);
    go_button->setText("GO!");
    go_button->setTextSize(0);
    go_button->onMousePress([&]{chooser_state_ = ChooserState::ChooseMapType;});

    exit_button = tgui::Button::create();
    exit_button->setPosition(700,500);
    exit_button->setSize(80,80);
    exit_button->setText("Exit");
    exit_button->setTextSize(0);
    exit_button->onMousePress([&]{chooser_state_ = ChooserState::Quit;});
    
    canvas = tgui::Canvas::create({200, 200});
    img_texture.loadFromFile("themes/homescreen.png");
    img_sprite.setTexture(img_texture);
    img_sprite.setScale(200.f / img_texture.getSize().x, 200.f / img_texture.getSize().y);
    canvas->setPosition(320, 200);

    label_description = tgui::Label::create();
    label_description->setText("Choose a Map Type!");
    label_description->setTextSize(50);
    label_description->setPosition(150,100);

    continuous_map_button = tgui::Button::create();
    continuous_map_button->setPosition(160,300);
    continuous_map_button->setSize(180,120);
    continuous_map_button->setText("Continuous\n      Map");
    continuous_map_button->setTextSize(0);
    continuous_map_button->onMousePress([&]{
                                            chooser_state_ = ChooserState::ContinuousMapSelector;
                                            app_data_->setChosenMap("continuous");});
    
    discrete_map_button = tgui::Button::create();
    discrete_map_button->setPosition(480,300);
    discrete_map_button->setSize(180,120);
    discrete_map_button->setText("Discrete\n    Map");
    discrete_map_button->setTextSize(28);
    discrete_map_button->onMousePress([&]{
                                            chooser_state_ = ChooserState::DiscreteMapSelector;
                                            app_data_->setChosenMap("discrete");});
    
    continuous_planners_label = tgui::Label::create();
    continuous_planners_label->setPosition(140,50);
    continuous_planners_label->setText("Choose a Continuous Planner!");
    continuous_planners_label->setTextSize(40);

    discrete_planners_label = tgui::Label::create();
    discrete_planners_label->setPosition(140,50);
    discrete_planners_label->setText("Choose a Discrete Planner!");
    discrete_planners_label->setTextSize(40);

    const sf::Vector2u button_size = {120,100};
    const int initial_x = 100;
    const int initial_y = 200;
    const int spacing_x = button_size.x + 20;
    const int spacing_y = button_size.y + 20;
    const int rowcol = 3;
    
    for (size_t i = 0; i < app_data_->continuous_planners.size(); ++i)
    {
        auto button = tgui::Button::create();
        button->setSize(button_size.x,button_size.y);
        button->setPosition(initial_x + spacing_x * (i / rowcol),initial_y + spacing_y * (i % rowcol));
        button->setText(app_data_->continuous_planners[i]);
        button->setTextSize(15);
        button->onMousePress(&PlannerGUI::ChoosePlannerCallback,this,app_data_->continuous_planners[i]);
        continuous_map_buttons.emplace_back(button);
    }
    for (size_t i = 0; i < app_data_->discrete_planners.size(); ++i)
    {
        auto button = tgui::Button::create();
        button->setSize(button_size.x,button_size.y);
        button->setPosition(initial_x + spacing_x * (i / rowcol),initial_y + spacing_y * (i % rowcol));
        button->setText(app_data_->discrete_planners[i]);
        button->setTextSize(15);
        button->onMousePress(&PlannerGUI::ChoosePlannerCallback,this,app_data_->discrete_planners[i]);
        discrete_map_buttons.emplace_back(button);
    }
    //control pane stuff
    circle_obs_button = tgui::Button::create();
    circle_obs_button->setSize(180,60);
    circle_obs_button->setPosition(10,60);
    circle_obs_button->setText("Draw Circle");
    circle_obs_button->setTextSize(15);
    circle_obs_button->onMousePress([&]{selector_state_=SelectorState::DrawCircle;});

    rectangle_obs_button = tgui::Button::create();
    rectangle_obs_button->setSize(180,60);
    rectangle_obs_button->setPosition(10,140);
    rectangle_obs_button->setText("Draw Rectangle");
    rectangle_obs_button->setTextSize(15);
    rectangle_obs_button->onMousePress([&]{selector_state_=SelectorState::DrawRectangle;});
    
    reset_last_circle_obs_button = tgui::Button::create();
    reset_last_circle_obs_button->setSize(180,60);
    reset_last_circle_obs_button->setPosition(10,220);
    reset_last_circle_obs_button->setText("Reset Last Circle");
    reset_last_circle_obs_button->setTextSize(15);
    reset_last_circle_obs_button->onMousePress([&]{if(selector_state_ == SelectorState::NotStarted)
                                            {return;}
                                                selector_state_=SelectorState::ResetLastCircle;});
    reset_last_rectangle_obs_button = tgui::Button::create();
    reset_last_rectangle_obs_button->setSize(180,60);
    reset_last_rectangle_obs_button->setPosition(10,300);
    reset_last_rectangle_obs_button->setText("Reset Last Rectangle");
    reset_last_rectangle_obs_button->setTextSize(15);
    reset_last_rectangle_obs_button->onMousePress([&]{if(selector_state_ == SelectorState::NotStarted)
                                            {return;}
                                            selector_state_ = SelectorState::ResetLastRectangle;});
    
    reset_all_obstacle_button = tgui::Button::create();
    reset_all_obstacle_button->setSize(180,60);
    reset_all_obstacle_button->setPosition(10,380);
    reset_all_obstacle_button->setText("Reset All");
    reset_all_obstacle_button->setTextSize(15);
    reset_all_obstacle_button->onMousePress([&]{if(selector_state_ == SelectorState::NotStarted)
                                            {return;}
                                            selector_state_=SelectorState::ResetAll;});
    
    random_obstacle_button = tgui::Button::create();
    random_obstacle_button->setSize(180,60);
    random_obstacle_button->setPosition(10,460);
    random_obstacle_button->setText("Random Obstacle");
    random_obstacle_button->setTextSize(15);
    random_obstacle_button->onMousePress([&]{selector_state_=SelectorState::Random;});

    done_button = tgui::Button::create();
    done_button->setSize(180,60);
    done_button->setPosition(10,540);
    done_button->setText("Done");
    done_button->setTextSize(15);
    done_button->onMousePress([&]{selector_state_=SelectorState::Done;});

    save_map_button = tgui::Button::create();
    save_map_button->setSize(180,60);
    save_map_button->setPosition(10,620);
    save_map_button->setText("Save Map");
    save_map_button->setTextSize(15);
    save_map_button->onMousePress([&]{selector_state_=SelectorState::SaveMap;});

    load_map_button = tgui::Button::create();
    load_map_button->setSize(180,60);
    load_map_button->setPosition(10,700);
    load_map_button->setText("Load Map");
    load_map_button->setTextSize(15);
    load_map_button->onMousePress([&]{selector_state_=SelectorState::LoadMap;});

    quit_button = tgui::Button::create();
    quit_button->setSize(180,60);
    quit_button->setPosition(10,780);
    quit_button->setText("Quit");
    quit_button->setTextSize(15);
    quit_button->onMousePress([&]{selector_state_=SelectorState::Quit;});
    
    std::cout << "[PlannerGUI]: Planner GUI initialized!\n";
}

PlannerGUI::~PlannerGUI() noexcept
{

    if (gui_window_chooser_.isOpen())
    {
        gui_window_chooser_.close();
    }
    if (gui_window_selector_.isOpen())
    {
        gui_window_selector_.close();
    }
    app_data_.reset();
    std::cout<<"[PlannerGUI]: Closing GUI now\n";
}

bool PlannerGUI::runGui()
{
    runMapAndPlannerChooser();
    runObstacleSelector();
    return run_state_;      
}

void PlannerGUI::DrawHomeScreen()
{
    gui_backend_chooser_.removeAllWidgets();
    //adding stuff
    gui_backend_chooser_.add(label_title);
    gui_backend_chooser_.add(go_button);
    gui_backend_chooser_.add(exit_button);
    gui_backend_chooser_.add(canvas);
    canvas->clear();
    canvas->draw(img_sprite);
    canvas->display();
    go_button->setEnabled(1);
    exit_button->setEnabled(1);
}

void PlannerGUI::DrawMapTypeScreen()
{
    gui_backend_chooser_.removeAllWidgets();
    gui_backend_chooser_.add(label_description);
    gui_backend_chooser_.add(continuous_map_button);
    gui_backend_chooser_.add(discrete_map_button);
    continuous_map_button->setEnabled(1);
    discrete_map_button->setEnabled(1);
}

void PlannerGUI::DrawContinuousMapSelection()
{
    gui_backend_chooser_.removeAllWidgets();
    gui_backend_chooser_.add(continuous_planners_label);
    for (const auto& button : continuous_map_buttons)
    {
        gui_backend_chooser_.add(button);
        button->setEnabled(1);
    }
}

void PlannerGUI::DrawDiscreteMapSelection()
{
    gui_backend_chooser_.removeAllWidgets();
    gui_backend_chooser_.add(discrete_planners_label);
    for (const auto& button : discrete_map_buttons)
    {
        gui_backend_chooser_.add(button);
        button->setEnabled(1);
    }
}

void PlannerGUI::ChoosePlannerCallback(std::string planner_name)
{
    app_data_->setChosenPlanner(planner_name);
    chooser_state_ = ChooserState::Done;
}

void PlannerGUI::runMapAndPlannerChooser()
{
    gui_window_chooser_.create(sf::VideoMode(gui_window_chooser_size_.x,gui_window_chooser_size_.y,32),"Planner GUI");
    gui_backend_chooser_.setTarget(gui_window_chooser_);
    while (gui_window_chooser_.isOpen())
    {
        sf::Event event;
        while (gui_window_chooser_.pollEvent(event))
        {
            gui_backend_chooser_.handleEvent(event);
            if (event.type == sf::Event::Closed)
            {
                gui_window_chooser_.close();
                run_state_= false;
            }
        }
        //gui_window_.clear(sf::Color::Red);
        if (chooser_state_ == ChooserState::HomeScreen)
        {
            DrawHomeScreen();
        }
        else if (chooser_state_ == ChooserState::ChooseMapType)
        {
            DrawMapTypeScreen();
        }
        else if (chooser_state_ == ChooserState::ContinuousMapSelector)
        {
            DrawContinuousMapSelection();
        }
        else if (chooser_state_ == ChooserState::DiscreteMapSelector)
        {
            DrawDiscreteMapSelection();
        }
        else if (chooser_state_ == ChooserState::Done)
        {
            std::cout<<"[Planner GUI]: Planner and Map Chosen. Prepare to set Obstacles\n";
            gui_window_chooser_.close();
            return;
        }
        else if (chooser_state_ == ChooserState::Quit)
        {
            gui_window_chooser_.close();
            run_state_ = false;
            return;
        }
        gui_backend_chooser_.draw();
        gui_window_chooser_.display();
        gui_window_chooser_.clear(sf::Color::Red);
    }
}
void PlannerGUI::runObstacleSelector()
{
    if (!run_state_)
    {
        return;
    }
    gui_window_selector_.create(sf::VideoMode(gui_window_selector_size_.x,gui_window_selector_size_.y,32),"Selector GUI");
    gui_backend_selector_.setTarget(gui_window_selector_);
    
    sf::Clock selector_clock; // check how long the mouse has be held
    bool pos_mouse = 0;
    sf::Int32 pos_mouse_time = 0;
    sf::Int32 neg_mouse_time = 0;
    sf::Vector2f pos_mouse_pos = {-1,-1};
    sf::Vector2f neg_mouse_pos = {-1,-1};

    const int x_lower = app_data_->getControlPaneWidth();
    const int x_higher = (int)gui_window_selector_.getSize().x;
    const int y_lower = 0;
    const int y_higher = (int)gui_window_selector_.getSize().y;

    //handle duplicate checking for discrete points faster (in O(1)) using boolean flat array 
    int num_cols = (app_data_->getMapX() / resolution_.first);
    std::vector<int> discrete_point_tester((app_data_->getMapX() / resolution_.first) * (app_data_->getMapY() / resolution_.second),0); 
    std::vector<Vec2D> points_selected;
    while (gui_window_selector_.isOpen())
    {
        sf::Event event;
        while (gui_window_selector_.pollEvent(event))
        {
            gui_backend_selector_.handleEvent(event);
            if (event.type == sf::Event::Closed)
            {
                gui_window_selector_.close();
                run_state_ = false;
                break; //break from the callback poll, and break out of the program loop
            }

            if (app_data_->getChosenMap()=="continuous")
            {
                if (event.type == sf::Event::MouseButtonPressed && !pos_mouse) 
                {
                    if ((selector_state_ == SelectorState::DrawCircle || selector_state_==SelectorState::DrawRectangle) && 
                        event.mouseButton.x >= x_lower && event.mouseButton.x < x_higher &&
                        event.mouseButton.y >= y_lower && event.mouseButton.y < y_higher)
                    {
                        pos_mouse = true; //positive mouse press detected
                        pos_mouse_time = selector_clock.getElapsedTime().asMilliseconds(); //record time
                        pos_mouse_pos.x = event.mouseButton.x;
                        pos_mouse_pos.y = event.mouseButton.y;
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased && pos_mouse)
                {
                    if ((selector_state_ == SelectorState::DrawCircle || selector_state_ == SelectorState::DrawRectangle) && 
                        event.mouseButton.x >= x_lower && event.mouseButton.x < x_higher &&
                        event.mouseButton.y >= y_lower && event.mouseButton.y < y_higher)
                    {
                        neg_mouse_time = selector_clock.getElapsedTime().asMilliseconds();
                        if (abs(neg_mouse_time - pos_mouse_time) >= 5)
                        {
                            //hold time must be about 5 milliseconds
                            neg_mouse_pos.x = event.mouseButton.x;
                            neg_mouse_pos.y = event.mouseButton.y;
                            double radius = sqrt((neg_mouse_pos.x-pos_mouse_pos.x)*(neg_mouse_pos.x-pos_mouse_pos.x)
                                                        +(neg_mouse_pos.y-pos_mouse_pos.y)*(neg_mouse_pos.y-pos_mouse_pos.y));
                            //check if the circle intersects anything
                            bool does_circle_overflow = checkCircleOverflow(sf::Vector2f(pos_mouse_pos.x,pos_mouse_pos.y) ,radius , x_lower , x_higher , y_lower , y_higher);
                            if (radius >= 10 && !does_circle_overflow)
                            {
                                if (selector_state_ == SelectorState::DrawCircle)
                                {
                                CircleObstacle circ(radius,pos_mouse_pos);
                                app_data_->circle_obs_array.array.emplace_back(circ);
                                std::cout<<"[Obstacle Selector]: Circle Obstacle Added!\n";
                                }
                                else if (selector_state_ == SelectorState::DrawRectangle)
                                {
                                    sf::Vector2f size = {abs(pos_mouse_pos.x-neg_mouse_pos.x),abs(pos_mouse_pos.y-neg_mouse_pos.y)};

                                    if ( (pos_mouse_pos.x < neg_mouse_pos.x) && (pos_mouse_pos.y < neg_mouse_pos.y) )
                                    {
                                        app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(pos_mouse_pos.x + size.x / 2,pos_mouse_pos.y + size.y / 2)));
                                        std::cout << "[Obstacle Selector]: Rectangle Obstacle Added!\n";
                                    }

                                    else if ((pos_mouse_pos.x > neg_mouse_pos.x) && (pos_mouse_pos.y > neg_mouse_pos.y) )
                                    {
                                        app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(neg_mouse_pos.x + size.x / 2,neg_mouse_pos.y + size.y / 2)));
                                        std::cout << "[Obstacle Selector]: Rectangle Obstacle Added!\n";
                                    }

                                    else if ((pos_mouse_pos.x > neg_mouse_pos.x) && (pos_mouse_pos.y < neg_mouse_pos.y) )
                                    {
                                        app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(neg_mouse_pos.x + size.x / 2,pos_mouse_pos.y + size.y/2)));
                                        std::cout<<"[Obstacle Selector]: Rectangle Obstacle Added!\n";
                                    }

                                    else if ((pos_mouse_pos.x < neg_mouse_pos.x) && (pos_mouse_pos.y > neg_mouse_pos.y) )
                                    {
                                        app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(pos_mouse_pos.x + size.x / 2,neg_mouse_pos.y + size.y / 2)));
                                        std::cout<<"[Obstacle Selector]: Rectangle Obstacle Added!\n";
                                    }
                                    
                                }
                            }
                        }
                    }
                    //reset everything before leaving this event
                    pos_mouse = 0;
                    pos_mouse_time = 0;
                    neg_mouse_time = 0;
                    pos_mouse_pos = {-1,-1};
                    neg_mouse_pos = {-1,-1};
                }
            }

            else if (app_data_->getChosenMap() == "discrete" && selector_state_ == SelectorState::DrawRectangle)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    double posx = event.mouseButton.x;
                    double posy = event.mouseButton.y;
                    bool isPointInRange = posx >= x_lower && posx < x_higher && posy >= y_lower && posy < y_higher;
                    if (isPointInRange)
                    {
                        Vec2D pos_cont_non_origin(posx,posy);
                        Vec2D pos_grid = pos_cont_non_origin.ContinuousSpaceToGridSpace(resolution_,gui_window_selector_size_.x,gui_window_selector_size_.y,app_data_->getControlPaneWidth(),0);
                        int idx_to_check =  num_cols * pos_grid.y() + pos_grid.x();
                        bool isPointValid = !discrete_point_tester[idx_to_check];
                        if (isPointValid)
                        {
                            discrete_point_tester[idx_to_check]=1;
                            Vec2D pos_cont_origin = pos_grid.GridSpaceToContinuousSpace(resolution_,app_data_->getControlPaneWidth(),0);
                            RectangleObstacle newobs(sf::Vector2f(resolution_.first,resolution_.second),pos_cont_origin);
                            app_data_ ->rect_obs_array.array.push_back(newobs);
                        }
                    }
                }
            }
        } //end of callback and event handling

        if (!run_state_)
        {
            return; //we kill the program here if the window was closed
        }
        //do actions are various states. 
        if (selector_state_ == SelectorState::ResetLastCircle)
        {
            if (app_data_ ->getChosenMap() == "discrete")
            {
                assert(app_data_ ->circle_obs_array.array.empty());
                std::cout<<"[Obstacle Selector]: Circle obstacles unavailable for discrete map.\n";
                selector_state_ = SelectorState::DrawRectangle;
            }
            else if (app_data_->getChosenMap() == "continuous")
            {
                if (app_data_->circle_obs_array.array.size()>0)
                {
                    app_data_->circle_obs_array.array.pop_back();
                    selector_state_ = SelectorState::DrawCircle; //reset state
                    std::cout << "[Obstacle Selector]: Circle Obstacles Removed!\n";
                }
                else 
                {
                    std::cout << "[Obstacle Selector]: No Circle Obstacles Left!\n";
                    selector_state_ = SelectorState::DrawCircle; //reset state
                }
            }
        }
        
        else if (selector_state_ == SelectorState::ResetLastRectangle)
        {
            if (app_data_->getChosenMap()=="continuous")
            {    
                if (app_data_->rect_obs_array.array.size()>0)
                {
                    app_data_->rect_obs_array.array.pop_back();
                    std::cout << "[Obstacle Selector]: Rectangle Obstacle Removed!\n";
                    selector_state_ = SelectorState::DrawRectangle;
                }
                else
                {
                    std::cout << "[Obstacle Selector]: No Rectangle Obstacles Left\n";
                    selector_state_ = SelectorState::DrawRectangle; //reset state
                }
            }
            else if (app_data_->getChosenMap()=="discrete")
            {
                if (app_data_->rect_obs_array.array.size() > 0)
                {
                    Vec2D cont_global_coords;
                    cont_global_coords.x(app_data_ -> rect_obs_array.array.at(app_data_->rect_obs_array.array.size()-1).getPosition().x);
                    cont_global_coords.y(app_data_ -> rect_obs_array.array.at(app_data_->rect_obs_array.array.size()-1).getPosition().y);
                    Vec2D grid_coords = cont_global_coords.ContinuousSpaceToGridSpace(this->resolution_,gui_window_selector_size_.x,gui_window_selector_size_.y,app_data_->getControlPaneWidth(),0);
                    std::cout<<"Global Coords:\n";
                    cont_global_coords.print();
                    std::cout<<"Grid Coords:\n";
                    grid_coords.print();
                    int num_col = app_data_->getMapX() / resolution_.first;
                    int flattened_coords = num_col * grid_coords.y() + grid_coords.x();
                    std::cout<<"Flattened Coord: "<<flattened_coords<<"\n";
                    discrete_point_tester[flattened_coords] = 0;
                    app_data_ ->rect_obs_array.array.pop_back();
                    std::cout << "[ObstacleSelector]: Rectangle Obstacle Removed!\n";
                    selector_state_ = SelectorState::DrawRectangle;
                }
                else
                {
                    std::cout<<"[ObstacleSelector]: No Rectangle Obstacles left\n";
                }
            }
        }

        else if (selector_state_ == SelectorState::ResetAll)
        {
            if (app_data_->circle_obs_array.array.size()>0)
            {
                app_data_->circle_obs_array.array.clear();
            }

            if (app_data_->rect_obs_array.array.size()>0)
            {
                app_data_->rect_obs_array.array.clear();
            }

            if (app_data_->getChosenMap() == "discrete")
            {
                for (auto& a: discrete_point_tester)
                {
                    a = 0;
                }
            }
            std::cout << "[Obstacle Selector]: Obstacle selections cleared!\n";
            selector_state_ = SelectorState::DrawRectangle;
        }

        else if (selector_state_ == SelectorState::Random)
        {
            if (app_data_->getChosenMap() == "continuous")
            {
                GenerateRandomContinuous();
                selector_state_ = SelectorState::DrawRectangle;
            }
            else if (app_data_->getChosenMap() == "discrete"){
                GenerateRandomDiscrete(discrete_point_tester);
                selector_state_ = SelectorState::DrawRectangle;
            }
        }

        else if (selector_state_ == SelectorState::SaveMap)
        {
            std::string map_name;
            std::string chosen_map_type = app_data_->getChosenMap();
            std::cout << "[PlannerGUI]: Enter your desired map name!:";
            std::cin >> map_name;
            map_name += ".yaml";
            std::filesystem::path path_to_map{"maps/" + chosen_map_type};
            path_to_map /= map_name;

            int number_circle_obs = app_data_ ->circle_obs_array.array.size();
            int number_rectangle_obs = app_data_ ->rect_obs_array.array.size();
            
            YAML::Emitter emtr;
            //emtr << YAML::Comment << "Do not modify saved maps!";
            emtr << YAML::BeginMap;
            emtr << YAML::Key << "map_type";
            emtr << YAML::Value << chosen_map_type;
            emtr << YAML::Key << "number_of_rectangle_obstacles";
            emtr << YAML::Value << number_rectangle_obs;
            emtr << YAML::Key << "number_of_circle_obstacles";
            emtr << YAML::Value << number_circle_obs;
            
            if (app_data_->rect_obs_array.array.size()!=0)
            {
                emtr << YAML::Key << "rectangle_obstacles";
                emtr << YAML::Value;
                emtr << YAML::BeginSeq;
                for (size_t i = 0 ; i<number_rectangle_obs ; ++i)
                {
                    sf::Vector2f size = app_data_->rect_obs_array.array.at(i).getSize();
                    sf::Vector2f pos = app_data_->rect_obs_array.array.at(i).getPosition();
                    std::vector<float> size_vec = {size.x , size.y};
                    std::vector<float> pos_vec = {pos.x , pos.y};
                    emtr << YAML::BeginMap;
                    emtr << YAML::Key << "index";
                    emtr << YAML::Value << i;
                    emtr << YAML::Key << "size";
                    emtr << YAML::Value;
                    emtr << YAML::Flow << size_vec;
                    emtr << YAML::Key << "position";
                    emtr << YAML::Flow << pos_vec;
                    emtr << YAML::EndMap;
                }
                emtr << YAML::EndSeq;
            }

            if (app_data_->circle_obs_array.array.size()!=0)
            {
                emtr << YAML::Key << "circle_obstacles";
                emtr << YAML::Value;
                emtr << YAML::BeginSeq;
                for (size_t i = 0 ; i<number_circle_obs ; ++i)
                {
                    double radius = app_data_->circle_obs_array.array.at(i).getRadius();
                    sf::Vector2f pos = app_data_->circle_obs_array.array.at(i).getPosition();
                    std::vector<float> pos_vec = {pos.x , pos.y};
                    emtr << YAML::BeginMap;
                    emtr << YAML::Key << "index";
                    emtr << YAML::Value << i;
                    emtr << YAML::Key << "radius";
                    emtr << YAML::Value << radius;
                    emtr << YAML::Key << "position";
                    emtr << YAML::Flow << pos_vec;
                    emtr << YAML::EndMap;
                }
                emtr << YAML::EndSeq;
            }
            std::cout<<"[PlannerGUI]: Saving map now!\n";
            if (number_circle_obs + number_rectangle_obs == 0)
            {
                std::cout<<"[PlannerGUI]: You are saving an empty map!\n";
            }
            std::ofstream fout(path_to_map);
            fout << emtr.c_str();
            std::cout<<"[PlannerGUI]: Map saved!\n";
            selector_state_ = SelectorState::DrawRectangle;
        }

        else if (selector_state_ == SelectorState::LoadMap)
        {
            std::string map_name; //ensure that this in the correct map type subdirectory
            std::cout<<"[PlannerGUI]: Input name of map to be loaded: ";
            std::cin >> map_name;
            map_name += ".yaml";
            std::string path_to_load = "maps/" +app_data_->getChosenMap() + "/" + map_name;
            YAML::Node loader = YAML::LoadFile(path_to_load);
            int num_rect_obs = loader["number_of_rectangle_obstacles"].as<int>();
            int num_circle_obs = loader["number_of_circle_obstacles"].as<int>();
            
            std::cout<<"[PlannerGUI]: Loading a map will erase existing map.\n";
            app_data_->rect_obs_array.array.clear();
            app_data_->circle_obs_array.array.clear();
            std::cout<<"[PlannerGUI]: Existing map erased. Loading map now\n";
            if (num_rect_obs+num_circle_obs == 0)
            {
                std::cout<<"[PlannerGUI]: You are loading an empty map\n";
            }
            else
            {
                if (num_rect_obs > 0)
                {
                    for (std::size_t i=0 ; i<num_rect_obs;i++)
                    {
                        std::vector<float> size_array;
                        std::vector<float> pos_array;
                        for (std::size_t j =0 ; j<2; ++j)
                        {
                            float size_val = loader["rectangle_obstacles"][i]["size"][j].as<float>();
                            float pos_val = loader["rectangle_obstacles"][i]["position"][j].as<float>();
                            size_array.push_back(size_val);
                            pos_array.push_back(pos_val);
                        }
                        sf::Vector2f size(size_array[0],size_array[1]);
                        sf::Vector2f pos(pos_array[0],pos_array[1]);
                        RectangleObstacle rectObs(size,pos);
                        app_data_->rect_obs_array.array.emplace_back(rectObs);
                    }
                    std::cout<<"[PlannerGUI]: Loaded rectangle obstacles!\n";
                }
                if (num_circle_obs > 0)
                {
                    for (std::size_t i=0 ; i<num_circle_obs;i++)
                    {
                        double radius = loader["circle_obstacles"][i]["radius"].as<double>();
                        std::vector<float> pos_array;
                        for (std::size_t j = 0 ; j<2; ++j)
                        {
                            float pos_val = loader["circle_obstacles"][i]["position"][j].as<double>();
                            pos_array.push_back(pos_val);
                        }
                        sf::Vector2f pos(pos_array[0],pos_array[1]);
                        CircleObstacle circleObs(radius, pos);
                        app_data_->circle_obs_array.array.emplace_back(circleObs);
                    }
                    std::cout<<"[PlannerGUI]: Loaded circle obstacles!\n";
                }
                std::cout<<"[PlannerGUI]: Loaded all obstacles successfully!\n";
            }
            selector_state_ = SelectorState::DrawRectangle;
        }
        
        else if (selector_state_ ==  SelectorState::Done)
        {
            gui_window_selector_.close();
            return;
        }

        else if (selector_state_ == SelectorState::Quit)
        {
            gui_window_selector_.close();
            run_state_ = false;
            return;
        }
        gui_window_selector_.clear(sf::Color::Black);
        DrawSelectorWindow();
        gui_window_selector_.display();
    }
}

void PlannerGUI::GenerateRandomContinuous()
{
    int successful_points = 0;
    int required_points = app_data_->getNumRandomObs();
    bool circle_or_rect_switch = 0; // 0 for circle, 1 for rect. This is to avoid checking both arrays 
    std::random_device rdx;
    std::random_device rdy;

    std::default_random_engine engx(rdx());
    std::default_random_engine engy(rdy());
    
    std::uniform_real_distribution<double> distrx(app_data_->getControlPaneWidth() + default_circobs_radius_,gui_window_selector_size_.x - default_circobs_radius_);
    std::uniform_real_distribution<double> distry(default_circobs_radius_,app_data_->getMapY() - default_circobs_radius_);

    if (!app_data_->circle_obs_array.array.empty()){app_data_->circle_obs_array.array.clear();} 
    if (!app_data_->rect_obs_array.array.empty()){app_data_->rect_obs_array.array.clear();}

    while (successful_points < required_points)
    {
        bool checkStatus = true;
        double randx=distrx(engx);
        double randy=distry(engy);
        Vec2D randPoint(randx,randy);
        
        for (const auto &obs: app_data_->circle_obs_array.array)
        {
            double centre_dist = sqrt(pow(randPoint.x()-obs.getPosition().x,2) + pow(randPoint.y()-obs.getPosition().y,2));
            double longest_centre_dist = 2 * default_circobs_radius_;
            double buffer = 3;
            if (centre_dist < longest_centre_dist + buffer - EPS)
            {
                checkStatus = false;
                break;
            }
        }

        for (const auto &obs: app_data_->rect_obs_array.array)
        {
            double centre_dist = sqrt(pow(randPoint.x()-obs.getPosition().x,2) + pow(randPoint.y()-obs.getPosition().y,2));
            double longest_centre_dist = 2 * default_circobs_radius_;
            double buffer = 3;
            if (centre_dist < longest_centre_dist + buffer - EPS)
            {
                checkStatus = false;
                break;
            }
        }
        if (checkStatus)
        {
            if (!circle_or_rect_switch)
            {
                CircleObstacle newObs(this->default_circobs_radius_,randPoint);
                app_data_->circle_obs_array.array.push_back(newObs);
            }
            else
            {
                RectangleObstacle newObs(this->default_rectobs_size_,randPoint);
                app_data_->rect_obs_array.array.push_back(newObs);
            }
            circle_or_rect_switch =! circle_or_rect_switch;
            successful_points ++ ;
        }
    }
    std::cout<<"[ObstacleSelector]: " << app_data_->circle_obs_array.array.size() << " Circle Obstacles created!\n";
    std::cout<<"[ObstacleSelector]: " << app_data_->rect_obs_array.array.size() << " Rectangle Obstacles created!\n";
}

void PlannerGUI::DrawControlPane()
{
    gui_backend_selector_.removeAllWidgets();
    sf::RectangleShape control_pane_background;
    control_pane_background.setPosition(0,0);
    control_pane_background.setSize(sf::Vector2f(200,gui_window_selector_.getSize().y));
    control_pane_background.setFillColor(sf::Color::Yellow);
    gui_window_selector_.draw(control_pane_background);
    gui_backend_selector_.add(circle_obs_button);
    gui_backend_selector_.add(rectangle_obs_button);
    gui_backend_selector_.add(reset_last_circle_obs_button);
    gui_backend_selector_.add(reset_last_rectangle_obs_button);
    gui_backend_selector_.add(reset_all_obstacle_button);
    gui_backend_selector_.add(random_obstacle_button);
    gui_backend_selector_.add(save_map_button);
    gui_backend_selector_.add(load_map_button);
    gui_backend_selector_.add(quit_button);
    gui_backend_selector_.add(done_button);
    gui_backend_selector_.draw();
}

void PlannerGUI::DrawSelectorWindow()
{
    DrawControlPane();
    //drawing circle obstacles
    for (const auto& circ_obs : app_data_->circle_obs_array.array)
    {
        gui_window_selector_.draw(circ_obs.shape);
    }

    //drawing rectangle obstacles
    for (const auto& rect_obs : app_data_->rect_obs_array.array)
    {
        gui_window_selector_.draw(rect_obs.shape);
    }
    if (app_data_ -> getChosenMap() == "discrete")
    {
        GenerateGrids();
    }
}
void PlannerGUI::GenerateGrids()
{
    int x_lower=app_data_->getControlPaneWidth();
    int x_higher=app_data_->getMapX()+x_lower;
    int y_lower = 0;
    int y_higher = app_data_ ->getMapY();
    for (int i=x_lower+resolution_.first; i<x_higher ; i+=resolution_.first)
    {
        sf::Vector2f pt1(i,y_lower);
        sf::Vector2f pt2(i,y_higher-1);
        sf::Vertex vertical_line[]=
        {
            sf::Vertex(pt1),
            sf::Vertex(pt2)
        };
        this->gui_window_selector_.draw(vertical_line,2,sf::Lines);
    }
    for (int i=y_lower+resolution_.second; i<y_higher ; i+=resolution_.second)
    {
        sf::Vector2f pt1(x_lower,i);
        sf::Vector2f pt2(x_higher-1,i);
        sf::Vertex horizontal_line[]=
        {
            sf::Vertex(pt1),
            sf::Vertex(pt2)
        };
        this->gui_window_selector_.draw(horizontal_line,2,sf::Lines);
    }
}
//returns true if overflow
bool PlannerGUI::checkCircleOverflow(const sf::Vector2f &centre , double radius , double x_lower , double x_higher , double y_lower , double y_higher)
{
    //checking horizontal line collisions
    const double radius_squared = radius * radius;
    bool x_lower_check = ((x_lower - centre.x) * (x_lower - centre.x)) <= radius_squared; //true if intersect
    bool x_higher_check = ((x_higher - centre.x) * (x_higher - centre.x)) <= radius_squared;
    bool y_lower_check = ((y_lower - centre.y) * (y_lower - centre.y)) <= radius_squared;
    bool y_higher_check = ((y_higher- centre.y) * (y_higher - centre.y)) <= radius_squared;
    if (x_lower_check || x_higher_check || y_lower_check || y_higher_check)
    {
        return true;
    }
    return false;

}
void PlannerGUI::GenerateRandomDiscrete(std::vector<int> &discrete_point_tracker)
{
    int successful_points = 0;
    int required_points = app_data_->getNumRandomObs();
    int num_cols = app_data_->getMapX() / app_data_->getResolution().first;
    int num_rows = app_data_->getMapY() / app_data_->getResolution().second;
    int possible_points = num_cols * num_rows;
    required_points = required_points >= possible_points ? possible_points * 0.6 : required_points; //default to 60% of the map if its too large
    int x_min = 0;
    int x_max = num_cols;
    int y_min = 0;
    int y_max = num_rows;

    //we generate coordinates in gridspace here
    std::random_device rdx;
    std::random_device rdy;

    std::default_random_engine engx(rdx());
    std::default_random_engine engy(rdy());
    
    std::uniform_int_distribution<int> distrx(x_min , x_max - 1);
    std::uniform_int_distribution<int> distry(y_min , y_max - 1);

    if (!app_data_->circle_obs_array.array.empty()){app_data_->circle_obs_array.array.clear();}
    if (!app_data_->rect_obs_array.array.empty()){app_data_->rect_obs_array.array.clear();}
    for (auto& a : discrete_point_tracker)
    {
        a = 0;
    }
    while (successful_points < required_points)
    {
        bool checkStatus = true;
        double randx=distrx(engx);
        double randy=distry(engy);
        Vec2D randPoint_discrete(randx,randy);
        checkStatus = checkDiscretePoint(randPoint_discrete,discrete_point_tracker,app_data_->getMapX());
        if (checkStatus)
        {
            Vec2D randPoint_continuous = randPoint_discrete.GridSpaceToContinuousSpace(app_data_ ->getResolution(),app_data_->getControlPaneWidth(),0);
            RectangleObstacle newobs(sf::Vector2f(this->resolution_.first,this->resolution_.second),randPoint_continuous);
            app_data_->rect_obs_array.array.push_back(newobs);
            successful_points ++ ;
        }
    }
    int count = 0;
    for (const auto& a : discrete_point_tracker)
    {
        count+=a;
    }
    assert(count == successful_points && count == app_data_->rect_obs_array.array.size());
    std::cout<<"[ObstacleSelector]: " << app_data_->circle_obs_array.array.size() << " Circle Obstacles created!\n";
    std::cout<<"[ObstacleSelector]: " << app_data_->rect_obs_array.array.size() << " Rectangle Obstacles created!\n";
}

bool PlannerGUI::checkDiscretePoint(const Vec2D &grid_space_coord,std::vector<int> &discrete_point_tracker, int map_size_x)
{
    bool pointOk = true;
    int num_col = map_size_x / resolution_.first;
    int flattened_coords = num_col * grid_space_coord.y() + grid_space_coord.x();
    pointOk = !discrete_point_tracker[flattened_coords];

    if (pointOk)
    {
        //update the point tracker array. We will select this point
        discrete_point_tracker[flattened_coords] = 1;
    }
    return pointOk;
}

bool PlannerGUI::DataTest()
{
    bool pass = true;

    if (app_data_->getChosenMap() != "discrete" && app_data_->getChosenMap() != "continuous")
    {
        pass = false;
        std::cout<<"[PlannerGUI]: Map type can only be discrete or continuous. It is not set correctly!\n";
        return false;
    }

    if (app_data_->getChosenMap() == "continuous")
    {
        std::string planner = app_data_->getChosenPlanner();
        bool check = false;
        for (const auto& a:app_data_->continuous_planners)
        {
            std::cout<<a<<"\n";
            if (a == planner)
            {
                check = true;
            }
        }
        if (!check)
        {
            pass = false;
            std::cout<<"[PlannerGUI]: Planner is invalid!\n";
            return pass;
        }
    }

    else if (app_data_->getChosenMap() == "discrete")
    {
        std::string planner = app_data_->getChosenPlanner();
        std::cout<<planner<<"\n";
        bool check = false;
        for (const auto& a:app_data_->discrete_planners)
        {
            std::cout<<a<<"\n";
            if (a == planner)
            {
                check = true;
            }
        }
        if (!check)
        {
            pass = false;
            std::cout<<"[PlannerGUI]: Planner is invalid!\n";
            return pass;
        }
    }

    int rez_x = app_data_->getResolution().first;
    int rez_y = app_data_->getResolution().second;
    
    if (rez_x != rez_y && app_data_->getChosenMap() == "discrete")
    {
        std::cout<<"[PlannerGUI]: Invalid Resolution. Resolution must be equal\n";
        pass = false;
        return pass;
    }

    else
    {
        int map_x = app_data_->getMapX();
        int map_y = app_data_->getMapY();
        
        if (map_x % rez_x != 0 || map_y % rez_y != 0)
        {
            std::cout<<"[PlannerGUI]: Invalid Resolutuon. Resolution does not evenly divide the map!\n";
            pass = false;
            return pass;
        }
    }

    std::cout<<"[DataCheck]: Checking app data now!\n";
    std::cout<<"######SUMMARY#######\n";
    std::cout<<"[DataCheck]: Chosen Map Type: "<<app_data_->getChosenMap()<<"\n";
    std::cout<<"[DataCheck]: Chosen Planner: "<<app_data_->getChosenPlanner()<<"\n";
    std::cout<<"[DataCheck]: Map Size: "<<"("<<app_data_->getMapX()<<","<<app_data_->getMapY()<<")\n";
    if (app_data_->getChosenMap() == "discrete")
    {
        std::cout<<"[DataCheck]: Resolution: "<<"("<<app_data_->getResolution().first<<","<<app_data_->getResolution().second<<")\n";
    }
    int num_circ_obs = app_data_->circle_obs_array.array.size();
    int num_rect_obs = app_data_->rect_obs_array.array.size();
    int total_obstacles = num_circ_obs + num_rect_obs;
    if (total_obstacles == 0)
    {
        std::cout<<"[DataCheck]: No obstacles have been created.\n";
    }
    else
    {
        std::cout<<"[DataCheck]: You have created "<<total_obstacles<<" obstacles\n";
    }
    std::cout<<"#####################\n";
    return pass;
}