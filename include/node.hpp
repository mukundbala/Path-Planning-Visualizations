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
    double cost_;
    int idx_;
    Vec2D pos_; //the global non-grid coordinates
public:
    Node();
    Node(int x, int y);
    Node(int x , int y , double cost);
    Node(int x, int y, double cost , int idx);
    void setGridCoords(int x, int y);

    void setGlobalCoords(Vec2D& vec);

    void x(int x);

    void y(int y);

    void cost(double cost);

    void idx(int idx);

    int x();

    int y();

    int operator[](int idx);

    double cost();
    
    int idx();

    Vec2D getGlobalCoords();

    void print();

    void clear();
};
#endif //NODE_HPP