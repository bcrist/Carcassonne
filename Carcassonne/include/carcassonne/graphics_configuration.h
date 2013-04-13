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
// File: carcassonne/graphics_configuration.cc
//
// GraphicsConfiguration is a DAO for accessing graphics configuration data
// stored in a Carcassonne configuration database.

#ifndef CARCASSONNE_GRAPHICS_CONFIGURATION_H_
#define CARCASSONNE_GRAPHICS_CONFIGURATION_H_
#include "carcassonne/_carcassonne.h"

#include <SFML/Window.hpp>

#include "carcassonne/db/db.h"

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

   enum FogMode
   {
      FOG_MODE_DISABLED = 0,
      FOG_MODE_LINEAR = 1,
      FOG_MODE_EXP = 2,
      FOG_MODE_EXP2 = 3
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
                         unsigned int gl_version_minor,
                         float vertical_field_of_view,
                         FogMode fog_mode,
                         const glm::vec4& fog_color,
                         float fog_density,
                         float fog_start,
                         float fog_end);

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

   float vertical_fov;           // The vertical field of view to use for a projection matrix

   FogMode fog_mode;             // The fog mode to use
   glm::vec4 fog_color;          // The color of the fog
   float fog_density;            // The density to use for EXP and EXP2 fog modes
   float fog_start;              // The clip-space z-coordinate where fog should start
   float fog_end;                // The clip-space z-coordinate where fog should end

};

} // namespace carcassonne

#endif
