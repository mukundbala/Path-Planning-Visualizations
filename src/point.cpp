#include "point.hpp"

Vec2D::Vec2D():x_(-1),y_(-1),cost_(-1),idx_(-1),EPS_(1e-6){}
Vec2D::Vec2D(double x_input, double y_input):x_(x_input),y_(y_input),cost_(-1),idx_(-1),EPS_(1e-6){}
Vec2D::Vec2D(double x_input, double y_input, double cost_input):x_(x_input),y_(y_input),cost_(cost_input),idx_(-1),EPS_(1e-6){}
Vec2D::Vec2D(double x_input, double y_input, double cost_input, int idx_input):x_(x_input),y_(y_input),cost_(cost_input),idx_(idx_input),EPS_(1e-6){};

double Vec2D::x()const
{
    return this->x_;
}

double Vec2D::y()const
{
    return this->y_;
}

double Vec2D::cost()const
{
    return this->cost_;
}

int Vec2D::idx()const
{
    return this->idx_;
}

double Vec2D::EPS()const
{
    return this->EPS_;
}

double Vec2D::operator [](int idx)const
{
    assert(idx == 0 || idx == 1);

    if (idx == 0)
    {
        return this->x_;
    }

    else
    {
        return this->y_;
    }
}

void Vec2D::setCoords(double x_,double y_)
{
    this->x_ = x_;
    this->y_ = y_;
}

void Vec2D::x(double x_)
{
    this->x_ = x_;
}

void Vec2D::y(double y_)
{
    this->y_ = y_;
}

void Vec2D::cost(double cost_)
{
    this->cost_ = cost_;
}

void Vec2D::idx(int idx_)
{
    this->idx_ = idx_;
}

bool Vec2D::operator ==(Vec2D& rhs)
{
    return (fabs(this->x_ - rhs.x())<this->EPS_ && fabs(this->y_ - rhs.y())<this->EPS_);
}

bool Vec2D::operator !=(Vec2D& rhs)
{
    return !(fabs(this->x_ - rhs.x())<this->EPS_ && fabs(this->y_ - rhs.y())<this->EPS_);
}

Vec2D Vec2D::operator+(Vec2D& rhs)const
{
    Vec2D my_vec;
    my_vec.setCoords(this->x_ + rhs.x(),this->y_ + rhs.y());
    return my_vec;
}

Vec2D Vec2D::operator-(Vec2D& rhs)const
{
    Vec2D my_vec;
    my_vec.setCoords(this->x_ - rhs.x(),this->y_ - rhs.y());
    return my_vec;
}

void Vec2D::operator+=(Vec2D& rhs)
{
    this->x_ += rhs.x();
    this->y_ += rhs.y();
}

void Vec2D::operator-=(Vec2D& rhs)
{
    this->x_ -= rhs.x();
    this->y_ -= rhs.y();
}

Vec2D Vec2D::operator +(double scalar)const
{
    Vec2D vec;
    vec.setCoords(this->x_ + scalar,this->y_ + scalar);
    return vec;
}

Vec2D Vec2D::operator -(double scalar)const
{
    Vec2D vec;
    vec.setCoords(this->x_ - scalar,this->y_ - scalar);
    return vec;
}

void Vec2D::operator+=(double scalar)
{
    this->x_ += scalar;
    this->y_ += scalar;
}

void Vec2D::operator-=(double scalar)
{
    this->x_ -= scalar;
    this->y_ -= scalar;
}

void Vec2D::operator++()
{
    this->x_ += 1;
    this->y_ += 1;
}

void Vec2D::operator--()
{
    this->x_ -= 1;
    this->y_ -= 1;
}

double Vec2D::mag()const
{
    double magnitude_of_line=sqrt(pow(this->x_,2)+pow(this->y_,2));
    return magnitude_of_line;
}

double Vec2D::mag(Vec2D& other_point)const
{
    double magnitude_of_line=sqrt(
                            pow(other_point.x()-this->x_,2)+
                            pow(other_point.y()-this->y_,2));
    return magnitude_of_line;
}

double Vec2D::dot(Vec2D& rhs)const
{
    double dot_product=(this->x_*rhs.x()) + (this->y_*rhs.y());
    return dot_product;
}

void Vec2D::normalize()
{
    this->x_ /= this->mag();
    this->y_ /= this->mag();
}

Vec2D Vec2D::normalize()const
{
    Vec2D vec;
    vec.setCoords(this->x_/this->mag(),this->y_/this->mag());
    return vec;
}

Vec2D Vec2D::SFV2fToVec2D(sf::Vector2f& vec)const
{
    Vec2D my_vec;
    my_vec.setCoords(vec.x,vec.y);
    return my_vec;
}

sf::Vector2f Vec2D::Vec2DToSFV2f()const
{
    sf::Vector2f my_vec;
    my_vec.x = this->x_;
    my_vec.y = this->y_;
    return my_vec;
}

void Vec2D::print()const
{
    std::cout<<"Coordinates:("<<this->x_<<","<<this->y_<<")\n";
    std::cout<<"Cost:"<<this->cost_<<"\n";
    std::cout<<"Index:"<<this->idx_<<"\n";
}

