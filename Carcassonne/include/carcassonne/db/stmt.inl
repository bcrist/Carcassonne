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
// File: stmt.inl
//
// RAII wrapper for SQLite's sqlite3_stmt API.  Stmt objects are non-copyable and 
// non-moveable and represent a prepared statement -- a compiled SQL query -- along
// with its currently bound parameters and the resultset returned when the query is
// executed.

#ifndef CARCASSONNE_DB_STMT_INL_
#define CARCASSONNE_DB_STMT_INL_

#ifndef CARCASSONNE_DB_STMT_H_
#include "carcassonne/db/stmt.h"
#endif

namespace carcassonne {

namespace db {


///////////////////////////////////////////////////////////////////////////////
// Compiles the provided SQL query against the provided database and constructs
// a Stmt object to represent it
inline Stmt::Stmt(DB& db, const std::string &sql)
   : db_(db)
{
   if (sqlite3_prepare_v2(db.db_, sql.c_str(), sql.length() + 1, &stmt_, nullptr) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db.db_)).append(1, '\n').append(sql));
}

///////////////////////////////////////////////////////////////////////////////
// Destroys a Stmt object.  All Stmt objects must be destroyed before their
// associated DB object can be destroyed.  Any other behavior is a programming
// error, and may result in an assertion failure.
inline Stmt::~Stmt()
{
   sqlite3_finalize(stmt_);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the DB object associated with this prepared statement.
inline DB& Stmt::getDB()
{
   return db_;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the uncompiled text of the SQL query.
inline const char* Stmt::getSQL()
{
   return sqlite3_sql(stmt_);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the number of bindable parameters present in the query.
// Parameter indices start at 1, so this is actually the largest valid
// parameter index which can be used.
inline int Stmt::parameters()
{
   return sqlite3_bind_parameter_count(stmt_);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the index of a named parameter in the query
inline int Stmt::parameter(const std::string& name)
{
   int result = sqlite3_bind_parameter_index(stmt_, name.c_str());
   assert(result != 0);
   return result;
}

///////////////////////////////////////////////////////////////////////////////
// Clears all parameter bindings
inline void Stmt::bind()
{
   if (sqlite3_clear_bindings(stmt_) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Clears the specified parameter binding.
inline void Stmt::bind(int parameter)
{
   if (sqlite3_bind_null(stmt_, parameter) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds an integer value to the specified parameter.
inline void Stmt::bind(int parameter, int value)
{
   if (sqlite3_bind_int(stmt_, parameter, value) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a 64-bit integer value to the specified parameter.
inline void Stmt::bind(int parameter, sqlite3_int64 value)
{
   if (sqlite3_bind_int64(stmt_, parameter, value) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a 64-bit floating point value to the specified parameter.
inline void Stmt::bind(int parameter, double value)
{
   if (sqlite3_bind_double(stmt_, parameter, value) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a text string value to the specified parameter.
inline void Stmt::bind(int parameter, const std::string& value)
{
   if (sqlite3_bind_text(stmt_, parameter, value.c_str(), value.length(), SQLITE_TRANSIENT) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}
inline void Stmt::bind(int parameter, const char* value)
{
   if (sqlite3_bind_text(stmt_, parameter, value, -1, SQLITE_TRANSIENT) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a text string value to the specified parameter.  The parameter
// must be located in static memory. (it may be accessed at a later point)
inline void Stmt::bind_s(int parameter, const char* value)
{
   if (sqlite3_bind_text(stmt_, parameter, value, -1, SQLITE_STATIC) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a binary string value to the specified parameter.
inline void Stmt::bindBlob(int parameter, const std::string& value)
{
   if (sqlite3_bind_blob(stmt_, parameter, value.c_str(), value.length(), SQLITE_TRANSIENT) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}
inline void Stmt::bindBlob(int parameter, const void* value, int length)
{
   if (sqlite3_bind_blob(stmt_, parameter, value, length, SQLITE_TRANSIENT) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Binds a binary string value to the specified parameter. The parameter
// must be located in static memory. (it may be accessed at a later point)
inline void Stmt::bindBlob_s(int parameter, const void* value, int length)
{
   if (sqlite3_bind_blob(stmt_, parameter, value, length, SQLITE_STATIC) != SQLITE_OK)
      throw DB::error(std::string(sqlite3_errmsg(db_.db_)).append(1, '\n').append(sqlite3_sql(stmt_)));
}

///////////////////////////////////////////////////////////////////////////////
// Resets the statement after it has been executed so that it can be executed
// again (with different parameters perhaps)
inline void Stmt::reset()
{
   sqlite3_reset(stmt_);
}

///////////////////////////////////////////////////////////////////////////////
// returns the number of columns in the result set
inline int Stmt::columns()
{
   return sqlite3_column_count(stmt_);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the SQLite type code of the column specified
inline int Stmt::getType(int column)
{
   assert(column >= 0 && column < columns());
   return sqlite3_column_type(stmt_, column);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified column as a 64-bit floating point value
inline double Stmt::getDouble(int column)
{
   assert(column >= 0 && column < columns());
   return sqlite3_column_double(stmt_, column);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified column as an integer value
inline int Stmt::getInt(int column)
{
   assert(column >= 0 && column < columns());
   return sqlite3_column_int(stmt_, column);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified column as a 64-bit integer value
inline sqlite3_int64 Stmt::getLong(int column)
{
   assert(column >= 0 && column < columns());
   return sqlite3_column_int64(stmt_, column);
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified column as a text string value
inline const char* Stmt::getText(int column)
{
   assert(column >= 0 && column < columns());
   return reinterpret_cast<const char*>(sqlite3_column_text(stmt_, column));
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified column as a binary string value
inline std::string Stmt::getBlob(int column)
{
   assert(column >= 0 && column < columns());
   int bytes = sqlite3_column_bytes(stmt_, column);
   return std::string(reinterpret_cast<const char*>(sqlite3_column_blob(stmt_, column)), bytes);
}
inline int Stmt::getBlob(int column, const void*& dest)
{
   assert(column >= 0 && column < columns());
   int bytes = sqlite3_column_bytes(stmt_, column);
   dest = sqlite3_column_blob(stmt_, column);
   return bytes;
}

///////////////////////////////////////////////////////////////////////////////
// Prints the uncompiled SQL source of this object
inline std::ostream& operator<<(std::ostream& os, Stmt& s)
{
   return os << s.getSQL();
}

} // namespace db

} // namespace carcassonne

#endif