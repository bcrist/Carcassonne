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
// File: carcassonne/db/transaction.h
//
// RAII transaction wrapper for DB and Stmt objects.  When a Transaction object
// is destroyed (goes out of scope) it will automatically roll back the transaction
// if it has not been comitted yet.

#ifndef CARCASSONNE_DB_TRANSACTION_H_
#define CARCASSONNE_DB_TRANSACTION_H_
#include "carcassonne/_carcassonne.h"

#include "carcassonne/db/db.h"

namespace carcassonne {
namespace db {

class Transaction
{   
public:
   enum TransactionType { TT_DEFERRED, TT_IMMEDIATE, TT_EXCLUSIVE };

   // create a new deferred transaction
   explicit Transaction(DB& db);
   // create a new transaction
   explicit Transaction(DB& db, TransactionType t);
   // destructor
   ~Transaction();

   void commit();
   void rollback();

private:
   DB& db_;
   bool pending_;

   Transaction(const Transaction&);
   void operator=(const Transaction&);
};

} // namespace carcassonne::db
} // namespace carcassonne

#include "carcassonne/db/transaction.inl"

#endif