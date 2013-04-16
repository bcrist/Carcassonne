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
// File: carcassonne/board.inl
//
// Represents a map of the game world - by using a std::unordered_map of tiles

#ifndef CARCASSONNE_BOARD_INL_
#define CARCASSONNE_BOARD_INL_

#ifndef CARCASSONNE_BOARD_H_
#include "carcassonne/board.h"
#endif

namespace std {

// std::hash specialization for glm::ivec2 to allow use in std::unordered_*
// containers.
template<>
inline size_t hash<glm::ivec2>::operator()(const glm::ivec2& value)
{
   size_t y = std::hash<int>()(value.y);
   y = (y >> (sizeof(size_t) * 4)) ^ (y << (sizeof(size_t) * 4));
   return std::hash<int>()(value.x) ^ y;
};

} // namespace std

#endif
