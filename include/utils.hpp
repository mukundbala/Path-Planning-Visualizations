#ifndef UTILS_HPP
#define UTILS_HPP

#include <point.hpp>

namespace utils
{
    sf::Vector2f Vec2DToV2f(Vec2D &to_convert);

    Vec2D V2ftoV2d(sf::Vector2f &to_convert);
}

#endif //UTILS_HPP