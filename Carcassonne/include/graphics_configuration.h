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
// File: graphics_configuration.cc
//
// GraphicsConfiguration is a DAO for accessing graphics configuration data
// stored in a Carcassonne configuration database.

#ifndef CARCASSONNE_GRAPHICS_CONFIGURATION_H_
#define CARCASSONNE_GRAPHICS_CONFIGURATION_H_

#include <SFML/Window.hpp>

#include "db/db.h"

namespace carcassonne {

struct GraphicsConfiguration
{
public:
   static GraphicsConfiguration load(db::DB& db);

   enum WindowMode
   {
      WINDOW_MODE_RESIZEABLE = 0,
      WINDOW_MODE_FIXED = 1,
      WINDOW_MODE_FULLSCREEN_WINDOWED = 2,
      WINDOW_MODE_FULLSCREEN_EXCLUSIVE = 3
   };

   GraphicsConfiguration();
   GraphicsConfiguration(bool save_window_position,
                         const glm::ivec2& window_position,
                         glm::ivec2 viewport_size,
                         WindowMode window_mode,
                         bool v_sync,
                         unsigned int msaa_level,
                         unsigned int color_bits,
                         unsigned int depth_bits,
                         unsigned int stencil_bits,
                         unsigned int gl_version_major,
                         unsigned int gl_version_minor);

   bool save(db::DB& db);
   bool saveWindowLocation(db::DB& db);

   bool save_window_location;    // whether or not so save and load window location data.
   glm::ivec2 window_position;   // the initial position of the window.
   glm::ivec2 viewport_size;     // the current size of the OpenGL client area.

   WindowMode window_mode;       // The type of fullscreen or windowed context to use.

   bool v_sync;                  // Whether to enable vertical-sync
   unsigned int msaa_level;      // The level of multisampling anti-aliasing to use. (0 to disable)
   
   unsigned int color_bits;      // The color depth to use. (0 for the desktop's native color depth)
   unsigned int depth_bits;      // The requested size of each element in the depth buffer.
   unsigned int stencil_bits;    // The requested size of each element in the stencil buffer.
   
   unsigned int gl_version_major;   // The major part of the OpenGL version number requested.
   unsigned int gl_version_minor;   // The minor part of the OpenGL version number requested.
};

} // namespace carcassonne

#endif
