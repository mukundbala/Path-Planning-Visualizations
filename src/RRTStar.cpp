#include <RRTStar.hpp>

RRTStar::RRTStar(std::shared_ptr<AppData> data):ContinuousPlanner(data)
{
    planner_name_ = "RRTStar";
    std::string config_dir = "config";
    std::string dirname = config_dir+"/"+planner_name_+".yaml";
    loader = YAML::LoadFile(dirname);
    step_ = loader["step"].as<double>();
    bias_ = loader["bias"].as<double>();
    rewire_radius_ = loader["rewire_radius"].as<double>();
    rewire_threshold_ = loader["rewire_threshold"].as<double>();
    goal_radius_ = loader["goal_radius"].as<double>();
    max_iterations_ = loader["max_iterations"].as<int>();
    current_iteration = 1;
    tree_size_ = 0;
    path_created_ = 0;
    tree_.reserve(100);
    parent_.reserve(100);
    path_.reserve(100);
    std::cout<<"[RRTStar]: RRTStar Planner Initialized\n";
}

RRTStar::~RRTStar()
{
    loader.~Node();
    tree_.clear();
    path_.clear();
    parent_.clear();
    std::cout<<"[RRTStar]: Halting RRT planner\n";
}

std::string RRTStar::getName()
{
    return planner_name_;
}

std::vector<Vec2D> RRTStar::returnTree()
{
    return this->tree_;
}

std::vector<Vec2D> RRTStar::returnParent()
{
    return parent_;
}

std::vector<Vec2D> RRTStar::returnPath()
{
    return path_;
}

int RRTStar::returnTreeSize()
{
    return tree_size_;
}

void RRTStar::updateDerivedPlannerTree()
{
    start_point_.cost(0);
    tree_.emplace_back(start_point_);
    Vec2D dummy(-2,-2,-2,-2);
    parent_.emplace_back(dummy);
    tree_size_ ++;
    this->planner_state_ = ContinuousPlannerState::Planning;
}

Vec2D RRTStar::chooseRandomPoint()
{
    int rand_num=1 + (rand() % 100);
    int limit=100*bias_;
    if (rand_num<=limit){
        return end_point_; //return the nedpoint as the randomly selected point
    }
    double buffer = 5;
    std::random_device rdx;
    std::random_device rdy;

    std::default_random_engine engx(rdx());
    std::default_random_engine engy(rdy());
    
    std::uniform_real_distribution<double> distrx(control_pane_width_+ buffer,map_x_ + control_pane_width_ - buffer);
    std::uniform_real_distribution<double> distry(0 + buffer,map_y_ - buffer);

    double randx=distrx(engx);
    double randy=distry(engy);
    Vec2D random_point(randx,randy);
    return random_point;
}

Vec2D RRTStar::getProximalNode(Vec2D &random_point){ 
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

bool RRTStar::checkProximalNode(Vec2D &proximal_node, Vec2D& random_point){
    if (proximal_node == random_point){ //prevent nan propagation
        return false; //fail
    }
    return true;
}

Vec2D RRTStar::getSteppedPoint(Vec2D &random_point, Vec2D&proximal_node){
    Vec2D distvec = random_point-proximal_node; //vector pointing from proximal node to random point
    if (distvec.mag() < step_)
    {
        return random_point;
    }
    distvec.normalize();
    distvec *= step_; //step direction vector
    Vec2D stepped_point = proximal_node + distvec; //computing the point where the stepped point lies
    return stepped_point;
}

Vec2D RRTStar::optimizeProximalNode(Vec2D &stepped_point, Vec2D &proximal_node)
{
    double current_cost = proximal_node.cost();
    Vec2D improved_proximal_node = proximal_node;

    for (size_t i = 0 ; i<tree_size_ ; ++i)
    {
        if (tree_.at(i) == end_point_ || tree_.at(i) == proximal_node)
        {
            continue;
        }

        Vec2D tree_node = tree_.at(i);
        double dist_from_stepped = stepped_point.mag(tree_node);
        if ( (dist_from_stepped - EPS_ < rewire_radius_) && (tree_node.cost() < current_cost) )
        {
            current_cost = tree_node.cost();
            improved_proximal_node = tree_node;
        }
    }
    return improved_proximal_node;
}

bool RRTStar::pointAlreadyChosen(Vec2D &pt)
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

bool RRTStar::checkSteppedPoint(Vec2D &stepped_point, Vec2D& proximal_node)
{
    bool already_chosen_point = pointAlreadyChosen(stepped_point);
    if (already_chosen_point)
    {
        std::cout<<"[RRTStar]: Stepped Point already chosen\n";
        return false;
    }
    bool collides_with_rectangle_obstacles = doesLineCollideRectangle(proximal_node,stepped_point);
    if (collides_with_rectangle_obstacles)
    {
        std::cout<<"[RRTStar]: Stepped Point collides with rectangle\n";
        stepped_point.print();
        return false;
    }
    bool collides_with_circle_obstacles = doesLineCollideCircle(proximal_node,stepped_point);
    if (collides_with_circle_obstacles)
    {
        std::cout<<"[RRTStar]: Stepped Point collides with circle\n";
        return false;
    }
    return true;
}

void RRTStar::recordNewNode(Vec2D &point_to_add, Vec2D& tree_node)
{
    if (point_to_add == end_point_)
    {
        std::cout<<"[RRT]: Adding end point now. Path has been found\n";
    }
    point_to_add.idx(tree_size_);
    double distance = tree_node.mag(point_to_add);
    double cost = tree_node.cost() + distance;
    point_to_add.cost(cost);
    tree_.emplace_back(point_to_add);
    parent_.emplace_back(tree_node);
    tree_size_++;
}

bool RRTStar::goalReached(Vec2D &child_node)
{
    double dist_to_goal=child_node.mag(this->end_point_);
    bool within_goal_radius = fabs(dist_to_goal-EPS_) <= goal_radius_;
    if (within_goal_radius)
    {
        std::cout<<"[RRT]: Goal Reached!\n";
    }
    return within_goal_radius;
}

void RRTStar::generatePath()
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

void RRTStar::rewire(Vec2D &latest_tree_node)
{
    for (size_t idx = 0 ; idx<tree_size_; ++idx)
    {
        Vec2D tree_node = tree_.at(idx);
        if (tree_node == latest_tree_node)
        {
            continue;
        }
        double current_cost = tree_node.cost();
        double distance = tree_node.mag(latest_tree_node);
        if (distance - EPS_ <= rewire_radius_)
        {
            double new_cost = latest_tree_node.cost() + distance;
            double cost_improvement = current_cost - new_cost - EPS_;
            if (cost_improvement > rewire_threshold_)
            {
                tree_node.cost(new_cost);
                tree_.at(idx) = tree_node;
                parent_.at(idx) = latest_tree_node;
                std::cout<<"[RRTStar]: Rewiring Successful\n";
            }
        }
    }
}
void RRTStar::plan()
{
    if (current_iteration == 1)
    {
        std::cout<<"Start Point: \n"; start_point_.print();
        std::cout<<"End PointL \n"; end_point_.print();
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
        Vec2D random_point = chooseRandomPoint();
        Vec2D proximal_node = getProximalNode(random_point);
        bool is_proximal_node_ok = checkProximalNode(proximal_node,random_point);
        if (!is_proximal_node_ok)
        {
            current_iteration++;
            std::cout<<"[RRTStar]: Proximal Node failed\n";
            std::cout<<"#########End Iteration: "<<current_iteration<<"#########\n";
            return; //point fails
        }
        Vec2D stepped_point = getSteppedPoint(random_point,proximal_node);
        Vec2D optimized_proximal_node = optimizeProximalNode(stepped_point,proximal_node);
        bool is_stepped_point_ok = checkSteppedPoint(stepped_point,optimized_proximal_node);
        if (!is_stepped_point_ok)
        {
            current_iteration++;
            std::cout<<"[RRTStar]: Stepped Point failed\n";
            std::cout<<"#########End Iteration: "<<current_iteration<<"#########\n";
            return;
        }
        recordNewNode(stepped_point,optimized_proximal_node);
        rewire(stepped_point);
        bool has_goal_been_reached = goalReached(stepped_point);
        if (has_goal_been_reached)
        {
            recordNewNode(end_point_ , stepped_point);
            this->planner_state_ = ContinuousPlannerState::PathFound;
            generatePath();
            this->path_created_ = true;
        }
        current_iteration++;
    }

    else if (planner_state_ == ContinuousPlannerState::PathFound)
    {
        std::cout<<"#########Current Iteration: "<<current_iteration<<"#########\n";
        std::cout<<"[RRTStar]: Improving Path\n";
        if (current_iteration > max_iterations_)
        {
            planner_state_ = ContinuousPlannerState::PlanningComplete;
            return;
        }
        Vec2D random_point = chooseRandomPoint();
        Vec2D proximal_node = getProximalNode(random_point);
        bool is_proximal_node_ok = checkProximalNode(proximal_node,random_point);
        if (!is_proximal_node_ok)
        {
            current_iteration++;
            std::cout<<"[RRTStar]: Proximal Node failed\n";
            std::cout<<"#########End Iteration: "<<current_iteration<<"#########\n";
            return; //point fails
        }
        Vec2D stepped_point = getSteppedPoint(random_point,proximal_node);
        Vec2D optimized_proximal_node = optimizeProximalNode(stepped_point,proximal_node);
        bool is_stepped_point_ok = checkSteppedPoint(stepped_point,optimized_proximal_node);
        if (!is_stepped_point_ok)
        {
            current_iteration++;
            std::cout<<"[RRTStar]: Stepped Point failed\n";
            std::cout<<"#########End Iteration: "<<current_iteration<<"#########\n";
            return;
        }
        recordNewNode(stepped_point,optimized_proximal_node);
        rewire(stepped_point);
        generatePath();
        current_iteration++;
    }

    else if (planner_state_ == ContinuousPlannerState::PlanningComplete)
    {
        if (!path_created_)
        {
            std::cout<<"[RRTStar]: Path has not been reached!\n";
        }
        return;
    }
    
}