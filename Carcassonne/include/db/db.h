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
// File: db.h
//
// RAII wrapper for SQLite's sqlite3 API.  DB objects are non-copyable and 
// non-moveable and represent an open connection with an sqlite database file.

#ifndef CARCASSONNE_DB_DB_H_
#define CARCASSONNE_DB_DB_H_
#include "_carcassonne.h"

#include <string>
#include "sqlite3.h"

namespace carcassonne {

namespace db {

namespace detail {
class _db_error;
}

class Stmt;

class DB
{
   friend class Stmt;
public:
   typedef detail::_db_error error;

   DB();
   explicit DB(const std::string& path);
   explicit DB(const std::string& path, int flags);
   explicit DB(const std::string& path, int flags, const std::string& vfs_name);
   ~DB();

   void begin();
   void commit();
   void rollback();

   void vacuum();

   void exec(const std::string& sql);

   int getInt(const std::string& sql, int default_value);

private:
   sqlite3* db_;

   DB(const DB&);
   void operator=(const DB&);
};

//namespace detail {

class detail::_db_error : public std::runtime_error
{
public:
   explicit _db_error(const std::string& what_arg);
   explicit _db_error(const char* what_arg);
};

//} // namespace detail

} // namespace db

} // namespace carcassonne

#include "db/db.inl"

#endif