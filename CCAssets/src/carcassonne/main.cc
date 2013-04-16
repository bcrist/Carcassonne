#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <map>

#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"
#include "carcassonne/db/transaction.h"

/*
bool operator<(const glm::vec3& a, const glm::vec3& b)
{
   if (a.x < b.x)
      return true;
   else if (a.x == b.x)
   {
      if (a.y < b.y)
         return true;
      else if (a.y == b.y && a.z < b.z)
         return true;
   }
   return false;
}
*/

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
   float x(atof(argv[4]));
   float y(atof(argv[5]));
   float w(atof(argv[6]));
   float h(atof(argv[7]));

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

int mesh(int argc, char** argv)
{
   std::string filename(argv[1]);

   if (argc < 6)
   {
      std::cout << std::endl
               << "Usage: " << std::endl
               << "   " << (argc > 0 ? argv[0] : "CCAssets") << " \"" << filename
               << "\" mesh <mesh name> <texture name> <OBJ filename>" << std::endl;
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

      

      carcassonne::db::Stmt s_mesh(db, "INSERT OR REPLACE INTO cc_meshes (name, primitive_type, texture, indices, vertices, normals, texture_coords) VALUES (?, ?, ?, ?, ?, ?, ?);");
      carcassonne::db::Stmt s_data(db, "INSERT OR REPLACE INTO cc_mesh_data (mesh_id, type, n, x, y, z) VALUES (?, ?, ?, ?, ?, ?);");

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
            {
               v_map[next_v_index++] = i - vertices.begin();
            }
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
            {
               n_map[next_n_index++] = i - normals.begin();
            }
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
            {
               t_map[next_t_index++] = i - texture_coords.begin();
            }
         }
         else if (operation == "f")
         {
            std::cout << "f: ";
            
            glm::ivec3 i;

            std::string str;
            {
               std::getline(iss, str, '/');
               std::istringstream iss2(str);
               iss2 >> i.x;
               i.x = v_map[i.x];
            }

            {
               std::getline(iss, str, '/');
               std::istringstream iss2(str);
               iss2 >> i.y;
               i.x = n_map[i.y];
            }

            {
               std::getline(iss, str, '/');
               std::istringstream iss2(str);
               iss2 >> i.z;
            }

            std::cout << i.x << ',' << i.y << ',' << i.z;

            std::cout << std::endl;

         }
      }




      ifs.close();

      

      /*update.bind(1, tex_name);
      update.bindBlob(2, content.data(), content.size());
      update.step();

      transaction.commit();*/
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
   else if (operation == "mesh")
      return mesh(argc, argv);
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
