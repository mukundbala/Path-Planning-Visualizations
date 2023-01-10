#include <continuous_planner.hpp>

ContinuousPlanner::ContinuousPlanner(std::shared_ptr<AppData> data, Vec2D &start_pt, Vec2D &end_pt)
{
    data_ = data;
    setStartPoint(start_pt);
    setEndPoint(end_pt);
    planner_state_ = ContinuousPlannerState::NotStarted;
    chosen_planner_name_ = data_->getChosenPlanner();
    control_pane_width = data->getControlPaneWidth();
    map_x_ = data_->getMapX();
    map_y_ = data_->getMapY();
    EPS_ = data_->getEPS();
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

ContinuousPlanner::~ContinuousPlanner()
{
    data_.reset();
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

bool ContinuousPlanner::pointInCircle(Vec2D &pt)
{
    if (data_->circle_obs_array.array.empty())
    {
        return false;
    }
    for (const auto& circObs:data_->circle_obs_array.array)
    {
        double origin_x = circObs.getOrigin().x;
        double origin_y = circObs.getOrigin().y;
        Vec2D origin(origin_x,origin_y);
        double dist_to_origin = pt.mag(origin);
        if (dist_to_origin <= (circObs.getRadius() + circObs.getTolerance() - EPS_))
        {
            return true;
        }
    }
    return false;
}

bool ContinuousPlanner::lineIntersectCircle(Vec2D &p1,Vec2D &p2)
{
    if (data_->circle_obs_array.array.empty())
    {
        return false;
    }

    Vec2D line_segment_midpoint ((p1.x() + p2.x()) / 2,(p1.y() + p2.y()) / 2);
    for (const auto& circObs:data_->circle_obs_array.array)
    {
        double origin_x = circObs.getOrigin().x;
        double origin_y = circObs.getOrigin().y;
        Vec2D origin(origin_x,origin_y);
        double dist_to_origin = line_segment_midpoint.mag(origin);
        if (dist_to_origin <= (circObs.getRadius() + circObs.getTolerance() - EPS_))
        {
            return true;
        }
    }
    return false;
}

bool ContinuousPlanner::doesLineCollideCircle(Vec2D &p1, Vec2D &p2)
{
    if (pointInCircle(p2))
    {
        return true;
    }
    if (lineIntersectCircle(p2,p1))
    {
        return true;
    }
    return false;
}
