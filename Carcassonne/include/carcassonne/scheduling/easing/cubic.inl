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
// File: carcassonne/scheduling/easing/cubic.inl

#ifndef CARCASSONNE_SCHEDULING_EASING_CUBIC_INL_
#define CARCASSONNE_SCHEDULING_EASING_CUBIC_INL_

#ifndef CARCASSONNE_SCHEDULING_EASING_CUBIC_H_
#include "carcassonne/scheduling/easing/cubic.h"
#endif


namespace carcassonne {
namespace scheduling {
namespace easing {

inline float CubicIn::operator()(float f)
{
   return f * f * f;
}

inline float CubicOut::operator()(float f)
{
   f = 1.0f - f;
   return 1.0f - f * f * f;
}

inline float CubicInOut::operator()(float f)
{
   f *= 2.0f;
   if (f <= 1.0f)
   {
      return 0.5f * (f * f * f);
   }
   else
   {
      f = 2.0f - f;
      return 0.5f * (2.0f - f * f * f);
   }
}

} // namespace carcassonne::scheduling::easing
} // namespace carcassonne::scheduling
} // namespace carcassonne

#endif
