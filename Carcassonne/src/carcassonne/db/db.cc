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
// File: carcassonne/db/db.cc
//
// RAII wrapper for SQLite's sqlite3 API.  DB objects are non-copyable and 
// non-moveable and represent an open connection with an sqlite database file.

#include "carcassonne/db/db.h"

#include <cassert>

#include "carcassonne/db/stmt.h"

namespace carcassonne {
namespace db {

///////////////////////////////////////////////////////////////////////////////
// Cleans up the DB object.  Assertion failure will result if there are still
// statement objects attached to this database that have not been destroyed.
// It is up to the programmer to ensure that Stmt objects have smaller scope
// than DB objects.
DB::~DB()
{
   assert(sqlite3_next_stmt(db_, NULL) == NULL);
   int result = sqlite3_close(db_);
   assert(result == SQLITE_OK);
}

///////////////////////////////////////////////////////////////////////////////
// Compiles & executes one or more SQL queries on the database, discarding any
// result sets that may be returned.
void DB::exec(const std::string& sql)
{
   char* err;
   int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
   if (result != SQLITE_OK)
   {
      if (err)
         throw error(err);
      else
#if defined(_MSC_VER) && _MSC_VER <= 1600
         // VC10 doesn't include std::to_string(int)
         throw error("sqlite3_exec() returned " + std::to_string(static_cast<_Longlong>(result)));
#else
         throw error("sqlite3_exec() returned " + std::to_string(result));
#endif
   }
}

///////////////////////////////////////////////////////////////////////////////
// Compiles & executes an SQL query which generates at least one row whose
// first field is convertible to integer, and returns that integer.  If no
// rows are returned by the query, returns default_value
int DB::getInt(const std::string& sql, int default_value)
{
   db::Stmt stmt(*this, sql);
   if (stmt.step())
      return stmt.getInt(0);

   return default_value;
}

} // namespace carcassonne::db
} // namespace carcassonne
