#include <node.hpp>

Node::Node(): x_(-1) , y_(-1),cost_(-1), idx_(-1){};
Node::Node(int x, int y): x_(x) , y_(y) , cost_(-1),idx_(-1){};
Node::Node(int x, int y, double cost):x_(x) , y_(y) , cost_(cost),idx_(-1){};
Node::Node(int x, int y, double cost , int idx):x_(x) , y_(y) , cost_(cost),idx_(idx){};
void Node::x(int x)
{
    this->x_ = x;
}

void Node::y(int y)
{
    this->y_ = y;
}

void Node::cost(double cost)
{
    this->cost_ = cost;
}

void Node::idx(int idx)
{
    this->idx_ = idx;
}

void Node::setGridCoords(int x, int y)
{
    this->x_ = x;
    this->y_ = y;
}

void Node::setGlobalCoords(Vec2D &vec)
{
    this->pos_.setCoords(vec.x(),vec.y());
}

int Node::x()
{
    return this->x_;
}

int Node::y()
{
    return this->y_;
}

int Node::idx()
{
    return this->idx_;
}

double Node::cost()
{
    return this->cost_;
}

int Node::operator[](int idx)
{
    if (idx == 0)
    {
        return this->x_;
    }
    else if (idx == 1)
    {
        return this->y_;
    }
    else
    {
        return -1;
    }
}

Vec2D Node::getGlobalCoords()
{
    return this->pos_;
}

void Node::print()
{
    std::cout<<"(" << x_ << "," << y_ << ")";
}

void Node::clear()
{
    this->x_ = -1;
    this->y_ = -1;
    this->cost_ = -1;
    this->idx_ = -1;
    this->pos_.x(-1);
    this->pos_.y(-1);
}