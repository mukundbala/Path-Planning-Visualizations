#include <continuous_planner.hpp>

ContinuousPlanner::ContinuousPlanner(std::shared_ptr<AppData> data)
{
    data_ = data;
    start_point_ = data_->getStartPoint();
    end_point_ = data_->getEndPoint();
    planner_state_string ={"NotStarted","Planning", "PathFound", "PlanningComplete"};
    planner_state_ = ContinuousPlannerState::NotStarted;
    chosen_planner_name_ = data_->getChosenPlanner();
    control_pane_width_ = data->getControlPaneWidth();
    map_x_ = data_->getMapX();
    map_y_ = data_->getMapY();
    EPS_ = data_->getEPS();
    std::cout<<"[ContinuousPlanner]: Base Planner Initialized!\n";
}

ContinuousPlanner::~ContinuousPlanner()
{
    data_.reset();
    std::cout<<"[ContinuousPlanner]: Halting Base Planner\n";
}

void ContinuousPlanner::setStartPoint(Vec2D &start)
{
    this->start_point_.setCoords(start.x(),start.y());
    this->start_point_.cost(0);
    this->start_point_.idx(0);
}

void ContinuousPlanner::setEndPoint(Vec2D &end)
{
    this->end_point_.setCoords(end.x(),end.y());
}

void ContinuousPlanner::updateBasePlannerStartEnd()
{
    this->start_point_.setCoords(data_->getStartPoint().x(),data_->getStartPoint().y());
    this->start_point_.idx(0);
    this->start_point_.cost(0);

    this->end_point_.setCoords(data_->getEndPoint().x(),data_->getEndPoint().y());
}

bool ContinuousPlanner::pointInRectangle(Vec2D &pt)
{
    if (data_->rect_obs_array.array.empty())
    {
        return false;
    }
    for (const auto& obs:data_->rect_obs_array.array)
    {
        if (pt.x() >= obs.toleranced_corners[0].x() &&
            pt.x() <= obs.toleranced_corners[3].x() &&
            pt.y() >= obs.toleranced_corners[1].y() &&
            pt.y()<= obs.toleranced_corners[0].y())
            {
                return true;
            }
    }
    return false;
}

int ContinuousPlanner::orientation(Vec2D &p, Vec2D &q, Vec2D &r)
{
    int val=(q.y()-p.y())*(r.x()-q.x()) - (q.x()-p.x()) * (r.y()-q.y());
    if (fabs(val-0)<EPS_)
    {
        return 0;
    }
    else
    {
        return (val>0)?1:2;
    } 
}

bool ContinuousPlanner::intersect(Vec2D &rect1, Vec2D &rect2, Vec2D &p1, Vec2D &p2)
{
    int o1=orientation(rect1,rect2,p1);
    int o2=orientation(rect1,rect2,p2);
    int o3=orientation(p1,p2,rect1);
    int o4=orientation(p1,p2,rect2);
    if (o1!=o2 && o3!=o4){
        return true;
    }
    return false;
}

bool ContinuousPlanner::lineIntersectRectangle(Vec2D &p2 , Vec2D &p1)
{
    if (data_->rect_obs_array.array.empty())
    {
        return false;
    }
    for (auto &rectObs : data_->rect_obs_array.array)
    {
        int intersection_count = intersect(rectObs.toleranced_corners[1],rectObs.toleranced_corners[0],p1, p2)+
                                 intersect(rectObs.toleranced_corners[0],rectObs.toleranced_corners[3],p1, p2)+
                                 intersect(rectObs.toleranced_corners[1],rectObs.toleranced_corners[2],p1, p2)+
                                 intersect(rectObs.toleranced_corners[2],rectObs.toleranced_corners[3],p1, p2);
        if (intersection_count > 0)
        {
            return true;
        }
    }
    return false;
}

bool ContinuousPlanner::doesLineCollideRectangle(Vec2D &p1, Vec2D &p2)
{
    //we only check p2 because it is the new point not part of the tree
    if (pointInRectangle(p2))
    {
        return true;
    }
    if (lineIntersectRectangle(p2,p1))
    {
        return true;
    }
    return false;
}

bool ContinuousPlanner::doesLineCollideCircle(Vec2D &tree_node, Vec2D &stepped_point) //tree , stepped
{
    if (data_->circle_obs_array.array.empty())
    {
        return false; //no circles to collides
    }

    for (auto &circle : data_->circle_obs_array.array)
    {
        sf::Vector2f circle_centre_V2f = circle.getPosition();
        Vec2D circle_centre_V2D(circle_centre_V2f.x,circle_centre_V2f.y);
        double adjusted_radius = circle.getAdjustedRadius();

        double dist_to_centre_p1 = tree_node.mag(circle_centre_V2f) - EPS_;
        double dist_to_centre_p2 = stepped_point.mag(circle_centre_V2f) - EPS_;

        if (dist_to_centre_p1 <= adjusted_radius || dist_to_centre_p2 <= adjusted_radius)
        {
            return true; //either piercing or impaling collision. If this check passes, all other cases are NoCollision OR ThroughandThrough Collisions
        }

        Vec2D tree_to_centre = circle_centre_V2D - tree_node;
        Vec2D tree_to_stepped = stepped_point - tree_node;
        double projection_k = tree_node.dot(stepped_point) / tree_to_stepped.dot(tree_to_stepped);
        Vec2D projected_point;
        projected_point.x(tree_to_stepped.x() * projection_k);
        projected_point.y(tree_to_stepped.y( )* projection_k);
        Vec2D tree_to_projected = projected_point - tree_node;

        double k = fabs(tree_to_stepped.x()) > fabs(tree_to_stepped.y()) ? tree_to_projected.x() / tree_to_stepped.x() : tree_to_projected.y() / tree_to_stepped.y();
        
        double dist = 0;
        if (k <= 0.0)
        {
            dist = sqrt(tree_to_centre.dot(tree_to_centre));
        }
        else if (k >= 1.0)
        {
            Vec2D stepped_to_centre = circle_centre_V2D - stepped_point;
            dist = sqrt(stepped_to_centre.dot(stepped_to_centre));
        }
        else
        {
            Vec2D projected_to_centre = circle_centre_V2D - projected_point;
            dist = sqrt(projected_to_centre.dot(projected_to_centre));
        }

        if (dist <= adjusted_radius)
        {
            return true;
        }
    }
    return false;
}

ContinuousPlannerState ContinuousPlanner::returnPlannerState(bool verbose)
{
    if (verbose)
    {
        std::cout<<"[PlannerState]: "<<this->planner_state_string[static_cast<unsigned short>(this->planner_state_)];
    }
    return planner_state_;
}

Vec2D ContinuousPlanner::getStartPoint()
{
    return start_point_;
}

Vec2D ContinuousPlanner::getEndPoint()
{
    return end_point_;
}
