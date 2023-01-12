#ifndef RRT_HPP
#define RRT_HPP
#include <continuous_planner.hpp>
#include <random>
class RRT:public ContinuousPlanner
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
    std::vector<Vec2D> tree_;
    std::vector<Vec2D> parent_;
    std::vector<Vec2D> path_;
public:
    RRT(std::shared_ptr<AppData> data);
    ~RRT() override;
    std::string getName() override;
    void plan() override;
    std::vector<Vec2D> returnTree() override;
    std::vector<Vec2D> returnParent() override;
    std::vector<Vec2D> returnPath() override;
    int returnTreeSize() override;
    void updateDerivedPlannerTree() override;

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

#endif //RRT_HPP