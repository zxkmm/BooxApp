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


#ifndef __SQLITEDATAREADER_H__
#define __SQLITEDATAREADER_H__

#include "../DBDataReader.h"

#include "SQLiteCommand.h"
#include "SQLiteConnection.h"

class SQLiteDataReader : public DBDataReader {

public:
	static fb::shared_ptr<DBValue> makeDBValue(sqlite3_stmt *statement, int column);

public:
	SQLiteDataReader(SQLiteCommand &command);
	~SQLiteDataReader();

	bool next();
	bool reset();
	void close();

	unsigned columnsNumber() const;

	DBValue::ValueType type(unsigned column) const;

	fb::shared_ptr<DBValue> value(unsigned column) const;

	int intValue(unsigned column) const;
	double realValue(unsigned column) const;
	std::string textValue(unsigned column) const;
	
private:
	sqlite3_stmt *currentStatement() const;

private:
	SQLiteCommand &myCommand;
	unsigned myCurrentStatement;
	bool myLocked;
};


inline sqlite3_stmt *SQLiteDataReader::currentStatement() const { return myCommand.statements()[myCurrentStatement]; }

#endif /* __SQLITEDATAREADER_H__ */

