#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"
#include "carcassonne/db/transaction.h"

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

   if (operation == "")
      return 0; //return texture(argc, argv);
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
                << "   " << (argc > 0 ? argv[0] : "CCConfig") << " <database file> <operation> <operation parameters>" << std::endl;

   return result;
}
