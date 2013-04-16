#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"
#include "carcassonne/db/transaction.h"

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
              "index INTEGER, "
              "x NUMERIC, "
              "y NUMERIC, "
              "z NUMERIC, "
              "PRIMARY KEY (mesh_id, type, index));");

      

      carcassonne::db::Stmt s_mesh(db, "INSERT OR REPLACE INTO cc_meshes (name, primitive_type, texture, indices, vertices, normals, texture_coords) VALUES (?, ?, ?, ?, ?, ?, ?);");
      carcassonne::db::Stmt s_data(db, "INSERT OR REPLACE INTO cc_mesh_data (mesh_id, type, index, x, y, z) VALUES (?, ?, ?, ?, ?, ?);");



      std::ifstream ifs(source, std::ifstream::in);
      




      ifs.close();

      

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
