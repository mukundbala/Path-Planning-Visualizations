#ifndef NODE_HPP
#define NODE_HPP
#include <iostream>
#include <point.hpp>
//used for discrete planners
class Node
{
private:
    int x_;
    int y_;
    int cost_;
    int idx_;
    Vec2D pos; //the global non-grid coordinates
public:
    Node();
    Node(int x, int y);
    Node(int x , int y , int cost);

    void setGridCoords(int x, int y);

    void setGlobalCoords(Vec2D& vec);

    void x(int x);

    void y(int y);

    void cost(int cost);

    void idx(int idx);

    int x();

    int y();

    int operator[](int idx);

    int cost();
    
    int idx();
    
    void print();
};
#endif //NODE_HPP