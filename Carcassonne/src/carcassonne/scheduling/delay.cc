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
// File: carcassonne/scheduling/delay.cc

#include "carcassonne/scheduling/delay.h"

namespace carcassonne {
namespace scheduling {

Delay::Delay()
   : duration_(sf::Time::Zero),
     running_(false)
{
}

Delay::Delay(sf::Time duration)
   : duration_(duration),
     running_(false)
{
}
   
sf::Time Delay::getDuration() const
{
   return duration_;
}

sf::Time Delay::getElapsed() const
{
   if (!running_)
      return sf::Time::Zero;

   return timer_.getElapsedTime();
}

void Delay::reset()
{
   running_ = false;
}

void Delay::reset(sf::Time duration)
{
   duration_ = duration;
   timer_.restart();
   running_ = false;
}

void Delay::add(sf::Time duration)
{
   duration_ += duration;
}

bool Delay::operator()()
{
   if (!running_)
   {
      running_ = true;
      timer_.restart();
   }

   return timer_.getElapsedTime() > duration_;
}

} // namespace scheduling
} // namespace carcassonne
