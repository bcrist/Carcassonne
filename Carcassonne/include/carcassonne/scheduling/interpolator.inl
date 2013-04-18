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
// File: carcassonne/scheduling/interpolator.inl

#ifndef CARCASSONNE_SCHEDULING_INTERPOLATOR_INL_
#define CARCASSONNE_SCHEDULING_INTERPOLATOR_INL_

#ifndef CARCASSONNE_SCHEDULING_INTERPOLATOR_H_
#include "carcassonne/scheduling/interpolator.h"
#endif

namespace carcassonne {
namespace scheduling {

template <typename T, typename P, typename R>
Interpolator<T,P,T,R>::Interpolator(sf::Time duration, const std::function<R(P)>& func, P initial, P final, const std::function<float(float)>& easing)
   : duration_(duration),
     easing_(easing),
     initial_(initial),
     final_(final),
     function_(func)
{}
   
template <typename T, typename P, typename R>
sf::Time Interpolator<T,P,R>::getDuration() const
{
   return duration_;
}

template <typename T, typename P, typename R>
sf::Time Interpolator<T,P,R>::getElapsed() const
{
   return elapsed_;
}

template <typename T, typename P, typename R>
void Interpolator<T,P,R>::reset()
{
   elapsed_ = sf::Time();
}

template <typename T, typename P, typename R>
void Interpolator<T,P,R>::reset(sf::Time duration)
{
   duration_ = duration;
   elapsed_ = sf::Time();
}

template <typename T, typename P, typename R>
float Interpolator<T,P,R>::getFraction() const
{
   float f = elapsed_.asMicroseconds() / float(duration_.asMicroseconds());
   if (f < 0)
      f = 0;
   else if (f > 1)
      f = 1;

   return f;
}

template <typename T, typename P, typename R>
float Interpolator<T,P,R>::getEased() const
{
   if (!easing_)
      return getFraction();

   return easing_(getFraction());
}

template <typename T, typename P, typename R>
float Interpolator<T,P,R>::getEased(float fraction) const
{
   if (!easing_)
      return fraction;

   return easing_(fraction);
}

template <typename T, typename P, typename R>
T Interpolator<T,P,R>::get() const
{
   return get(getFraction());
}

template <typename T, typename P, typename R>
T Interpolator<T,P,R>::get(float fraction) const
{
   float b = getEased(fraction),
         a = 1 - b;

   return initial_ * a + final_ * b;
}

template <typename T, typename P, typename R>
bool Interpolator<T,P,R>::operator()(sf::Time delta)
{
   elapsed_ += delta;
   function_(get());
   return elapsed_ >= duration_;
}

} // namespace scheduling
} // namespace carcassonne

#endif
