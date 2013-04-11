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
// File: main.cc
//
// Application entry point.


#ifdef DEBUG

//#pragma comment (lib, "sfml-main-d.lib")
#pragma comment (lib, "sfml-system-d.lib")
#pragma comment (lib, "sfml-window-d.lib")
#pragma comment (lib, "opengl32.lib")

#else

//#pragma comment (lib, "sfml-main.lib")
#pragma comment (lib, "sfml-system.lib")
#pragma comment (lib, "sfml-window.lib")
#pragma comment (lib, "opengl32.lib")

#endif

#include "game.h"

int main()
{
   carcassonne::Game* game;

   try
   {
      carcassonne::Game g;
      game = &g;
   }
   catch (const carcassonne::db::DB::error& err)
   {
      std::cerr << "Could not load carcassonne.config!" << std::endl
                << err.what() << std::endl;
      return -1;
   }

   return game->run();
}
