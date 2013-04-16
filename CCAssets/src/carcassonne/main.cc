#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <map>

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

      carcassonne::db::Stmt update(db, "INSERT OR REPLACE INTO cc_textures (name, data) VALUES (?, ?);");
      update.bind(1, tex_name);
      update.bindBlob(2, content.data(), content.size());
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

      db.exec("CREATE TABLE IF NOT EXISTS cc_sprites ("
              "name TEXT PRIMARY KEY, "
              "texture TEXT, "
              "x NUMERIC, y NUMERIC, "
              "width NUMERIC, height NUMERIC)");

      for (int i = 8; i < argc; ++i)
      {
         std::string sprite_name(argv[i]);

         carcassonne::db::Stmt update(db, "INSERT OR REPLACE INTO cc_sprites (name, texture, x, y, width, height) VALUES (?, ?, ?, ?, ?, ?);");
         update.bind(1, sprite_name);
         update.bind(2, tex_name);
         update.bind(3, x);
         update.bind(4, y);
         update.bind(5, w);
         update.bind(6, h);
         update.step();

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
         s_data.bind(5, i->y);
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
                << "   " << (argc > 0 ? argv[0] : "CCAssets") << " <database file> <operation> <operation parameters>" << std::endl;

   return result;
}
