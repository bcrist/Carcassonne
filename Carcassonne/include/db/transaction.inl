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
// File: transaction.inl
//
// RAII transaction wrapper for DB and Stmt objects.  When a Transaction object
// is destroyed (goes out of scope) it will automatically roll back the transaction
// if it has not been comitted yet.

#ifndef CARCASSONNE_DB_TRANSACTION_INL_
#define CARCASSONNE_DB_TRANSACTION_INL_

#ifndef CARCASSONNE_DB_TRANSACTION_H_
#include "db/transaction.h"
#endif

namespace carcassonne {

namespace db {

inline Transaction::Transaction(DB& db)
   : db_(db),
     pending_(true)
{
   db_.begin();
}

inline Transaction::Transaction(DB& db, TransactionType t)
   : db_(db),
     pending_(true)
{
   const char* sql;
   switch (t)
   {
      case TT_IMMEDIATE:
         sql = "BEGIN IMMEDIATE";
         break;

      case TT_EXCLUSIVE:
         sql = "BEGIN EXCLUSIVE";
         break;

      case TT_DEFERRED:
         sql = "BEGIN DEFERRED";
         break;

      default:
         sql = "BEGIN";
         break;
   }
   db_.exec(sql);
}

inline Transaction::~Transaction()
{
   if (pending_)
   {
      try
      {
         rollback();
      }
      catch (const DB::error&)
      {
      }
   }
}

inline void Transaction::commit()
{
   db_.commit();
}

inline void Transaction::rollback()
{
   db_.rollback();
}

} // namespace db

} // namespace carcassonne

#endif