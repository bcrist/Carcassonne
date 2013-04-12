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
// File: carcassonne/db/stmt.h
//
// RAII wrapper for SQLite's sqlite3_stmt API.  Stmt objects are non-copyable and 
// non-moveable and represent a prepared statement -- a compiled SQL query -- along
// with its currently bound parameters and the resultset returned when the query is
// executed.

#include "carcassonne/db/stmt.h"

namespace carcassonne {

namespace db {

///////////////////////////////////////////////////////////////////////////////
// Executes the statement.  Returns true if a result set was returned and
// there is a row available.
bool Stmt::step()
{
   int result = sqlite3_step(stmt_);
   col_names_.reset();

   if (result == SQLITE_DONE)
      return false;
   if (result == SQLITE_ROW)
      return true;

   // otherwise an error occurred
   if (result == SQLITE_BUSY)
      throw DB::error("Database is busy.");
   if (result == SQLITE_MISUSE)
      throw DB::error("Statement shouldn't be executed now!");
   throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Returns the column index of the column with the specified name.
// NOTE: this is MUCH slower than specifying column indices directly.
int Stmt::column(const std::string& name)
{
   if (!col_names_)
   {
      col_names_.reset(new std::unordered_map<std::string, int>());
      int cols = columns();
      for (int i = 0; i < cols; ++i)
         col_names_->insert(std::pair<std::string,int>(sqlite3_column_name(stmt_, i), i));
   }

   std::unordered_map<std::string, int>::iterator it = col_names_->find(name);
   if (it != col_names_->end())
      return it->second;
   throw DB::error(std::string("Column '").append(name).append("' not found!"));
}


} // namespace db

} // namespace carcassonne
