#ifndef NODE_HPP
#define NODE_HPP
#include <iostream>
//used for discrete planners
class Node
{
private:
    int x_;
    int y_;
    int cost_;
public:
    Node();
    Node(int x, int y);
    Node(int x , int y , int cost);

    void setCoords(int x, int y);
    void x(int x);
    void y(int y);
    void cost(int cost);
    int x();
    int y();
    int operator[](int idx);
    int cost();
    void print();
};
#endif //NODE_HPP