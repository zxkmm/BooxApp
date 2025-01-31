/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#include <iostream>

#include "../DBRunnables.h"
#include "../../sqldb/implsqlite/SQLiteFactory.h"


SaveRecentBooksRunnable::SaveRecentBooksRunnable(DBConnection &connection) {
	myClearRecentBooks = SQLiteFactory::createCommand(BooksDBQuery::CLEAR_RECENT_BOOKS, connection);
	myInsertRecentBooks = SQLiteFactory::createCommand(BooksDBQuery::INSERT_RECENT_BOOKS, connection, "@book_id", DBValue::DBINT);
}

bool SaveRecentBooksRunnable::run() {
	if (!myClearRecentBooks->execute()) {
		return false;
	}
	DBIntValue &insertBookId = (DBIntValue &) *myInsertRecentBooks->parameter("@book_id").value();
	for (std::vector<fb::shared_ptr<DBBook> >::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		fb::shared_ptr<DBBook> book = (*it);
if (book->bookId() == 0) { // TODO: remove debug code
	std::cout << "SaveRecentBooksRunnable::run(): bookId == 0 in \"" << book->fileName() << "\"" << std::endl;
	return false;
}
		insertBookId = book->bookId();
		if (!myInsertRecentBooks->execute()) {
			return false;
		}
	}
	return true;
}

