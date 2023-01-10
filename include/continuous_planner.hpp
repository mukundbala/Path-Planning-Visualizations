#ifndef CONTINUOUS_PLANNER_HPP
#define CONTINUOUS_PLANNER_HPP
#include <point.hpp>
#include <data.hpp>
enum class ContinuousPlannerState
{
    NotStarted,
    Planning,
    PathFound,
};

class ContinuousPlanner
{
protected:
ContinuousPlanner(std::shared_ptr<AppData> data, Vec2D &start_pt, Vec2D &end_pt); //protected contructor to prevent usage of this class
std::shared_ptr<AppData> data_; //containing data on obstacles, map and the chosen planner
Vec2D start_point_;
Vec2D end_point_;
ContinuousPlannerState planner_state_;
std::string chosen_planner_name_;
double control_pane_width;
double map_x_;
double map_y_;
double EPS_;

public:
virtual ~ContinuousPlanner();
virtual void plan() = 0;

void setStartPoint(Vec2D &start);
void setEndPoint(Vec2D &end);

//tools to detect collision between line formed by any 2 points and rectangles
bool pointInRectangle(Vec2D &pt);
int orientation(Vec2D &p, Vec2D &q, Vec2D &r);
bool intersect(Vec2D &rect1, Vec2D &rect2, Vec2D &p1, Vec2D &p2);
bool lineIntersectRectangle(Vec2D &p2, Vec2D &p1);
bool doesLineCollideRectangle(Vec2D &p1, Vec2D &p2);

//tools to detect collision between line formed by any 2 points and circles
bool pointInCircle(Vec2D &pt);
bool lineIntersectCircle(Vec2D &p1, Vec2D &p2);
bool doesLineCollideCircle(Vec2D &p1, Vec2D &p2);

};
#endif //CONTINUOUS_PLANNER_HPP