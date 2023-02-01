#ifndef POINT_HPP
#define POINT_HPP
#include <string>
#include <cassert>
#include <math.h>
#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>


class Vec2D
{
    // Class to represent points and lines in the 2D coordinate system of SFML
    // Origin of SFML is in the top left screen corner, +ve right and down
private:
    double x_;
    double y_;
    double cost_;
    int idx_;
    double EPS_;

public:
   Vec2D();
   Vec2D(double x_input, double y_input);
   Vec2D(double x_input, double y_input, double cost_input);
   Vec2D(double x_input, double y_input, double cost_input, int idx_input);

   double x()const;
   double y()const;
   double operator[](int idx)const;
   double cost()const;
   int idx()const;
   double EPS()const;

   void setCoords(double x_,double y_);
   void x(double x_);
   void y(double y_);
   void cost(double cost_);
   void idx(int idx_);

   bool operator == (Vec2D& rhs);
   bool operator != (Vec2D& rhs);

   Vec2D operator + (Vec2D& rhs)const;
   Vec2D operator - (Vec2D& rhs)const;

   Vec2D operator * (double scalar)const;
   Vec2D operator / (double scalar)const;
   
   void operator += (Vec2D& rhs);
   void operator -= (Vec2D& rhs);
   
   Vec2D operator + (double scalar)const;
   Vec2D operator - (double scalar)const;
   
   void operator += (double scalar);
   void operator -= (double scalar);
   
   void operator ++();
   void operator --();

   void operator *= (double scalar);

   double mag()const;
   double mag(Vec2D& other_point)const;
   double mag(sf::Vector2f& other_point)const;

   void normalize();
   Vec2D normalize()const;
   
   double dot (Vec2D& rhs)const;

   Vec2D SFV2fToVec2D(sf::Vector2f& vec)const;
   sf::Vector2f Vec2DToSFV2f()const;

   /*
    Function tools to convert from continuous space <->grid space
   */

    Vec2D ContinuousSpaceToGridSpace(const std::pair<int,int>& rez,double x_max, double y_max,double x_min,double y_min)const;

    Vec2D GridSpaceToContinuousSpace(const std::pair<int,int>& rez, double x_min_cont, double y_min_cont)const;

   void print()const;

};

#endif //POINT_HPP