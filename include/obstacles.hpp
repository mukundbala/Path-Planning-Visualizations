#ifndef OBSTACLES_HPP
#define OBSTACLES_HPP
#include <point.hpp>
#include <vector>
#include <SFML/Graphics.hpp>
struct RectangleObstacle
{

private:

    sf::Vector2f origin; //origin set to the local centre of the rectangle
    
    sf::Vector2f size; //l*b dimensions of the obstacles.
    
    sf::Vector2f position;
  
    double closeness_tolerance;
    
    sf::Color obstacle_color;
    
    std::string obstacle_type;

public:

    sf::RectangleShape shape;

    std::vector<Vec2D> corners;
    
    std::vector<Vec2D> toleranced_corners;
    
    RectangleObstacle(sf::Vector2f size_input,sf::Vector2f position_input);
    
    RectangleObstacle(sf::Vector2f size_input,Vec2D position_input);
    
    sf::Vector2f getOrigin()const;

    sf::Vector2f getPosition()const;

    sf::Vector2f getSize()const;

    double getTolerance()const;

    std::string getType()const;

};

struct CircleObstacle
{

private:

    sf::Vector2f origin;
    
    double radius;

    sf::Vector2f position;
    
    double closeness_tolerance;

    sf::Color obstacle_color=sf::Color::Red;
    
    std::string obstacle_type;

public:

    sf::CircleShape shape;

    CircleObstacle(double radius_input,sf::Vector2f position_input);
    
    CircleObstacle(double radius_input,Vec2D position_input);

    sf::Vector2f getOrigin()const;

    sf::Vector2f getPosition()const;

    double getRadius()const;

    double getTolerance()const;

    std::string getType()const;

};

template<typename T, template <typename, typename> class Container>
class ObstacleArray
{   
public:
    Container<T,std::allocator<T>> array;
};

#endif //OBSTACLES_HPP