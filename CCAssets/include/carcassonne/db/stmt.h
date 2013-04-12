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

#ifndef CARCASSONNE_DB_STMT_H_
#define CARCASSONNE_DB_STMT_H_
#include "carcassonne/_carcassonne.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <cassert>
#include "sqlite3.h"

#include "carcassonne/db/db.h"

namespace carcassonne {

namespace db {

class Stmt;

std::ostream& operator<<(std::ostream& os, Stmt& s);

class Stmt
{
   friend std::ostream& operator<<(std::ostream& os, Stmt& s);
public:
   // standard constructor
   Stmt(DB& db, const std::string& sql);
   // destructor
   ~Stmt();

   DB& getDB();

   const char* getSQL();

   // get the index of the last valid parameter binding site
   int parameters();
   // get the index of a named parameter binding site
   int parameter(const std::string& name);

   // clear all bound parameters (binds them to NULL)
   void bind();
   // bind NULL to the specified parameter index
   void bind(int parameter);
   // bind an integer to the specified parameter index
   void bind(int parameter, int value);
   // bind a long integer to the specified parameter index
   void bind(int parameter, sqlite3_int64 value);
   // bind a double to the specified parameter index
   void bind(int parameter, double value);
   // bind text to the specified parameter index
   void bind(int parameter, const std::string& value);
   void bind(int parameter, const char* value);
   // bind text to the specified parameter index.  "value" must be
   // located in static storage (eg. a string literal)
   void bind_s(int parameter, const char* value);
   // bind a blob to the specified parameter index
   void bindBlob(int parameter, const std::string& value);
   void bindBlob(int parameter, const void* value, int length);
   // bind a blob to the specified parameter index.  "value" must be
   // located in static storage (eg. a string literal)
   void bindBlob_s(int parameter, const void* value, int length);

   // execute the statement.  Returns true if there are results
   // available to retrieve.
   bool step();
   // reset the statement.  Does not clear parameter bindings.
   void reset();

   // get the number of columns in the result set
   int columns();
   // get the index of the column with the given name.
   int column(const std::string& name);

   // returns the typecode of the data in the given column (one of
   // SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL).
   int getType(int column);

   // get a column's value as a double
   double getDouble(int column);
   // get a column's value as an int
   int getInt(int column);
   // get a column's value as a 64bit integer
   sqlite3_int64 getLong(int column);
   // get a column's value as UTF-8 text
   const char* getText(int column);
   // get a column's blob value and return it as a string
   std::string getBlob(int column);
   // get a column's blob value and set the provided pointer
   // to point to the data.  The number of bytes in the buffer is returned.
   int getBlob(int column, const void*& dest);

private:
   DB& db_;
   sqlite3_stmt* stmt_;
   std::unique_ptr<std::unordered_map<std::string, int> > col_names_;

   Stmt(const Stmt&);
   void operator=(const Stmt&);
};

} // namespace db

} // namespace carcassonne

#include "carcassonne/db/stmt.inl"

#endif