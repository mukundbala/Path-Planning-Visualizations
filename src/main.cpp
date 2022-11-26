#include <iostream>
#include <point.hpp>
#include <obstacles.hpp>

int main() {
    sf::RenderWindow test_window(sf::VideoMode(1480,1000),"TestWindow");
    RectangleObstacle obs1(sf::Vector2f(50,50),sf::Vector2f(500,500));
    CircleObstacle obs2(50,sf::Vector2f(800,800));
    CircleObstacle obs3(30,sf::Vector2f(600,700));
    CircleObstacle obs4(90,sf::Vector2f(500,300));
    ObstacleArray<CircleObstacle,std::vector> circ_array;
    circ_array.array.push_back(obs2);
    circ_array.array.push_back(obs3);
    circ_array.array.push_back(obs4);

    while(test_window.isOpen())
    {
        sf::Event event;
        while(test_window.pollEvent(event))
        {
            if (event.type==sf::Event::Closed)
            {
                test_window.close();
            }
        }
        
        test_window.clear(sf::Color::Black);
        test_window.draw(obs1.shape);
        for (auto shape:circ_array.array)
        {
            test_window.draw(shape.shape);
        }
        test_window.display();
    }
}