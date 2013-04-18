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
// File: carcassonne/scheduling/unifier.cc

#include "carcassonne/scheduling/unifier.h"

namespace carcassonne {
namespace scheduling {

Unifier::Unifier()
{
}
   
void Unifier::schedule(const std::function<bool(sf::Time)>& deferred)
{
   deferred_functions_.push_back(deferred);
}

void Unifier::clear()
{
   deferred_functions_.clear();
}

// call each function in deferred_functions_.  Remove any functions
// which return true.  If any functions return false, return
// false, otherwise return true;
bool Unifier::operator()(sf::Time delta)
{
   for (auto i(deferred_functions_.begin()); i != deferred_functions_.end();)
   {
      if ((*i)(delta))
         i = deferred_functions_.erase(i);
      else
         ++i;
   }

   if (deferred_functions_.empty())
      return true;

   return false;
}


} // namespace scheduling
} // namespace carcassonne
