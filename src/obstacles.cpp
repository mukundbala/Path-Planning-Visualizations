#include <obstacles.hpp>


RectangleObstacle::RectangleObstacle(sf::Vector2f size_input,sf::Vector2f position_input):size(size_input),position(position_input)
{
    origin.x = size.x / 2;
    origin.y = size.y / 2;
    obstacle_color = sf::Color::Red;
    shape.setFillColor(sf::Color::Red);
    shape.setSize(size);
    shape.setOrigin(origin);
    shape.setPosition(position);
    closeness_tolerance = 3.0;
    /*
    corners run clockwise, from 0 to 3, starting from bottom left
    * is the local origin of the obstacle (centre of the rectangle) 
        1 . . . 2  ---->+
        .       .  | axis
        .   *   .  |
        .       .  | 
        0 . . . 3  +
    */
    //actual corners
    Vec2D corner_0 (position.x - (size.x / 2) , position.y + (size.y / 2));
    Vec2D corner_1 (position.x - (size.x / 2) , position.y - (size.y / 2));
    Vec2D corner_2 (position.x + (size.x / 2) , position.y - (size.y / 2));
    Vec2D corner_3 (position.x + (size.x / 2) , position.y + (size.y / 2));
    corners = {corner_0,corner_1,corner_2,corner_3};

    //adjust_corners
    Vec2D adj_corner_0 (corners[0].x() - closeness_tolerance , corners[0].y() + closeness_tolerance);
    Vec2D adj_corner_1 (corners[1].x() - closeness_tolerance , corners[1].y() - closeness_tolerance);
    Vec2D adj_corner_2 (corners[2].x() + closeness_tolerance , corners[2].y() - closeness_tolerance);
    Vec2D adj_corner_3 (corners[3].x() + closeness_tolerance , corners[3].y() + closeness_tolerance);
    toleranced_corners = {adj_corner_0,adj_corner_1,adj_corner_2,adj_corner_3};

    obstacle_type="rectangle";
}

RectangleObstacle::RectangleObstacle(sf::Vector2f size_input,Vec2D position_input):size(size_input)
{
    position = position_input.Vec2DToSFV2f();
    origin.x = size.x / 2;
    origin.y = size.y / 2;
    obstacle_color = sf::Color::Red;
    shape.setFillColor(sf::Color::Red);
    shape.setSize(size);
    shape.setOrigin(origin);
    shape.setPosition(position);
    closeness_tolerance = 3.0;

    /*
    corners run clockwise, from 0 to 3, starting from bottom left
    * is the local origin of the obstacle (centre of the rectangle) 
        1 . . . 2  ---->+
        .       .  | axis
        .   *   .  |
        .       .  | 
        0 . . . 3  +
    */
    //actual corners
    Vec2D corner_0 (position.x - size.x / 2,position.y + size.y / 2);
    Vec2D corner_1 (position.x - size.x / 2,position.y - size.y / 2);
    Vec2D corner_2 (position.x + size.x / 2,position.y - size.y / 2);
    Vec2D corner_3 (position.x + size.x / 2,position.y + size.y / 2);
    corners={corner_0,corner_1,corner_2,corner_3};

    //adjust_corners
    Vec2D adj_corner_0 (position.x - size.x / 2 - closeness_tolerance,position.y + size.y / 2 + closeness_tolerance);
    Vec2D adj_corner_1 (position.x - size.x / 2 - closeness_tolerance,position.y - size.y / 2 - closeness_tolerance);
    Vec2D adj_corner_2 (position.x + size.x / 2 + closeness_tolerance,position.y - size.y / 2 - closeness_tolerance);
    Vec2D adj_corner_3 (position.x + size.x / 2 + closeness_tolerance,position.y + size.y / 2 + closeness_tolerance);
    toleranced_corners={adj_corner_0,adj_corner_1,adj_corner_2,adj_corner_3};

    obstacle_type="rectangle";
}

sf::Vector2f RectangleObstacle::getOrigin()const
{
    return this->origin;
}

sf::Vector2f RectangleObstacle::getPosition()const
{
    return this->position;
}

sf::Vector2f RectangleObstacle::getSize()const
{
    return this->size;
}

double RectangleObstacle::getTolerance()const
{
    return closeness_tolerance;
}

std::string RectangleObstacle::getType()const
{
    return this->obstacle_type;
}

CircleObstacle::CircleObstacle(double radius_input,sf::Vector2f position_input):radius(radius_input),position(position_input)
{
    shape.setFillColor(obstacle_color);

    shape.setRadius(radius);

    shape.setOrigin(radius,radius);
    
    shape.setPosition(position);

    obstacle_type = "circle";

    closeness_tolerance = 3.0;

    adjusted_radius = radius + (closeness_tolerance);
}

CircleObstacle::CircleObstacle(double radius_input,Vec2D position_input):radius(radius_input)
{
    position = position_input.Vec2DToSFV2f();

    shape.setFillColor(obstacle_color);

    shape.setRadius(radius);

    shape.setOrigin(radius,radius);
    
    shape.setPosition(position);

    obstacle_type = "circle";

    closeness_tolerance = 3.0;

    adjusted_radius = radius + (closeness_tolerance);
}

sf::Vector2f CircleObstacle::getOrigin()const
{
    return this->origin;
}

sf::Vector2f CircleObstacle::getPosition()const
{
    return this->position;
}

double CircleObstacle::getRadius()const
{
    return this->radius;
}

double CircleObstacle::getTolerance()const
{
    return this->closeness_tolerance;
}

double CircleObstacle::getAdjustedRadius()const
{
    return this->adjusted_radius;
}

std::string CircleObstacle::getType()const
{
    return this->obstacle_type;
}


