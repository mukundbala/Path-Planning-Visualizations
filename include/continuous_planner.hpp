#ifndef CONTINUOUS_PLANNER_HPP
#define CONTINUOUS_PLANNER_HPP
#include <point.hpp>
#include <data.hpp>
enum class ContinuousPlannerState
{
    NotStarted,
    Planning,
    PathFound,
    PlanningComplete,
};

class ContinuousPlanner
{
protected:
ContinuousPlanner(std::shared_ptr<AppData> data); //protected contructor to prevent usage of this class
std::shared_ptr<AppData> data_; //containing data on obstacles, map and the chosen planner
Vec2D start_point_;
Vec2D end_point_;
ContinuousPlannerState planner_state_;
std::string chosen_planner_name_;
double control_pane_width_;
double map_x_;
double map_y_;
double EPS_;

public:
virtual ~ContinuousPlanner();
virtual void plan() = 0;
virtual std::string getName() = 0;
virtual std::vector<Vec2D> returnTree() = 0;
virtual std::vector<Vec2D> returnParent() = 0;
virtual std::vector<Vec2D> returnPath() = 0;
virtual int returnTreeSize() = 0;
virtual void updateDerivedPlannerTree() = 0;

void setStartPoint(Vec2D &start);
void setEndPoint(Vec2D &end);
void updateBasePlannerStartEnd(); //update from the data

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

ContinuousPlannerState returnPlannerState();
Vec2D getStartPoint()
{
    return start_point_;
}
Vec2D getEndPoint()
{
    return end_point_;
}
};
#endif //CONTINUOUS_PLANNER_HPP