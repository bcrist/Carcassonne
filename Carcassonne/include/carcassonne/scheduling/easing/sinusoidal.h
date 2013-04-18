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
// File: carcassonne/scheduling/easing/sinusoidal.h

#ifndef CARCASSONNE_SCHEDULING_EASING_SINUSOIDAL_H_
#define CARCASSONNE_SCHEDULING_EASING_SINUSOIDAL_H_
#include "carcassonne/_carcassonne.h"

namespace carcassonne {
namespace scheduling {
namespace easing {

struct SinusoidalIn
{
   float operator()(float f);
};

struct SinusoidalOut
{
   float operator()(float f);
};

struct SinusoidalInOut
{
   float operator()(float f);
};

} // namespace carcassonne::scheduling::easing
} // namespace carcassonne::scheduling
} // namespace carcassonne

#include "carcassonne/scheduling/easing/sinusoidal.inl"

#endif
