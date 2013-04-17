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
// File: carcassonne/scheduling/sequence.cc

#include "carcassonne/scheduling/sequence.h"

namespace carcassonne {
namespace scheduling {

Sequence::Sequence()
{
}
   
void Sequence::schedule(const std::function<bool()>& deferred)
{
   deferred_functions_.push_back(deferred);
}

void Sequence::clear()
{
   deferred_functions_.clear();
}

   // call the first function in deferred_functions_.  Remove it and return true
   // if it returns true.  Otherwise return false
bool Sequence::operator()()
{
   if (deferred_functions_.empty())
      return true;

   if (deferred_functions_.front()())
   {
      deferred_functions_.erase(deferred_functions_.begin());

      if (deferred_functions_.empty())
         return true;
   }
   return false;
}


} // namespace scheduling
} // namespace carcassonne
