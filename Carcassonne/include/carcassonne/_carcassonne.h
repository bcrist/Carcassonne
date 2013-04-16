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
//         Josh Douglas
// File: _carcassonne.h
//
// Default include file.

#ifndef CARCASSONNE__CARCASSONNE_H_
#define CARCASSONNE__CARCASSONNE_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif

// stream insertion overloads for debugging glm vector types
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec2<T>& vec)
{
   return os << '(' << vec.x << ',' << vec.y << ')';
}
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec3<T>& vec)
{
   return os << '(' << vec.x << ',' << vec.y << ',' << vec.z << ')';
}
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const glm::detail::tvec4<T>& vec)
{
   return os << '(' << vec.x << ',' << vec.y << ',' << vec.z << ',' << vec.w << ')';
}

#endif
