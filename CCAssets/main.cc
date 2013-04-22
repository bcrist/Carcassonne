#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <set>

#include "stb_image.h"

#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"
#include "carcassonne/db/transaction.h"

enum PrimitiveType {
   TRIANGLES = 0,
   TRIANGLE_STRIP,
   TRIANGLE_FAN,
   QUADS,
   QUAD_STRIP,
   POLYGON,
   POINTS,
   LINES,
   LINE_STRIP,
   LINE_LOOP
};       

enum MeshDataType {
   FACE = 0,
   VERTEX,
   NORMAL,
   TEXTURE_COORD
};

enum FeatureType {
   TYPE_FARM = 0,  // a single farm over the entire edge
   TYPE_ROAD = 1,  // a road in the center with 2 farms (possibly the same farm) on either side
   TYPE_CITY = 2,   // a single city over the entire edge
   TYPE_CLOISTER = 3
};

struct TileEdge
{
   std::string feature;
   std::string cw_farm;
   std::string ccw_farm;
};

struct Follower
{
   bool farming;
   float x;
   float z;
   float rotation;

   Follower() : farming(false), x(0), z(0), rotation(0) {}
};

struct Feature
{
   FeatureType type;
   std::set<std::string> adjacent_cities; // only for farms
   int pennants;  // only for cities
   Follower follower;
   int id;

   Feature() : type(TYPE_FARM), pennants(0), id(0) {}
};

int texture(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 5)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" texture <texture name> <source filename>" << std::endl;
      return 1;
   }

   std::string tex_name(argv[3]);
   std::string source(argv[4]);

   try {
      int width(0), height(0), comps(0);

      stbi_uc* data = stbi_load(source.c_str(), &width, &height, &comps, 4);
      if (data == nullptr)
         throw std::runtime_error("Could not load file!");

      stbi_image_free(data);


      std::ifstream ifs(source, std::ifstream::in | std::ifstream::binary);
      std::vector<char> content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
      ifs.close();

      std::cout << content.size() << " bytes read." << std::endl;

      
      carcassonne::db::DB db(filename);
      carcassonne::db::Transaction transaction(db);
            
      db.exec("CREATE TABLE IF NOT EXISTS cc_textures ("
              "name TEXT PRIMARY KEY, "
              "format TEXT DEFAULT 'file', "
              "width INTEGER DEFAULT 0, "
              "height INTEGER DEFAULT 0, "
              "data BLOB);");

      carcassonne::db::Stmt update(db, "INSERT OR REPLACE INTO cc_textures (name, data, width, height) VALUES (?, ?, ?, ?);");
      update.bind(1, tex_name);
      update.bindBlob(2, content.data(), content.size());
      update.bind(3, width);
      update.bind(4, height);
      update.step();

      transaction.commit();
   }
   catch (const std::exception& e)
   {
      // transaction will be rolled back if not committed,
      // and DB file closed if open.
      std::cerr << e.what();
      return 1;
   }

   return 0;
}

void saveSprite(carcassonne::db::DB& db, const std::string& name, const std::string& texture, float x, float y, float w, float h)
{
   static bool table_check_done(false);

   if (!table_check_done)
   {
      table_check_done = true;
      db.exec("CREATE TABLE IF NOT EXISTS cc_sprites ("
              "name TEXT PRIMARY KEY, "
              "texture TEXT, "
              "x NUMERIC, y NUMERIC, "
              "width NUMERIC, height NUMERIC)");
   }

   carcassonne::db::Stmt update(db, "INSERT OR REPLACE INTO cc_sprites (name, texture, x, y, width, height) VALUES (?, ?, ?, ?, ?, ?);");
   update.bind(1, name);
   update.bind(2, texture);
   update.bind(3, x);
   update.bind(4, y);
   update.bind(5, w);
   update.bind(6, h);
   update.step();
   update.reset();
}

int sprite(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 9)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" sprite <texture name> <x> <y> <w> <h> <sprite name> [...]" << std::endl;
      return 1;
   }

   std::string tex_name(argv[3]);
   float x(float(atof(argv[4])));
   float y(float(atof(argv[5])));
   float w(float(atof(argv[6])));
   float h(float(atof(argv[7])));

   float initial_x(x);

   try
   {
      carcassonne::db::DB db(filename);
      carcassonne::db::Transaction transaction(db);

      for (int i = 8; i < argc; ++i)
      {
         saveSprite(db, argv[i], tex_name, x, y, w, h);

         x += w;
         if (x >= 1)
         {
            x = initial_x;
            y += h;
         }
      }
      
      transaction.commit();
   }
   catch (const std::exception& e)
   {
      // transaction will be rolled back if not committed,
      // and DB file closed if open.
      std::cerr << e.what();
      return 1;
   }

   return 0;
}

int obj_mesh(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 6)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" obj <mesh name> <texture name> <OBJ filename>" << std::endl;
      return 1;
   }

   std::string mesh_name(argv[3]);
   std::string tex_name(argv[4]);
   std::string source(argv[5]);

   try {
      carcassonne::db::DB db(filename);
      carcassonne::db::Transaction transaction(db);
      

      db.exec("CREATE TABLE IF NOT EXISTS cc_meshes ("
              "name TEXT UNIQUE, "
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "primitive_type INTEGER, "
              "texture TEXT, "
              "indices INTEGER, "
              "vertices INTEGER, "
              "normals INTEGER, "
              "texture_coords INTEGER); "

              "CREATE TABLE IF NOT EXISTS cc_mesh_data ("
              "mesh_id INTEGER, "
              "type INTEGER, "
              "n INTEGER, "
              "x NUMERIC, "
              "y NUMERIC, "
              "z NUMERIC, "
              "PRIMARY KEY (mesh_id, type, n));");

      std::vector<glm::dvec3> vertices;
      std::vector<glm::dvec3> normals;
      std::vector<glm::dvec3> texture_coords;
      std::vector<std::vector<glm::ivec3> > faces;

      std::map<int, int> v_map;
      std::map<int, int> n_map;
      std::map<int, int> t_map;

      int next_v_index = 0;
      int next_n_index = 0;
      int next_t_index = 0;
      int next_f_index = 0;

#pragma region OBJ_loading

      std::ifstream ifs(source, std::ifstream::in);
      std::string line;
      while (std::getline(ifs, line))
      {
         if (line.length() < 1)
            continue;   // skip blank lines

         if (line[0] == '#')
            continue;   // skip comment lines

         std::istringstream iss(line);
         std::string operation;
         iss >> operation;

         if (operation == "v")
         {
            glm::dvec3 v;
            iss >> v.x >> v.y >> v.z;

            auto i (std::find(vertices.begin(), vertices.end(), v));
            if (i == vertices.end())
            {
               v_map[next_v_index++] = vertices.size();
               vertices.push_back(v);
            }
            else
               v_map[next_v_index++] = i - vertices.begin();
         }
         else if (operation == "vn")
         {
            glm::dvec3 n;
            iss >> n.x >> n.y >> n.z;

            auto i (std::find(normals.begin(), normals.end(), n));
            if (i == normals.end())
            {
               n_map[next_n_index++] = normals.size();
               normals.push_back(n);
            }
            else
               n_map[next_n_index++] = i - normals.begin();
         }
         else if (operation == "vt")
         {
            glm::dvec3 t;
            iss >> t.x >> t.y;

            auto i (std::find(texture_coords.begin(), texture_coords.end(), t));
            if (i == texture_coords.end())
            {
               t_map[next_t_index++] = texture_coords.size();
               texture_coords.push_back(t);
            }
            else
               t_map[next_t_index++] = i - texture_coords.begin();
         }
         else if (operation == "f")
         {  
            faces.resize(next_f_index + 1);
            while (iss)
            {
               glm::ivec3 i;

               std::string str;
               {
                  std::getline(iss, str, '/');
                  if (!iss)
                     break;
                  std::istringstream iss2(str);
                  iss2 >> i.x;
                  i.x = v_map[i.x - 1];
                  if (static_cast<size_t>(i.x) >= vertices.size())
                  {
                     std::cerr << "Vertex " << i.x << " not found!  " << vertices.size() << " vertices defined." << std::endl;
                     return 1;
                  }
               }

               {
                  std::getline(iss, str, '/');
                  if (!iss)
                     break;
                  std::istringstream iss2(str);
                  iss2 >> i.z;
                  i.z = t_map[i.z - 1];
                  if (static_cast<size_t>(i.z) >= vertices.size())
                  {
                     std::cerr << "Texture coordinate " << i.z << " not found!  " << texture_coords.size() << " texture coordinates defined." << std::endl;
                     return 1;
                  }
               }

               {
                  std::getline(iss, str, ' ');
                  if (!iss)
                     break;
                  std::istringstream iss2(str);
                  iss2 >> i.y;
                  i.y = n_map[i.y - 1];
                  if (static_cast<size_t>(i.y) >= normals.size())
                  {
                     std::cerr << "Normal " << i.y << " not found!  " << normals.size() << " normals defined." << std::endl;
                     return 1;
                  }
               }
               faces[next_f_index].push_back(i);
            }
            ++next_f_index;
         }
      }

      ifs.close();

#pragma endregion

      int max_poly_size = 0;
      for (auto i(faces.begin()), end(faces.end()); i != end; ++i)
         if (i->size() > static_cast<size_t>(max_poly_size))
            max_poly_size = i->size();

      if (max_poly_size > 4)
      {
         std::cerr << "Mesh contains large face(s)!" << std::endl;
         return 1;
      }

      PrimitiveType type = max_poly_size == 4 ? QUADS : TRIANGLES;
      max_poly_size = type == QUADS ? 4 : 3;
      std::cout << "Loaded " << faces.size() << (type == QUADS ? " quads." : " triangles.");


      
      carcassonne::db::Stmt s_get_id(db, "SELECT id FROM cc_meshes WHERE name = ?");
      s_get_id.bind(1, mesh_name);
      while (s_get_id.step())
      {
         int id = s_get_id.getInt(0);

         carcassonne::db::Stmt s_delete_mesh(db, "DELETE FROM cc_meshes WHERE id = ?");
         s_delete_mesh.bind(1, id);
         s_delete_mesh.step();

         carcassonne::db::Stmt s_delete_data(db, "DELETE FROM cc_mesh_data WHERE mesh_id = ?");
         s_delete_data.bind(1, id);
         s_delete_data.step();
      }

      carcassonne::db::Stmt s_mesh(db, "INSERT INTO cc_meshes ("
                                       "name, "
                                       "primitive_type, "
                                       "texture, "
                                       "indices, "
                                       "vertices, "
                                       "normals, "
                                       "texture_coords) "
                                       "VALUES (?, ?, ?, ?, ?, ?, ?);");
      s_mesh.bind(1, mesh_name);
      s_mesh.bind(2, type);
      s_mesh.bind(3, tex_name);
      s_mesh.bind(4, next_f_index);
      s_mesh.bind(5, int(vertices.size()));
      s_mesh.bind(6, int(normals.size()));
      s_mesh.bind(7, int(texture_coords.size()));
      s_mesh.step();

      int id = -1;
      s_get_id.reset();
      s_get_id.bind(1, mesh_name);
      if (s_get_id.step())
         id = s_get_id.getInt(0);
      else
      {
         std::cerr << "Failed to get mesh id from cc_meshes!" << std::endl;
         return 1;
      }



      carcassonne::db::Stmt s_data(db, "INSERT INTO cc_mesh_data ("
                                       "mesh_id, "
                                       "type, "
                                       "n, "
                                       "x, "
                                       "y, "
                                       "z) "
                                       "VALUES (?, ?, ?, ?, ?, ?);");
      s_data.bind(1, id);
      s_data.bind(2, VERTEX);
      for (auto i(vertices.begin()), end(vertices.end()); i != end; ++i)
      {
         
         s_data.bind(3, i - vertices.begin());
         s_data.bind(4, i->x);
         s_data.bind(5, i->y);
         s_data.bind(6, i->z);
         s_data.step();
         s_data.reset();
      }

      s_data.bind(2, NORMAL);
      for (auto i(normals.begin()), end(normals.end()); i != end; ++i)
      {
         s_data.bind(3, i - normals.begin());
         s_data.bind(4, i->x);
         s_data.bind(5, i->y);
         s_data.bind(6, i->z);
         s_data.step();
         s_data.reset();
      }

      s_data.bind(2, TEXTURE_COORD);
      for (auto i(texture_coords.begin()), end(texture_coords.end()); i != end; ++i)
      {
         s_data.bind(3, i - texture_coords.begin());
         s_data.bind(4, i->x);
         s_data.bind(5, -i->y);  // opengl expects texture coordinates that are inverted compared to everyone else
         s_data.bind(6, i->z);
         s_data.step();
         s_data.reset();
      }

      s_data.bind(2, FACE);
      int idx = 0;
      for (auto i(faces.begin()), end(faces.end()); i != end; ++i)
      {
         int poly_idx = 0;
         for (auto j(i->begin()), end(i->end()); j != end; ++j)
         {
            s_data.bind(3, idx++);
            s_data.bind(4, j->x);
            s_data.bind(5, j->y);
            s_data.bind(6, j->z);
            s_data.step();
            s_data.reset();
            if (++poly_idx == max_poly_size)
               break;
         }
         while (poly_idx < max_poly_size)
         {
            // if there aren't enough vertices in this primitive, repeat the last one.
            s_data.bind(3, idx++);
            s_data.step();
            s_data.reset();
            ++poly_idx;
         }
      }

      transaction.commit();
   }
   catch (const std::exception& e)
   {
      // transaction will be rolled back if not committed,
      // and DB file closed if open.
      std::cerr << e.what();
      return 1;
   }

   return 0;
}

void insertTile(carcassonne::db::DB& db, const std::string& tile_name, const std::string& texture_name, std::map<std::string, Feature>& features, const TileEdge* edges, const std::string& cloister_name)
{
   std::cout << tile_name << ": " << texture_name << std::endl;
   for (auto i(features.begin()), end(features.end()); i != end; ++i)
   {
      std::cout << "   " << i->first << ": type:" << i->second.type << " p:" << i->second.pennants << " follower: " << i->second.follower.farming << " " << i->second.follower.x << " " << i->second.follower.z << " " << i->second.follower.rotation << " adj:";
      for (auto j(i->second.adjacent_cities.begin()), end(i->second.adjacent_cities.end()); j != end; ++j)
         std::cout << " " << *j;
      std::cout << std::endl;
   }
   std::cout << std::endl;

   std::cout << "   N: " << edges[0].feature << ' ' << edges[0].cw_farm << ' ' << edges[0].ccw_farm << std::endl
             << "   E: " << edges[1].feature << ' ' << edges[1].cw_farm << ' ' << edges[1].ccw_farm << std::endl
             << "   S: " << edges[2].feature << ' ' << edges[2].cw_farm << ' ' << edges[2].ccw_farm << std::endl
             << "   W: " << edges[3].feature << ' ' << edges[3].cw_farm << ' ' << edges[3].ccw_farm << std::endl;
   std::cout << std::endl;

   // save tile to db
#pragma region remove old tile
   std::set<int> old_features;
   carcassonne::db::Stmt s_get(db, "SELECT north, north_cw, north_ccw, "   // 0, 1, 2
                                    "east, east_cw, east_ccw, "            // 3, 4, 5
                                    "south, south_cw, south_ccw, "         // 6, 7, 8
                                    "west, west_cw, west_ccw, "            // 9, 10, 11
                                    "cloister "                            // 12
                                    "FROM cc_tiles "
                                    "WHERE name = ?");
   s_get.bind(1, tile_name);
   if (s_get.step())
   {
      old_features.insert(s_get.getInt(0));
      old_features.insert(s_get.getInt(1));
      old_features.insert(s_get.getInt(2));
      old_features.insert(s_get.getInt(3));
      old_features.insert(s_get.getInt(4));
      old_features.insert(s_get.getInt(5));
      old_features.insert(s_get.getInt(6));
      old_features.insert(s_get.getInt(7));
      old_features.insert(s_get.getInt(8));
      old_features.insert(s_get.getInt(9));
      old_features.insert(s_get.getInt(10));
      old_features.insert(s_get.getInt(11));
      old_features.insert(s_get.getInt(12));
      s_get.reset();

      // remove old tile
      carcassonne::db::Stmt s_delete_tile(db, "DELETE FROM cc_tiles WHERE name = ?");
      s_delete_tile.bind(1, tile_name);
      s_delete_tile.step();


      // if old features are still in use by other tiles, don't delete them
      carcassonne::db::Stmt s_get_others(db, "SELECT name FROM cc_tiles WHERE "
                                             "north = ? OR north_cw = ? OR north_ccw = ? OR "
                                             "east = ? OR east_cw = ? OR east_ccw = ? OR "
                                             "south = ? OR south_cw = ? OR south_ccw = ? OR "
                                             "west = ? OR west_cw = ? OR west_ccw = ? OR "
                                             "cloister = ? LIMIT 1");
      for (auto i(old_features.begin()); i != old_features.end();)
      {
         s_get_others.bind(1, *i);
         s_get_others.bind(2, *i);
         s_get_others.bind(3, *i);

         s_get_others.bind(4, *i);
         s_get_others.bind(5, *i);
         s_get_others.bind(6, *i);

         s_get_others.bind(7, *i);
         s_get_others.bind(8, *i);
         s_get_others.bind(9, *i);

         s_get_others.bind(10, *i);
         s_get_others.bind(11, *i);
         s_get_others.bind(12, *i);

         s_get_others.bind(13, *i);

         if (s_get_others.step())
         {
            i = old_features.erase(i);
            
         }
         else
            ++i;

         s_get_others.reset();
      }

      // all old features remaining can be removed
      carcassonne::db::Stmt s_delete_old(db, "DELETE FROM cc_tile_features WHERE id = ?");
      for (auto i(old_features.begin()), end(old_features.end()); i != end; ++i)
      {
         s_delete_old.bind(1, *i);
         s_delete_old.step();
         s_delete_old.reset();
      }
   }
#pragma endregion

   // old tile removed if present; insert new tile
   carcassonne::db::Stmt s_insert_feature(db, "INSERT INTO cc_tile_features ("
                                              "type, follower_orientation, "
                                              "follower_x, follower_z, follower_r, "
                                              "pennants) VALUES (?, ?, ?, ?, ?, ?)");

   carcassonne::db::Stmt s_get_feature_id(db, "SELECT id FROM cc_tile_features ORDER BY id DESC LIMIT 1");

   for (auto i(features.begin()), end(features.end()); i != end; ++i)
   {
      s_insert_feature.bind();
      s_insert_feature.bind(1, static_cast<int>(i->second.type));
      s_insert_feature.bind(2, i->second.follower.farming ? 1 : 0);
      s_insert_feature.bind(3, i->second.follower.z);
      s_insert_feature.bind(4, i->second.follower.x);
      s_insert_feature.bind(5, i->second.follower.rotation);

      s_insert_feature.bind(6, i->second.pennants);

      s_insert_feature.step();
      if (s_get_feature_id.step())
         i->second.id = s_get_feature_id.getInt(0);

      s_insert_feature.reset();
      s_get_feature_id.reset();
   }

   // update adjacent cities for farms
   carcassonne::db::Stmt s_update_feature(db, "UPDATE cc_tile_features SET adjacent1 = ?, adjacent2 = ?, adjacent3 = ?, adjacent4 = ? WHERE id = ?");
   for (auto i(features.begin()), end(features.end()); i != end; ++i)
   {
      if (i->second.adjacent_cities.empty())
         continue;

      auto k(i->second.adjacent_cities.begin());
      for (int j = 1; j <= 4; ++j)
      {
         int id;
         if (k == i->second.adjacent_cities.end())
            id = 0;
         else
         {
            id = features[*k].id;
            ++k;
         }
         s_update_feature.bind(j, id);
      }
      s_update_feature.bind(5, i->second.id);
      s_update_feature.step();
      s_update_feature.reset();
   }


   // insert actual tile object
   carcassonne::db::Stmt s_insert_tile(db, "INSERT INTO cc_tiles ("
                                           "name, "   // 1
                                           "texture, "   //2
                                           "north, north_cw, north_ccw, "  //3 4 5
                                           "east, east_cw, east_ccw, "     //6 7 8
                                           "south, south_cw, south_ccw, "  //9 10 11
                                           "west, west_cw, west_ccw, "     //12 13 14
                                           "cloister) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");  //15
   s_insert_tile.bind(1, tile_name);
   s_insert_tile.bind(2, texture_name);

   for (int i = 0; i < 4; ++i)
   {
      s_insert_tile.bind(3 + 3 * i, features[edges[i].feature].id);
      if (features[edges[i].feature].type == TYPE_ROAD)
      {
         s_insert_tile.bind(4 + 3 * i, features[edges[i].cw_farm].id);
         s_insert_tile.bind(5 + 3 * i, features[edges[i].ccw_farm].id);
      }
      else
      {
         s_insert_tile.bind(4 + 3 * i);
         s_insert_tile.bind(5 + 3 * i);
      }
   }

   if (cloister_name.length() > 0)
      s_insert_tile.bind(15, features[cloister_name].id);
   else
      s_insert_tile.bind(15);

   s_insert_tile.step();
}

int tilespec(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 5)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" tileset <tileset name> <tilespec filename>" << std::endl;
      return 1;
   }

   std::string tileset_name(argv[3]);
   std::string tilespec(argv[4]);

   try {
      carcassonne::db::DB db(filename);
      carcassonne::db::Transaction transaction(db);
      
      db.exec("CREATE TABLE IF NOT EXISTS cc_tilesets ("
              "name TEXT UNIQUE, "
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "starting_tile TEXT); "

              "CREATE TABLE IF NOT EXISTS cc_tileset_tiles ("
              "tileset_id INTEGER, "
              "tile TEXT, "
              "quantity INTEGER, "
              "PRIMARY KEY (tileset_id, tile)); "
         
              "CREATE TABLE IF NOT EXISTS cc_tiles ("
              "name TEXT PRIMARY KEY, "
              "texture TEXT, "
              "north INTEGER, north_cw INTEGER DEFAULT 0, north_ccw INTEGER DEFAULT 0, "
              "east  INTEGER, east_cw  INTEGER DEFAULT 0, east_ccw  INTEGER DEFAULT 0, "
              "south INTEGER, south_cw INTEGER DEFAULT 0, south_ccw INTEGER DEFAULT 0, "
              "west  INTEGER, west_cw  INTEGER DEFAULT 0, west_ccw  INTEGER DEFAULT 0, "
              "cloister INTEGER DEFAULT 0); "

              "CREATE TABLE IF NOT EXISTS cc_tile_features ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "type INTEGER, "
              "adjacent1 INTEGER DEFAULT 0, "
              "adjacent2 INTEGER DEFAULT 0, "
              "adjacent3 INTEGER DEFAULT 0, "
              "adjacent4 INTEGER DEFAULT 0, "
              "pennants INTEGER DEFAULT 0, "
              "follower_orientation INTEGER, "
              "follower_x NUMERIC, "
              "follower_z NUMERIC, "
              "follower_r NUMERIC);");

      std::string starting_tile;
      std::map<std::string, int> other_tiles;

      std::string tile_name;
      std::string texture_name;
      TileEdge edges[4];
      std::map<std::string, Feature> features;
      std::string cloister_name;



      std::ifstream ifs(tilespec, std::ifstream::in);
      std::string line;
      while (std::getline(ifs, line))
      {
         if (line.length() < 1)
            continue;   // skip blank lines

         if (line[0] == '#')
            continue;   // skip comment lines

         std::istringstream iss(line);

         if (line[0] <= ' ')  // whitespace at beginning -> same tile as before
         {
            std::string command;
            iss >> command;
            std::transform(command.begin(), command.end(), command.begin(), tolower);

            if (command == "t" || command == "tex" || command == "texture")
            {
               iss >> texture_name;
            }
            else if (command == "n" || command == "north" ||
                     command == "e" || command == "east" ||
                     command == "s" || command == "south" ||
                     command == "w" || command == "west")
            {
               int i;
               switch (command[0])
               {
                  case 'e': i = 1; break;
                  case 's': i = 2; break;
                  case 'w': i = 3; break;
                  default:  i = 0; break;
               }
               TileEdge& edge = edges[i];

               std::string type;
               iss >> type;
               std::transform(type.begin(), type.end(), type.begin(), tolower);

               FeatureType edge_type = TYPE_CITY;

               if (type == "c" || type == "city")
                  edge_type = TYPE_CITY;
               else if (type == "f" || type == "field" || type == "farm")
                  edge_type = TYPE_FARM;
               else if (type == "r" || type == "road")
                  edge_type = TYPE_ROAD;

               iss >> edge.feature >> edge.cw_farm >> edge.ccw_farm;

               features[edge.feature].type = edge_type;
               if (edge_type == TYPE_ROAD)
               {
                  features[edge.cw_farm].type = TYPE_FARM;
                  features[edge.ccw_farm].type = TYPE_FARM;
               }

            }
            else if (command == "c" || command == "cloister")
            {
               std::string feature;
               iss >> feature;

               features[feature].type = TYPE_CLOISTER;
               cloister_name = feature;
            }
            else if (command == "f" || command == "follower")
            {
               std::string feature, temp;
               iss >> feature >> temp;

               Feature& f = features[feature];

               std::transform(temp.begin(), temp.end(), temp.begin(), tolower);
               if (temp == "f" || temp == "farming")
               {
                  f.follower.farming = true;
                  iss >> f.follower.x >> f.follower.z >> f.follower.rotation;
               }
               else
               {
                  std::istringstream iss2(temp);
                  iss2 >> f.follower.x;
                  iss >> f.follower.z >> f.follower.rotation;
               }
            }
            else if (command == "p" || command == "pennant")
            {
               std::string feature;
               iss >> feature;

               Feature& f = features[feature];
               f.type = TYPE_CITY;
               ++f.pennants;
            }
            else if (command == "a" || command == "adjacent")
            {
               std::string farm, city;
               iss >> farm >> city;

               Feature& f = features[farm];
               Feature& c = features[city];
               f.type = TYPE_FARM;
               c.type = TYPE_CITY;
               f.adjacent_cities.insert(city);
            }

         }
         else  // new tile
         {
            if (tile_name.length() > 0 && tile_name[0] != '$') // $ indicates a tile that should already exist in the DB (i.e. from another tileset)
               insertTile(db, tile_name, texture_name, features, edges, cloister_name);

            features.clear();
            edges[0] = TileEdge();
            edges[1] = TileEdge();
            edges[2] = TileEdge();
            edges[3] = TileEdge();
            cloister_name = std::string();

            std::string temp;
            int tile_quantity = 1;
            iss >> tile_name >> tile_quantity >> temp;
            std::transform(temp.begin(), temp.end(), temp.begin(), tolower);

            if (tile_quantity < 1)
               tile_quantity = 1;

            const char* name = tile_name.c_str();
            if (*name == '$')
               ++name;

            if (temp == "s" || temp == "start")
            {
               --tile_quantity;
               starting_tile = name;
            }

            if (tile_quantity > 0)
               other_tiles[name] = tile_quantity;
         }
      }
      ifs.close();

      if (tile_name.length() > 0 && tile_name[0] != '$') // $ indicates a tile that should already exist in the DB (i.e. from another tileset)
         insertTile(db, tile_name, texture_name, features, edges, cloister_name);

      carcassonne::db::Stmt s_insert_tileset(db, "INSERT OR REPLACE INTO cc_tilesets ("
                                                 "name, starting_tile) VALUES (?, ?)");
      s_insert_tileset.bind(1, tileset_name);
      s_insert_tileset.bind(2, starting_tile);
      s_insert_tileset.step();
         
      int ts_id;

      carcassonne::db::Stmt s_get_tileset_id(db, "SELECT id FROM cc_tilesets WHERE name = ?");
      s_get_tileset_id.bind(1, tileset_name);
      if (s_get_tileset_id.step())
      {
         ts_id = s_get_tileset_id.getInt(0);
      }
      else
         throw std::runtime_error("Could not update cc_tilesets!");


      carcassonne::db::Stmt s_insert_tiles(db, "INSERT OR REPLACE INTO cc_tileset_tiles ("
                                               "tileset_id, tile, quantity) VALUES (?, ?, ?)");
      s_insert_tiles.bind(1, ts_id);

      for (auto i(other_tiles.begin()), end(other_tiles.end()); i != end; ++i)
      {
         s_insert_tiles.bind(2, i->first);
         s_insert_tiles.bind(3, i->second);
         s_insert_tiles.step();
         s_insert_tiles.reset();
      }

      transaction.commit();
   }
   catch (const std::runtime_error& e)
   {
      std::cerr << e.what();
      return 1;
   }

   return 0;
}

int texfont(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 6)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" texfont <texfont name> <default character index> <spec filename> [<character spacing> [<preload start> <preload count>]]" << std::endl;
      return 1;
   }

   std::string font_name(argv[3]);
   int default_character(atoi(argv[4]));
   std::string source(argv[5]);
   int preload_start = 0;
   int preload_count = 0;
   int id = 0;
   float extra_spacing = 0;

   if (argc >= 7)
   {
      extra_spacing = atof(argv[6]);
   }

   if (argc >= 9)
   {
      preload_start = atoi(argv[7]);
      preload_count = atoi(argv[8]);
   }

   try {
      carcassonne::db::DB db(filename);
      carcassonne::db::Transaction transaction(db);
      
      db.exec("CREATE TABLE IF NOT EXISTS cc_texfonts ("
              "name TEXT UNIQUE, "
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "default_character INTEGER DEFAULT 0, "
              "preload_start INTEGER, "
              "preload_count INTEGER); "

              "CREATE TABLE IF NOT EXISTS cc_texfont_characters ("
              "font_id INTEGER, "
              "character INTEGER, "
              "sprite TEXT, "
              "offset_x NUMERIC, "
              "offset_y NUMERIC, "
              "width NUMERIC, "
              "PRIMARY KEY (font_id, character));");
      
      carcassonne::db::Stmt sf(db, "INSERT OR REPLACE INTO cc_texfonts (name, default_character, preload_start, preload_count) VALUES (?, ?, ?, ?);");
      sf.bind(1, font_name);
      sf.bind(2, default_character);
      sf.bind(3, preload_start);
      sf.bind(4, preload_count);
      sf.step();

      carcassonne::db::Stmt get_sf_id(db, "SELECT id FROM cc_texfonts WHERE name = ?");
      get_sf_id.bind(1, font_name);
      if (get_sf_id.step())
         id = get_sf_id.getInt(0);
      else
         throw std::runtime_error("Failed to insert texfont into database!");



      carcassonne::db::Stmt get_tex_dims(db, "SELECT width, height FROM cc_textures WHERE name = ?");

      carcassonne::db::Stmt sfc(db, "INSERT OR REPLACE INTO cc_texfont_characters ("
                                    "font_id, character, "
                                    "sprite, offset_x, offset_y, "
                                    "width) VALUES (?, ?, ?, ?, ?, ?);");
      sfc.bind(1, id);


      std::ifstream ifs(source, std::ifstream::in);
      std::string line;
      while (std::getline(ifs, line))
      {
         if (line.length() < 1)
            continue;   // skip blank lines

         if (line[0] == '#')
            continue;   // skip comment lines

         std::istringstream iss(line);

         std::string character;
         int charindex;

         iss >> character;
         if (character.length() >= 2 && character[0] == '\'')
            charindex = (int)character[1];
         else
         {
            std::istringstream iss(character);
            iss >> std::hex >> charindex;
         }

         std::string texture;
         iss >> texture;

         float baseline(0), descent(0), ascent(0), left(0), right(0), extra_l(0), extra_r(0);
         iss >> baseline >> descent >> ascent >> left >> right >> extra_l >> extra_r;

         float width(256);
         float height(256);

         get_tex_dims.bind(1, texture);
         if (get_tex_dims.step())
         {
            width = get_tex_dims.getInt(0);
            height = get_tex_dims.getInt(1);
         }
         else
            std::cerr << "Warning: texture '" << texture << "' not found; assuming 256x256 dimensions!" << std::endl;
         get_tex_dims.reset();

         std::ostringstream sprite_name;
         sprite_name << "font-" << font_name << '-' << charindex;

         std::cout << charindex << ":\t" << baseline << '\t' << descent << '\t' << ascent << '\t' << left << '\t' << right << '\t' << extra_l << '\t' << extra_r << std::endl;

         float rect_x = (left - extra_l) / width;
         float rect_w = (right + extra_r + extra_l - left) / width;
         float rect_y = (baseline - ascent) / height;
         float rect_h = (ascent + descent) / height;

         float w = (right - left + extra_spacing) / width;
         float offset_x = (rect_x + rect_w * 0.5f) - ((right + left) * 0.5f / width);
         float offset_y = (rect_y + rect_h * 0.5f) - baseline / height;

         sfc.bind(2, charindex);
         sfc.bind(3, sprite_name.str());
         sfc.bind(4, offset_x);
         sfc.bind(5, offset_y);
         sfc.bind(6, w);
         sfc.step();
         sfc.reset();

         saveSprite(db, sprite_name.str(), texture, rect_x, rect_y, rect_w, rect_h);
      }
      ifs.close();

      

      transaction.commit();
   }
   catch (const std::runtime_error& e)
   {
      std::cerr << e.what();
      return 1;
   }

   return 0;
}



int runApp(int argc, char** argv)
{
   if (argc < 2)
   {
      std::cerr << "No database file specified!" << std::endl;
      return -1;
   }
   else if (argc < 3)
   {
      std::cerr << "No operation specified!" << std::endl;
      return -1;
   }

   std::string operation(argv[2]);
   std::transform(operation.begin(), operation.end(), operation.begin(), tolower);

   if (operation == "texture")
      return texture(argc, argv);
   else if (operation == "sprite")
      return sprite(argc, argv);
   else if (operation == "obj")
      return obj_mesh(argc, argv);
   else if (operation == "tileset")
      return tilespec(argc, argv);
   else if (operation == "texfont")
      return texfont(argc, argv);
   else
   {
      std::cerr << "Unrecognized operation!" << std::endl;
      return -1;
   }
}

int main(int argc, char** argv)
{
   int result = runApp(argc, argv);

   if (result < 0)
      std::cout << std::endl
                << "Usage: " << std::endl
                << "   " << (argc > 0 ? argv[0] : "CCAssets") << " <database file> <operation> <operation parameters>" << std::endl
                << std::endl
                << "Operations:" << std::endl
                << "   texture   Create a texture asset by embedding an image file." << std::endl
                << "   sprite    Specify texture and texture coordinates for a sprite asset." << std::endl
                << "   obj       Create a mesh asset from a Wavefront .OBJ file (only v, vn, vt, f supported)." << std::endl
                << "   tileset   Create a tileset asset from a tilespec file." << std::endl
                << "   texfont   Create a texture font asset from a fontspec file." << std::endl;

   return result;
}

