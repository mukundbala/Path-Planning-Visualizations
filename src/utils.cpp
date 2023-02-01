#include <utils.hpp>

sf::Vector2f utils::Vec2DToV2f(Vec2D &to_convert)
{
    sf::Vector2f converted;
    converted.x = to_convert.x();
    converted.y = to_convert.y();
    return converted;
}

Vec2D utils::V2ftoV2d(sf::Vector2f &to_convert)
{
    Vec2D converted;
    converted.x(to_convert.x);
    converted.y(to_convert.y);
    return converted;
}