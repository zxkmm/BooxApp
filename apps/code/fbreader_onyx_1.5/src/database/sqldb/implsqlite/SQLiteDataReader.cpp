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


#include "SQLiteDataReader.h"


fb::shared_ptr<DBValue> SQLiteDataReader::makeDBValue(sqlite3_stmt *statement, int column) {
	if (column < 0 || column >= sqlite3_column_count(statement)) {
		return 0;
	}
	const int type = sqlite3_column_type(statement, column);
	switch (type) {
	case SQLITE_INTEGER: return new DBIntValue(sqlite3_column_int(statement, column));
	case SQLITE_FLOAT:   return new DBRealValue(sqlite3_column_double(statement, column));
	case SQLITE_TEXT:    return new DBTextValue((const char *) sqlite3_column_text(statement, column));
	case SQLITE_NULL:    return DBValue::create(DBValue::DBNULL);
	}
	return 0;
}


SQLiteDataReader::SQLiteDataReader(SQLiteCommand &command) : 
	myCommand(command), 
	myCurrentStatement(0),
	myLocked(true) {
}

SQLiteDataReader::~SQLiteDataReader() {
	close();
}


bool SQLiteDataReader::next() {
	std::vector<sqlite3_stmt *> statements = myCommand.statements();
	while (true) {
		int res = sqlite3_step(statements[myCurrentStatement]);
		switch (res) {
		case SQLITE_OK:
			break;
		case SQLITE_ROW:
			return true;
		case SQLITE_DONE:
			if (++myCurrentStatement >= statements.size()) {
				return false;
			}
			break;
		default:
			myCommand.dumpError();
			return false;
		}
	}	
}

bool SQLiteDataReader::reset() {
	return myCommand.resetStatements();
}

void SQLiteDataReader::close() {
	if (myLocked) {
		reset();
		myCommand.unlock();
		myLocked = false;
	}
}

unsigned SQLiteDataReader::columnsNumber() const {
	sqlite3_stmt *statement = currentStatement();
	return sqlite3_column_count(statement);
}

DBValue::ValueType SQLiteDataReader::type(unsigned column) const {
	sqlite3_stmt *statement = currentStatement();
	if ((int) column >= sqlite3_column_count(statement)) {
		return DBValue::DBNULL;
	}
	const int type = sqlite3_column_type(statement, column);
	switch (type) {
	case SQLITE_INTEGER: return DBValue::DBINT;
	case SQLITE_FLOAT:   return DBValue::DBREAL;
	case SQLITE_TEXT:    return DBValue::DBTEXT;
	case SQLITE_NULL:    return DBValue::DBNULL;
	default:
		return DBValue::DBNULL;
	}
}

fb::shared_ptr<DBValue> SQLiteDataReader::value(unsigned column) const {
	sqlite3_stmt *statement = currentStatement();
	return makeDBValue(statement, column);
}

int SQLiteDataReader::intValue(unsigned column) const {
	sqlite3_stmt *statement = currentStatement();
	if ((int) column >= sqlite3_column_count(statement)) {
		return 0;
	}
	return sqlite3_column_int(statement, column);
}

double SQLiteDataReader::realValue(unsigned column) const {
	sqlite3_stmt *statement = currentStatement();
	if ((int) column >= sqlite3_column_count(statement)) {
		return 0;
	}
	return sqlite3_column_double(statement, column);
}

std::string SQLiteDataReader::textValue(unsigned column) const {
	static const std::string _NULL_STR("NULL");
	sqlite3_stmt *statement = currentStatement();
	if ((int) column >= sqlite3_column_count(statement)) {
		return "";
	}
	const char *res = (const char *) sqlite3_column_text(statement, column);
	if (res == 0) {
		return _NULL_STR;
	}
	return res;
}

