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

#include "carcassonne/graphics_configuration.h"

#include <SFML/OpenGL.hpp>

#include "carcassonne/db/stmt.h"

namespace carcassonne {

///////////////////////////////////////////////////////////////////////////////
// Loads and returns the active GraphicsConfiguration from the database
// provided.  If the database contains no configuration data, or if an error
// occurs, the default configuration is returned.
GraphicsConfiguration GraphicsConfiguration::load(db::DB& db)
{
   try
   {
      if (db.getInt("SELECT count(*) "
                    "FROM sqlite_master "
                    "WHERE type='table' "
                    "AND name='cc_gfx_cfg'", 0) > 0)
      {
         // Load config data from the database
         db::Stmt s(db, "SELECT "
                        "save_window_location, " // 0
                        "window_x, window_y, size_x, size_y, " // 1, 2, 3, 4
                        "window_mode, "   // 5
                        "v_sync, msaa_level, " // 6, 7
                        "color_bits, depth_bits, stencil_bits, " // 8, 9, 10
                        "gl_version_major, gl_version_minor, " // 11, 12
                        "vertical_fov, " // 13
                        "fog_mode, " // 14
                        "fog_color_r, fog_color_g, fog_color_b, fog_color_a, " // 15, 16, 17, 18
                        "fog_density, fog_start, fog_end" // 19, 20, 21
                        "FROM cc_gfx_cfg LIMIT 1");
         if (s.step())
         {
            GraphicsConfiguration cfg;

            cfg.save_window_location = s.getInt(0) > 0;
            cfg.window_position = glm::ivec2(s.getInt(1), s.getInt(2));
            cfg.viewport_size = glm::ivec2(s.getInt(3), s.getInt(4));

            int mode = s.getInt(5);
            if (mode < WINDOW_MODE_RESIZEABLE || mode > WINDOW_MODE_FULLSCREEN_EXCLUSIVE)
               mode = WINDOW_MODE_RESIZEABLE;
            cfg.window_mode = static_cast<WindowMode>(mode);

            cfg.v_sync = s.getInt(6) > 0;
            cfg.msaa_level = s.getInt(7);

            cfg.color_bits = s.getInt(8);
            cfg.depth_bits = s.getInt(9);
            cfg.stencil_bits = s.getInt(10);
            cfg.gl_version_major = s.getInt(11);
            cfg.gl_version_minor = s.getInt(12);

            cfg.vertical_fov = static_cast<float>(s.getDouble(13));
            
            mode = s.getInt(14);
            if (mode < FOG_MODE_DISABLED || mode > FOG_MODE_EXP2)
               mode = FOG_MODE_DISABLED;
            cfg.fog_mode = static_cast<FogMode>(mode);

            cfg.fog_color = glm::vec4(s.getDouble(15), s.getDouble(16), s.getDouble(17), s.getDouble(18));
            cfg.fog_density = static_cast<float>(s.getDouble(19));
            cfg.fog_start = static_cast<float>(s.getDouble(20));
            cfg.fog_end = static_cast<float>(s.getDouble(21));

            return cfg;
         }
      }
   }
   catch (const db::DB::error& err)
   {
      std::cerr << "Error loading graphics configuration: " << err.what();
   }

   return GraphicsConfiguration();
}

///////////////////////////////////////////////////////////////////////////////
// Constructs a GraphicsConfiguration using default values
GraphicsConfiguration::GraphicsConfiguration()
   : save_window_location(false),
     window_position(-1, -1),
     viewport_size(768, 480),
     window_mode(WINDOW_MODE_RESIZEABLE),
     v_sync(true),
     msaa_level(0),
     color_bits(0),
     depth_bits(0),
     stencil_bits(0),
     gl_version_major(2),
     gl_version_minor(0),
     vertical_fov(50),
     fog_mode(FOG_MODE_DISABLED),
     fog_color(0,0,0,0),
     fog_density(0.001f),
     fog_start(10),
     fog_end(100)
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructs a GraphicsConfiguration using the default values provided.
GraphicsConfiguration::GraphicsConfiguration(bool save_window_location,
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
                                             float vertical_fov,
                                             FogMode fog_mode,
                                             const glm::vec4& fog_color,
                                             float fog_density,
                                             float fog_start,
                                             float fog_end)
   : save_window_location(save_window_location),
     window_position(window_position),
     viewport_size(viewport_size),
     window_mode(window_mode),
     v_sync(v_sync),
     msaa_level(msaa_level),
     color_bits(color_bits),
     depth_bits(depth_bits),
     stencil_bits(stencil_bits),
     gl_version_major(gl_version_major),
     gl_version_minor(gl_version_minor),
     vertical_fov(vertical_fov),
     fog_mode(fog_mode),
     fog_color(fog_color),
     fog_density(fog_density),
     fog_start(fog_start),
     fog_end(fog_end)
{
}

///////////////////////////////////////////////////////////////////////////////
// Saves the content of this GraphicsConfiguration to the database provided and
// makes it the active GraphicsConfiguration in the database.  Old graphics
// configuration data is retained in the database in the form of a stack.
// Deleting the record with the highest ROWID effectively pops the active
// GraphicsConfiguration off that stack.
// 
// Returns true if the GraphicsConfiguration was saved successfully or
// false if an error occurred.
bool GraphicsConfiguration::save(db::DB& db)
{
   try
   {
      // Create the graphics config table if it doesn't exist
      db.exec("CREATE TABLE IF NOT EXISTS cc_gfx_cfg ("
              "ROWID INTEGER PRIMARY KEY DESC AUTOINCREMENT, "
              "save_window_location INTEGER, "
              "window_x INTEGER, "
              "window_y INTEGER, "
              "size_x INTEGER, "
              "size_y INTEGER, "
              "window_mode INTEGER, "
              "v_sync INTEGER, "
              "msaa_level INTEGER, "
              "color_bits INTEGER, "
              "depth_bits INTEGER, "
              "stencil_bits INTEGER, "
              "gl_version_major INTEGER, "
              "gl_version_minor INTEGER, "
              "vertical_fov NUMERIC, "
              "fog_mode INTEGER, "
              "fog_color_r NUMERIC, "
              "fog_color_g NUMERIC, "
              "fog_color_b NUMERIC, "
              "fog_color_a NUMERIC, "
              "fog_density NUMERIC, "
              "fog_start NUMERIC, "
              "fog_end NUMERIC)");

      // Save config data to database
      db::Stmt s(db, "INSERT INTO cc_gfx_cfg ("
                     "save_window_location, " // 1
                     "window_x, window_y, size_x, size_y, " // 2, 3, 4, 5
                     "window_mode, " // 6
                     "v_sync, msaa_level, " // 7, 8
                     "color_bits, depth_bits, stencil_bits, " // 9, 10, 11
                     "gl_version_major, gl_version_minor," // 12, 13
                     "vertical_fov, " // 14
                     "fog_mode, " // 15
                     "fog_color_r, fog_color_g, fog_color_b, fog_color_a, " // 16, 17, 18, 19
                     "fog_density, fog_start, fog_end" // 20, 21, 22
                     ") VALUES (?,?,?,?,?,?,?,?,?,?,"
                               "?,?,?,?,?,?,?,?,?,?,"
                               "?,?)");
      s.bind(1, save_window_location ? 1 : 0);
      s.bind(2, window_position.x);
      s.bind(3, window_position.y);
      s.bind(4, viewport_size.x);
      s.bind(5, viewport_size.y);
      s.bind(6, window_mode);
      s.bind(7, v_sync ? 1 : 0);
      s.bind(8, static_cast<int>(msaa_level));
      s.bind(9, static_cast<int>(color_bits));
      s.bind(10, static_cast<int>(depth_bits));
      s.bind(11, static_cast<int>(stencil_bits));
      s.bind(12, static_cast<int>(gl_version_major));
      s.bind(13, static_cast<int>(gl_version_minor));
      s.bind(14, vertical_fov);
      s.bind(15, fog_mode);
      s.bind(16, fog_color.r);
      s.bind(17, fog_color.g);
      s.bind(18, fog_color.b);
      s.bind(19, fog_color.a);
      s.bind(20, fog_density);
      s.bind(21, fog_start);
      s.bind(22, fog_end);

      s.step();

      return true;
   }
   catch (const db::DB::error& err)
   {
      std::cerr << "Error saving graphics configuration: " << err.what();
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
// If this GraphicsConfiguration represents a non-fullscreen mode and the
// save_window_location flag is set, saves the window_position and
// viewport_size fields to the active GraphicsConfiguration in the database
// provided.
// 
// If the database does not have any GraphicsConfigurations, the default
// configuration is saved first (note: the default, NOT this configuration!).
// 
// Returns true if the window location was saved successfully.
bool GraphicsConfiguration::saveWindowLocation(db::DB& db)
{
   // IMHO, the word successfully has too many double consonants.

   if (!save_window_location ||
       window_mode == WINDOW_MODE_FULLSCREEN_WINDOWED ||
       window_mode == WINDOW_MODE_FULLSCREEN_EXCLUSIVE)
   {
      return false;
   }

   try
   {
      if (db.getInt("SELECT count(*) "
                    "FROM sqlite_master "
                    "WHERE type='table' "
                    "AND name='cc_gfx_cfg';", 0) == 0)
      {
         GraphicsConfiguration().save(db);
      }

      // Save config data to database
      int rowid = db.getInt("SELECT ROWID FROM cc_gfx_cfg LIMIT 1", -1);
      if (rowid < 0)
      {
         GraphicsConfiguration().save(db);
         rowid = db.getInt("SELECT ROWID FROM cc_gfx_cfg LIMIT 1", -1);
      }

      db::Stmt s(db, "UPDATE cc_gfx_cfg SET "
                     "window_x = ?, "
                     "window_y = ?, "
                     "size_x = ?, "
                     "size_y = ? "
                     "WHERE ROWID = ?");

      s.bind(1, window_position.x);
      s.bind(2, window_position.y);
      s.bind(3, viewport_size.x);
      s.bind(4, viewport_size.y);
      s.bind(5, rowid);
      s.step();

      return true;
   }
   catch (const db::DB::error& err)
   {
      std::cerr << "Error saving graphics configuration: " << err.what();
   }

   return false;
}

} // namespace carcassonne
