#include <RRTMarch.hpp>

RRTMarch::RRTMarch(std::shared_ptr<AppData> data):ContinuousPlanner(data)
{
    planner_name_ = "RRTMarch";
    std::string config_dir = "config";
    std::string dirname = config_dir+"/"+planner_name_+".yaml";
    loader = YAML::LoadFile(dirname);
    step_ = loader["step"].as<double>();
    bias_ = loader["bias"].as<double>();
    goal_radius_ = loader["goal_radius"].as<double>();
    max_iterations_ = loader["max_iterations"].as<int>();
    current_iteration = 1;
    tree_size_ = 0;
    path_created_ = 0;
    march_ = false;
    tree_.reserve(100);
    parent_.reserve(100);
    path_.reserve(100);
    std::cout<<"[RRT]: RRT Planner Initialized\n";
}

RRTMarch::~RRTMarch()
{
    loader.~Node();
    tree_.clear();
    path_.clear();
    parent_.clear();
    std::cout<<"[RRT]: Halting RRT planner\n";
}

std::string RRTMarch::getName()
{
    return planner_name_;
}

bool RRTMarch::getMarch()
{
    return march_;
}

void RRTMarch::plan()
{
    //sf::sleep(sf::seconds(3)); //for debugging 

    if (current_iteration == 1)
    {
        std::cout<<"Start Point: \n"; start_point_.print();
        std::cout<<"End Point: \n"; end_point_.print();
    }
    if (planner_state_ == ContinuousPlannerState::NotStarted)
    {
        return; //safeguard
    }

    else if (planner_state_ == ContinuousPlannerState::Planning)
    {
        std::cout<<"#########Current Iteration: "<<current_iteration<<"#########\n";
        if (current_iteration > max_iterations_)
        {
            planner_state_ = ContinuousPlannerState::PlanningComplete;
            return;
        }
        Vec2D random_point = march_ ? end_point_ : chooseRandomPoint(); //this means that if march_ is true, we will always pick the endpoint
        Vec2D proximal_node = getProximalNode(random_point);
        bool is_proximal_node_ok = checkProximalNode(proximal_node,random_point);
        if (!is_proximal_node_ok)
        {
            current_iteration++;
            std::cout<<"[RRT]: Proximal Node failed\n";
            return; //point fails
        }
        Vec2D stepped_point = getSteppedPoint(random_point,proximal_node);
        bool is_stepped_point_ok = checkSteppedPoint(stepped_point,proximal_node);
        if (!is_stepped_point_ok)
        {
            current_iteration++;
            march_ = false;
            std::cout<<"[RRT]: Stepped Point failed\n";
            return;
        }
        
        march_ = random_point == end_point_ ? true : false; 
        recordNewNode(stepped_point,proximal_node);
        bool has_goal_been_reached = goalReached(stepped_point);
        if (has_goal_been_reached)
        {
            recordNewNode(end_point_ , stepped_point);
            this->planner_state_ = ContinuousPlannerState::PathFound;
            generatePath();
            this->path_created_ = true;
        }
        current_iteration ++;
    }

    else if (planner_state_ == ContinuousPlannerState::PlanningComplete)
    {
        if (!path_created_)
        {
            std::cout<<"[RRT]: Goal has not been reached!\n";
        }
        return; //planning is complete
    }

}

Vec2D RRTMarch::chooseRandomPoint()
{
    int rand_num=1 + (rand() % 100);
    int limit=100*bias_;
    if (rand_num<=limit){
        return end_point_; //return the endpoint as the randomly selected point
    }
    double buffer = 5.0;
    std::random_device rdx;
    std::random_device rdy;

    std::default_random_engine engx(rdx());
    std::default_random_engine engy(rdy());
    
    std::uniform_real_distribution<double> distrx(control_pane_width_ + buffer,map_x_ + control_pane_width_ - buffer);
    std::uniform_real_distribution<double> distry(0 + buffer,map_y_ - buffer);

    double randx=distrx(engx);
    double randy=distry(engy);
    Vec2D random_point(randx,randy);
    return random_point;
}

Vec2D RRTMarch::getProximalNode(Vec2D &random_point){ 
    double distMax=1e10;
    Vec2D proximalNode;
    for (int i=0;i<tree_size_;++i){
        double mag=tree_[i].mag(random_point);
        if (mag<distMax){
            distMax=mag;
            proximalNode=tree_[i];
        }
    }
    return proximalNode;
}

bool RRTMarch::checkProximalNode(Vec2D &proximal_node, Vec2D& random_point){
    if (proximal_node == random_point){ //prevent nan propagation
        return false; //fail
    }
    return true;
}

Vec2D RRTMarch::getSteppedPoint(Vec2D &random_point, Vec2D&proximal_node){
    Vec2D distvec=random_point-proximal_node; //vector pointing from proximal node to random point
    if (distvec.mag() < step_)
    {
        return random_point;
    }
    distvec.normalize();
    distvec*=step_; //step direction vector
    Vec2D stepped_point=proximal_node+distvec; //computing the point where the stepped point lies
    return stepped_point;
}

bool RRTMarch::pointAlreadyChosen(Vec2D &pt)
{
    for (auto& node : tree_)
    {
        if (node == pt)
        {
            return true;
        }
    }
    return false;
}

bool RRTMarch::checkSteppedPoint(Vec2D &stepped_point, Vec2D& proximal_node)
{
    bool already_chosen_point = pointAlreadyChosen(stepped_point);
    if (already_chosen_point)
    {
        std::cout<<"[RRT]: Stepped Point already chosen\n";
        return false;
    }
    bool collides_with_rectangle_obstacles = doesLineCollideRectangle(proximal_node,stepped_point);
    if (collides_with_rectangle_obstacles)
    {
        std::cout<<"[RRT]: Stepped Point collides with rectangle\n";
        stepped_point.print();
        return false;
    }
    bool collides_with_circle_obstacles = doesLineCollideCircle(proximal_node,stepped_point);
    if (collides_with_circle_obstacles)
    {
        std::cout<<"[RRT]: Stepped Point collides with circle\n";
        return false;
    }
    return true;
}

void RRTMarch::recordNewNode(Vec2D &point_to_add, Vec2D& tree_node)
{
    if (point_to_add == end_point_)
    {
        std::cout<<"[RRT]: Adding end point now. Path has been found\n";
    }
    point_to_add.idx(tree_size_);
    tree_.emplace_back(point_to_add);
    parent_.emplace_back(tree_node);
    tree_size_++;
}

bool RRTMarch::goalReached(Vec2D &child_node)
{
    double dist_to_goal=child_node.mag(this->end_point_);
    bool within_goal_radius = fabs(dist_to_goal-EPS_) <= goal_radius_;
    if (within_goal_radius)
    {
        std::cout<<"[RRT]: Goal Reached!\n";
    }
    return within_goal_radius;
}

void RRTMarch::generatePath()
{
    int curr=end_point_.idx(); //start from the EndPoint idx
    path_.clear();
    while(parent_[curr].idx()!=0){
        path_.emplace_back(tree_[curr]);
        curr=parent_[curr].idx();
    }
    path_.emplace_back(tree_[0]);
    std::reverse(path_.begin(),path_.end());
}

std::vector<Vec2D> RRTMarch::returnTree()
{
    return this->tree_;
}

std::vector<Vec2D> RRTMarch::returnParent()
{
    return parent_;
}

std::vector<Vec2D> RRTMarch::returnPath()
{
    return path_;
}

int RRTMarch::returnTreeSize()
{
    return tree_size_;
}

void RRTMarch::updateDerivedPlannerTree()
{
    tree_.emplace_back(start_point_); //bruh this works
    Vec2D dummy(-2,-2,-2,-2);
    parent_.emplace_back(dummy);
    tree_size_ ++;
    this->planner_state_ = ContinuousPlannerState::Planning;
}
