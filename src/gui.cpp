#include <gui.hpp>

AppData::AppData()
{
    chosen_map_type_="";
    chosen_planner_="";
    loader=YAML::LoadFile("config/config.yaml");
    continuous_planners=loader["continuous_planner"].as<std::vector<std::string>>();
    discrete_planners=loader["discrete_planner"].as<std::vector<std::string>>();
    resolution_=std::make_pair(loader["resolution"].as<std::vector<int>>()[0],loader["resolution"].as<std::vector<int>>()[1]);
    std::cout<<"[AppData]: List of planners loaded!\n";
}

std::string AppData::getChosenMap()
{
    return this->chosen_map_type_;
}

std::string AppData::getChosenPlanner()
{
    return this->chosen_planner_;
}

void AppData::setChosenMap(std::string type)
{
    this->chosen_map_type_=type;
}

void AppData::setChosenPlanner(std::string type)
{
    this->chosen_planner_=type;
}

PlannerGUI::PlannerGUI(std::shared_ptr<AppData> data,int window_x,int window_y,int selector_window_x, int selector_window_y)
{   
    //sf::RenderWindow window{{800, 600}, "TGUI window with SFML"};
    gui_window_size_.x=window_x;
    gui_window_size_.y=window_y;
    selector_window_size_.x=selector_window_x;
    selector_window_size_.y=selector_window_y;
    gui_window_.create(sf::VideoMode(gui_window_size_.x,gui_window_size_.y,32),"Planner GUI");
    gui_backend_.setTarget(gui_window_);
    background_color_=sf::Color::Red;
    dark_theme_.setDefault("themes/Black.txt");
    screen_num = 0;
    app_data_=data;

}

void PlannerGUI::run()
{
    while (gui_window_.isOpen())
    {
        sf::Event event;
        while (gui_window_.pollEvent(event))
        {
            gui_backend_.handleEvent(event);
            if (event.type==sf::Event::Closed)
            {
                gui_window_.close();
            }
        }
        //gui_window_.clear(sf::Color::Red);
        if (screen_num == 0){DrawHomeScreen();}
        else if (screen_num == 1){DrawMapTypeScreen();}
        else if (screen_num==2){DrawContinuousMapSelection();}
        else if (screen_num==3){DrawDiscreteMapSelection();}
        gui_backend_.draw();
        gui_window_.display();
        gui_window_.clear(sf::Color::Red);
    }
    sf::sleep(sf::seconds(1));
    runObstacleSelector();       
}

void PlannerGUI::DrawHomeScreen()
{
    gui_backend_.removeAllWidgets();

    auto label_title = tgui::Label::create(); 
    auto go_button = tgui::Button::create("Go");
    auto exit_button = tgui::Button::create("Exit");
    sf::Texture img_texture;
    sf::Sprite img_sprite;

    label_title->setText("Path Planning Visualizations");
    label_title->setTextSize(50);
    label_title->setPosition(50,100);
    
    
    go_button->setPosition(350,460);
    go_button->setSize(150,120);
    go_button->setText("GO!");
    go_button->setTextSize(0);
    go_button->onMousePress(&PlannerGUI::goCallback,this);

    exit_button->setPosition(700,500);
    exit_button->setSize(80,80);
    exit_button->setText("Exit");
    exit_button->setTextSize(0);
    exit_button->onMousePress(&PlannerGUI::quitCallback,this);

    img_texture.loadFromFile("themes/homescreen.png");
    img_sprite.setTexture(img_texture);
    img_sprite.setScale(200.f / img_texture.getSize().x, 200.f / img_texture.getSize().y);
    auto canvas = tgui::Canvas::create({200, 200});
    canvas->setPosition(320, 200);
    canvas->clear();
    canvas->draw(img_sprite);
    canvas->display();

    //adding stuff
    gui_backend_.add(label_title);
    gui_backend_.add(go_button);
    gui_backend_.add(exit_button);
    gui_backend_.add(canvas);

}

void PlannerGUI::quitCallback()
{
    gui_window_.close(); //ToDO, use RAII standards to release resources using destructor
}

void PlannerGUI::goCallback()
{
    screen_num = 1;
    //sf::sleep(sf::seconds(5));
}

void PlannerGUI::DrawMapTypeScreen()
{
    gui_backend_.removeAllWidgets();
    
    auto label_description = tgui::Label::create();
    auto continuous_map_button = tgui::Button::create();
    auto discrete_map_button = tgui::Button::create();

    label_description->setText("Choose a Map Type!");
    label_description->setTextSize(50);
    label_description->setPosition(150,100);

    continuous_map_button->setPosition(160,300);
    continuous_map_button->setSize(180,120);
    continuous_map_button->setText("Continuous\n      Map");
    continuous_map_button->setTextSize(0);
    continuous_map_button->onMousePress(&PlannerGUI::ContinuousMapTypeCallback,this);
    
    discrete_map_button->setPosition(480,300);
    discrete_map_button->setSize(180,120);
    discrete_map_button->setText("Discrete\n    Map");
    discrete_map_button->setTextSize(28);
    discrete_map_button->onMousePress(&PlannerGUI::DiscreteMapTypeCallback,this);

    gui_backend_.add(label_description);
    gui_backend_.add(continuous_map_button);
    gui_backend_.add(discrete_map_button);
}



void PlannerGUI::ContinuousMapTypeCallback()
{
    if (app_data_->getChosenMap().size()!=0)
    {
        return;
    }
    app_data_->setChosenMap("continuous");
    screen_num=2;

}

void PlannerGUI::DiscreteMapTypeCallback()
{
    if (app_data_->getChosenMap().size()!=0)
    {
        return;
    }
    app_data_->setChosenMap("discrete");
    screen_num=3;
}

void PlannerGUI::DrawDiscreteMapSelection()
{
    gui_backend_.removeAllWidgets();
    
    int num_planners=app_data_->discrete_planners.size();
    const int initial_x=100;
    const int initial_y=200;
    const sf::Vector2u button_size={80,100};
    const int spacing_x=button_size.x + 20;
    const int spacing_y=button_size.y + 20;
    const int rowcol=3;
    auto label=tgui::Label::create();
    label->setPosition(140,50);
    label->setText("Choose a Discrete Planner!");
    label->setTextSize(40);
    gui_backend_.add(label);
    std::vector<tgui::Button::Ptr> buttons;

    for (int i=0; i<num_planners ; ++i)
    {
        auto button=tgui::Button::create();
        buttons.push_back(button);
    }

    for (int i=0; i<num_planners; ++i)
    {
        buttons[i]->setSize(button_size.x,button_size.y);
        buttons[i]->setPosition(initial_x + spacing_x * (i / rowcol),initial_y + spacing_y * (i % rowcol));
        buttons[i]->setText(app_data_->discrete_planners[i]);
        buttons[i]->setTextSize(0);
        buttons[i]->onMousePress(&PlannerGUI::ChoosePlannerCallback,this,app_data_->discrete_planners[i]);
        gui_backend_.add(buttons[i]);
    }
}

void PlannerGUI::DrawContinuousMapSelection()
{
    gui_backend_.removeAllWidgets();
    
    int num_planners=app_data_->continuous_planners.size();
    const int initial_x=100;
    const int initial_y=200;
    const sf::Vector2u button_size={80,100};
    const int spacing_x=button_size.x + 20;
    const int spacing_y=button_size.y + 20;
    const int rowcol=3;
    auto label=tgui::Label::create();
    label->setPosition(140,50);
    label->setText("Choose a Continuous Planner!");
    label->setTextSize(40);
    gui_backend_.add(label);
    std::vector<tgui::Button::Ptr> buttons;

    for (int i=0; i<num_planners ; ++i)
    {
        auto button=tgui::Button::create();
        buttons.push_back(button);
    }

    for (int i=0; i<num_planners; ++i)
    {
        buttons[i]->setSize(button_size.x,button_size.y);
        buttons[i]->setPosition(initial_x + spacing_x * (i / rowcol),initial_y + spacing_y * (i % rowcol));
        buttons[i]->setText(app_data_->continuous_planners[i]);
        buttons[i]->setTextSize(0);
        buttons[i]->onMousePress(&PlannerGUI::ChoosePlannerCallback,this,app_data_->continuous_planners[i]);
        gui_backend_.add(buttons[i]);
    }
}

void PlannerGUI::ChoosePlannerCallback(std::string planner)
{
    app_data_->setChosenPlanner(planner);
    std::cout<<"[Planner GUI]: Planner and Map Chosen. Prepare to set Obstacles\n";
    sf::sleep(sf::seconds(1));
    gui_window_.close();
}

void PlannerGUI::runObstacleSelector()
{
    gui_window_selector_.create(sf::VideoMode(selector_window_size_.x,selector_window_size_.y,32),"Selector GUI");
    gui_backend_selector_.setTarget(gui_window_selector_);
    
    Actions state=Actions::NotStarted;
    
    sf::Clock selector_clock; // check how long the mouse has be held
    bool pos_mouse = 0;
    sf::Int32 pos_mouse_time = 0;
    sf::Int32 neg_mouse_time = 0;
    sf::Vector2f pos_mouse_pos = {-1,-1};
    sf::Vector2f neg_mouse_pos = {-1,-1};
    
    while (gui_window_selector_.isOpen())
    {
        sf::Event event;
        while (gui_window_selector_.pollEvent(event))
        {
            gui_backend_selector_.handleEvent(event);
            if (event.type == sf::Event::Closed)
            {
                gui_window_selector_.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && !pos_mouse) 
            {
                if ((state == Actions::DrawCircle || state==Actions::DrawRectangle) && 
                    event.mouseButton.x > 200 && 
                    event.mouseButton.x < (int)gui_window_selector_.getSize().x &&
                    event.mouseButton.y >= 0 &&
                    event.mouseButton.y < (int)gui_window_selector_.getSize().y)
                {
                    pos_mouse = true; //positive mouse press detected
                    pos_mouse_time = selector_clock.getElapsedTime().asMilliseconds(); //record time
                    pos_mouse_pos.x = event.mouseButton.x;
                    pos_mouse_pos.y = event.mouseButton.y;
                }
            }

            if (event.type==sf::Event::MouseButtonReleased && pos_mouse)
            {
                if ((state == Actions::DrawCircle || state == Actions::DrawRectangle) && 
                    event.mouseButton.x > 200 && 
                    event.mouseButton.x < (int)gui_window_selector_.getSize().x &&
                    event.mouseButton.y >= 0 &&
                    event.mouseButton.y < (int)gui_window_selector_.getSize().y)
                {
                    neg_mouse_time = selector_clock.getElapsedTime().asMilliseconds();
                    if (abs(neg_mouse_time - pos_mouse_time) >= 5)
                    {
                        //hold time must be about 5 milliseconds
                        neg_mouse_pos.x=event.mouseButton.x;
                        neg_mouse_pos.y=event.mouseButton.y;
                        double dist_between= sqrt((neg_mouse_pos.x-pos_mouse_pos.x)*(neg_mouse_pos.x-pos_mouse_pos.x)
                                                    +(neg_mouse_pos.y-pos_mouse_pos.y)*(neg_mouse_pos.y-pos_mouse_pos.y));
                        if (dist_between>=10)
                        {
                            if (state==Actions::DrawCircle)
                            {
                               CircleObstacle circ(dist_between/2,pos_mouse_pos);
                               app_data_->circle_obs_array.array.emplace_back(circ);
                               std::cout<<"[Obstacle Selector]: Circle Obstacle Added!\n";
                            }
                            else if (state==Actions::DrawRectangle)
                            {
                                sf::Vector2f size = {abs(pos_mouse_pos.x-neg_mouse_pos.x),abs(pos_mouse_pos.y-neg_mouse_pos.y)};

                                if ( (pos_mouse_pos.x < neg_mouse_pos.x) && (pos_mouse_pos.y < neg_mouse_pos.y) )
                                {
                                    app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(pos_mouse_pos.x + size.x / 2,pos_mouse_pos.y + size.y / 2)));
                                    std::cout<<"[Obstacle Selector]: Rectangle Obstacle Added!\n";
                                }

                                else if ((pos_mouse_pos.x > neg_mouse_pos.x) && (pos_mouse_pos.y > neg_mouse_pos.y) )
                                {
                                    app_data_->rect_obs_array.array.emplace_back(RectangleObstacle(size,sf::Vector2f(neg_mouse_pos.x + size.x / 2,neg_mouse_pos.y + size.y / 2)));
                                    std::cout<<"[Obstacle Selector]: Rectangle Obstacle Added!\n";
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
                pos_mouse=0;
                pos_mouse_time=0;
                neg_mouse_time=0;
                pos_mouse_pos = {-1,-1};
                neg_mouse_pos = {-1,-1};
            }
        }
        gui_backend_selector_.removeAllWidgets();

        auto circle_obs_button=tgui::Button::create();
        circle_obs_button->setSize(80,80);
        circle_obs_button->setPosition(60,100);
        circle_obs_button->setText("Draw\nCircle");
        circle_obs_button->setTextSize(15);
        circle_obs_button->onMousePress([&]{state=Actions::DrawCircle;});
        gui_backend_selector_.add(circle_obs_button);
        
        auto rectangle_obs_button=tgui::Button::create();
        rectangle_obs_button->setSize(80,80);
        rectangle_obs_button->setPosition(60,200);
        rectangle_obs_button->setText(" Draw\nRectangle");
        rectangle_obs_button->setTextSize(15);
        rectangle_obs_button->onMousePress([&]{state=Actions::DrawRectangle;});
        gui_backend_selector_.add(rectangle_obs_button);
        
        auto reset_last_circle_obs_button=tgui::Button::create();
        reset_last_circle_obs_button->setSize(80,80);
        reset_last_circle_obs_button->setPosition(60,300);
        reset_last_circle_obs_button->setText("Reset\nLastCircle");
        reset_last_circle_obs_button->setTextSize(15);
        reset_last_circle_obs_button->onMousePress([&]{if(state == Actions::NotStarted)
                                                {return;}
                                                state=Actions::ResetLastCircle;});
        gui_backend_selector_.add(reset_last_circle_obs_button);

        auto reset_last_rectangle_obs_button=tgui::Button::create();
        reset_last_rectangle_obs_button->setSize(80,80);
        reset_last_rectangle_obs_button->setPosition(60,400);
        reset_last_rectangle_obs_button->setText("Reset\nLastRect");
        reset_last_rectangle_obs_button->setTextSize(15);
        reset_last_rectangle_obs_button->onMousePress([&]{if(state == Actions::NotStarted)
                                                {return;}
                                                state=Actions::ResetLastRectangle;});
        gui_backend_selector_.add(reset_last_rectangle_obs_button);
        
        auto reset_all_obstacle_button=tgui::Button::create();
        reset_all_obstacle_button->setSize(80,80);
        reset_all_obstacle_button->setPosition(60,500);
        reset_all_obstacle_button->setText("Reset\n All");
        reset_all_obstacle_button->setTextSize(15);
        reset_all_obstacle_button->onMousePress([&]{if(state == Actions::NotStarted)
                                                {return;}
                                                state=Actions::ResetAll;});
        gui_backend_selector_.add(reset_all_obstacle_button);
        
        auto done_button=tgui::Button::create();
        done_button->setSize(80,80);
        done_button->setPosition(60,600);
        done_button->setText("Done");
        done_button->setTextSize(15);
        done_button->onMousePress([&]{if(state == Actions::NotStarted)
                                                {return;}
                                                state=Actions::Done;});
        gui_backend_selector_.add(done_button);

        if (state==Actions::ResetLastCircle)
        {
            
            if (app_data_->circle_obs_array.array.size()>0)
            {
                app_data_->circle_obs_array.array.pop_back();
                state=Actions::DrawCircle; //reset state
                std::cout<<"[Obstacle Selector]: Circle Obstacles Removed!\n";
            }
            else 
            {
                std::cout<<"[Obstacle Selector]: No Circle Obstacles Left!\n";
                state=Actions::DrawCircle; //reset state
            }
        }
        
        else if (state == Actions::ResetLastRectangle)
        {
            if (app_data_->rect_obs_array.array.size()>0)
            {
                app_data_->rect_obs_array.array.pop_back();
                std::cout<<"[Obstacle Selector]: Rectangle Obstacles Removed!\n";
                state=Actions::DrawRectangle;
            }
            else
            {
                std::cout<<"[Obstacle Selector]: No Rectangle Obstacles Left\n";
                state=Actions::DrawRectangle; //reset state
            }
        }

        else if (state == Actions::ResetAll)
        {
            if (app_data_->circle_obs_array.array.size()>0)
            {
                app_data_->circle_obs_array.array.clear();
            }

            if (app_data_->rect_obs_array.array.size()>0)
            {
                app_data_->rect_obs_array.array.clear();
            }

            std::cout<<"[Obstacle Selector]: Obstacle selections cleared!\n";
            state=Actions::DrawCircle;

        }
        

        for (const auto& circ_obs : app_data_->circle_obs_array.array)
        {
            gui_window_selector_.draw(circ_obs.shape);
        }

        for (const auto& rect_obs : app_data_->rect_obs_array.array)
        {
            gui_window_selector_.draw(rect_obs.shape);
        }

        sf::RectangleShape rect_background;
        rect_background.setPosition(0,0);
        rect_background.setSize(sf::Vector2f(200,gui_window_selector_.getSize().y));
        rect_background.setFillColor(sf::Color::Yellow);
        gui_window_selector_.draw(rect_background);
        gui_backend_selector_.draw();
        gui_window_selector_.display();
        gui_window_selector_.clear(sf::Color::Black);
    }
}

void PlannerGUI::GenerateRandomContinuous()
{
    int successful_points=0;
    std::random_device rdx;
    std::random_device rdy;

    std::default_random_engine engx(rdx());
    std::default_random_engine engy(rdy());
    
    std::uniform_real_distribution<double> distrx(0,windowX-obs_size.x);
    std::uniform_real_distribution<double> distry(0,windowY-obs_size.y);
}
