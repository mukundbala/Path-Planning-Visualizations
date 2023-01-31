#ifndef RRTMarch_HPP
#define RRTMarch_HPP
#include <continuous_planner.hpp>
#include <random>
/*
The critical improvement to RRT that RRTMarch performs is to keep track of the success of the last known random sample of the goal
In sparse maps, RRTMarch can quickly converge to find a path that is as close as possible to the goal
*/
class RRTMarch:public ContinuousPlanner
{
private:
    std::string planner_name_;
    YAML::Node loader;
    double step_;
    double bias_;
    double goal_radius_;
    int max_iterations_;
    int current_iteration;
    double tree_size_;
    bool path_created_;
    bool march_; //this is the control variable to trigger marching behaviour
    std::vector<Vec2D> tree_;
    std::vector<Vec2D> parent_;
    std::vector<Vec2D> path_;
public:
    RRTMarch(std::shared_ptr<AppData> data);
    ~RRTMarch() override;
    std::string getName() override;
    void plan() override;
    std::vector<Vec2D> returnTree() override;
    std::vector<Vec2D> returnParent() override;
    std::vector<Vec2D> returnPath() override;
    int returnTreeSize() override;
    void updateDerivedPlannerTree() override;

    bool getMarch();
    Vec2D chooseRandomPoint();
    Vec2D getProximalNode(Vec2D &random_point);
    bool checkProximalNode(Vec2D &proximal_node, Vec2D &random_point);
    Vec2D getSteppedPoint(Vec2D &random_point, Vec2D &proximal_node);
    bool pointAlreadyChosen(Vec2D &pt);
    bool checkSteppedPoint(Vec2D &stepped_point, Vec2D& proximal_node);
    void recordNewNode(Vec2D &point_to_add, Vec2D& tree_node);
    bool goalReached(Vec2D &child_node);
    void generatePath();
};



#endif