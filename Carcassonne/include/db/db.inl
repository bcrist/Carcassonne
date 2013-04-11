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
// File: db.inl
//
// RAII wrapper for SQLite's sqlite3 API.  DB objects are non-copyable and 
// non-moveable and represent an open connection with an sqlite database file.

#ifndef CARCASSONNE_DB_DB_INL_
#define CARCASSONNE_DB_DB_INL_

#ifndef CARCASSONNE_DB_DB_H_
#include "db/db.h"
#endif

namespace carcassonne {

namespace db {

///////////////////////////////////////////////////////////////////////////////
// constructors for exception objects (thin wrapper for std::runtime_error)
inline detail::_db_error::_db_error(const std::string& what_arg)
   : std::runtime_error(what_arg)
{
}
inline detail::_db_error::_db_error(const char* what_arg)
   : std::runtime_error(what_arg)
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructs a new in-memory database.
inline DB::DB()
{
   if (sqlite3_open(":memory:", &db_) != SQLITE_OK)
   {
      error e(sqlite3_errmsg(db_));
      sqlite3_close(db_);
      throw e;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Opens the specified database file for R/W access, creating it if it does not
// exist.
inline DB::DB(const std::string& path)
{
   if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK)
   {
      error e(sqlite3_errmsg(db_));
      sqlite3_close(db_);
      throw e;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Opens a database file, using the specified sqlite3_open_v2() flag bitfield.
inline DB::DB(const std::string& path, int flags)
{
   if (sqlite3_open_v2(path.c_str(), &db_, flags, nullptr) != SQLITE_OK)
   {
      error e(sqlite3_errmsg(db_));
      sqlite3_close(db_);
      throw e;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Opens a database file using the specified sqlite3_open_v2() flag bitfield
// and vfs name.
inline DB::DB(const std::string& path, int flags, const std::string& vfs_name)
{
   if (sqlite3_open_v2(path.c_str(), &db_, flags, vfs_name.c_str()) != SQLITE_OK)
   {
      error e(sqlite3_errmsg(db_));
      sqlite3_close(db_);
      throw e;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Convienience function for starting a new transaction.
// NOTE: Consider using a Transaction object as an RAII-safe alternative
inline void DB::begin()
{
   exec("BEGIN");
}

///////////////////////////////////////////////////////////////////////////////
// Convienience function for comitting a previously started transaction.
// NOTE: Consider using a Transaction object as an RAII-safe alternative
inline void DB::commit()
{
   exec("END");
}

///////////////////////////////////////////////////////////////////////////////
// Convienience function for rolling back a previously started transaction.
// NOTE: Consider using a Transaction object as an RAII-safe alternative
inline void DB::rollback()
{
   exec("ROLLBACK");
}

///////////////////////////////////////////////////////////////////////////////
// Convienience function for vacuuming the SQLite database (defragments
// data pages)
inline void DB::vacuum()
{
   exec("VACUUM");
}

} // namespace db

} // namespace carcassonne

#endif