// Copyright (c) 2013 Dougrist Productions
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Benjamin Crist
// File: carcassonne/scheduling/interpolator.h

#ifndef CARCASSONNE_SCHEDULING_INTERPOLATOR_H_
#define CARCASSONNE_SCHEDULING_INTERPOLATOR_H_
#include "carcassonne/_carcassonne.h"

#include <functional>
#include <SFML/System.hpp>

namespace carcassonne {
namespace scheduling {

// T - type of value being interpolated
// P - type of parameter of function to set value
// R - return type of function to set value (discarded)
template <typename T, typename P = T, typename R = void>
class Interpolator
{
public:
   Interpolator(sf::Time duration, const std::function<R(P)>& func, P initial = 0, P final = 1, const std::function<float(float)>& easing = nullptr);
   
   sf::Time getDuration() const;
   sf::Time getElapsed() const;

   void reset();
   void reset(sf::Time duration);

   float getFraction() const;

   float getEased() const;
   float getEased(float fraction) const;

   T get() const;

   T get(float fraction) const;

   bool operator()(sf::Time delta);

private:
   sf::Time elapsed_;
   sf::Time duration_;

   std::function<float(float)> easing_;

   T initial_;
   T final_;

   std::function<R(P)> function_;
};

} // namespace scheduling
} // namespace carcassonne

#endif
