#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

class Database {

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	int rowCount = 0;

	public:
		inline Database();
		inline void OpenDatabase(std::string dbName);
		inline void CloseDatabase();
		inline void CreateSqlDatabase(std::string dbName);
		inline void InsertOrReplace(std::string sumName, std::string rank, int leaguePoints, std::string tableName, std::string dbName);
		inline void CreateTable(std::string tableName, std::string dbName);
		inline void CreateUniqueIndex(std::string tableName, std::string dbName);
		inline bool TableExists(char *query, std::string dbName);
		inline bool query(std::string str, std::string dbName);
		inline void Update(std::string sumName, std::string rank, int lp, std::string tableName, std::string dbName);
};
#endif