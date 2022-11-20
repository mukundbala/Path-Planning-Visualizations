#ifndef POINT_HPP
#define POINT_HPP
#include <string>
#include <cassert>
#include <math.h>
#include <SFML/Graphics.hpp>


class Vec2D
{
    // Class to reporesent points and lines in the 2D coordinate system of SFML
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
   
   void operator += (Vec2D& rhs);
   void operator -= (Vec2D& rhs);
   
   Vec2D operator + (double scalar)const;
   Vec2D operator - (double scalar)const;
   
   void operator += (double scalar);
   void operator -= (double scalar);
   
   void operator ++();
   void operator --();

   double mag()const;
   double mag(Vec2D& other_point)const;

   double dot (Vec2D& rhs)const;

};

#endif //POINT_HPP