#include <sqlite3.h> 
#include <iostream>
#include "Database.h"

#define SQLITE_OK 0

Database::Database() {
	// Constructor
}

static int callbackTableExists(void *rowCount, int argc, char **argv, char **azColName) {

	if (argc == 1 && argv)
		*static_cast<int*>(rowCount) = atoi(argv[0]);

	printf("\n");
	return 0;
}

void Database::OpenDatabase(std::string dbName) {
	const char *c_dbName = dbName.c_str();

	this->rc = sqlite3_open(c_dbName, &db);

	if (rc)
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
}

void Database::CloseDatabase() {
	sqlite3_close(this->db);

	//Check for error opening database
	if (rc)
		fprintf(stderr, "Can't close database: %s\n", sqlite3_errmsg(db));
}

/**
 * Simply opens and closes an sqlite database, which in turn instantiates the database for use.
 */
void Database::CreateSqlDatabase(std::string dbName) {
	OpenDatabase(dbName);
	CloseDatabase();
}

/**
 * Updates the rank and leaguePoints for a specified sumName value
 */
void Database::Update(std::string sumName, std::string rank, int leaguePoints, std::string tableName, std::string dbName) {

	//Update Rank
	std::string str = "UPDATE " + tableName + " SET rank = '" + rank + "' WHERE sumName = '" + sumName + "'";
	query(str, dbName);

	//Update LeaguePoints
	str = "UPDATE " + tableName + " SET leaguePoints = '" + std::to_string(leaguePoints) + "' WHERE sumName = '" + sumName + "'";
	query(str, dbName);
}

void Database::InsertOrReplace(std::string sumName, std::string rank, int leaguePoints, std::string tableName, std::string dbName) {

	std::string str = "INSERT OR REPLACE INTO " + tableName + " (sumName, rank, leaguePoints) " +
		" VALUES ('" + sumName + "', COALESCE((SELECT rank FROM LEAGUEOFLEGENDS WHERE sumName = '" + sumName + "'), '" + rank + "')" +
		", COALESCE((SELECT leaguePoints FROM LEAGUEOFLEGENDS WHERE sumName = '" + sumName + "'), '" + std::to_string(leaguePoints) + "'));";

	query(str, dbName);
}

/**
 * If the specified table does exist, Create it and create and unique index as well.
 */
void Database::CreateTable(std::string tableName, std::string dbName) {

	// First check to see if the table exists
	if (!TableExists(tableName, dbName)) {
		std::cout << "Creating table " << tableName << std::endl;
		std::string str = "CREATE TABLE " + tableName + "(sumName TEXT, rank TEXT NOT NULL, leaguePoints INTEGER NOT NULL)";
		query(str, dbName);
		CreateUniqueIndex(tableName, dbName);
	}
}

/**
 * Check if unique index already exists. If no unique index exists, create one.
 */
void Database::CreateUniqueIndex(std::string tableName, std::string dbName) {

	std::string str = "CREATE UNIQUE INDEX summoner_index ON " + tableName + "(sumName, rank, leaguePoints)";
	query(str, dbName);
}

/**
 * Performs the SQL query and reports the cooresponding error, if there is one.
 */
bool Database::query(std::string sql, std::string dbName) {

	char *sql1 = (char*)sql.c_str();

	OpenDatabase(dbName);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql1, callbackTableExists, NULL, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return false;
	}
	else {
		//fprintf(stdout, "Operation done successfully\n");
		sqlite3_close(db);
		return true;
	}
}

/**
 * Executes a query, which checks to see if any rows exist in a specified database. If any rows exist, 
 * function returns true, else false.
 */
bool Database::TableExists(std::string tableName, std::string dbName) {

	std::string query = "SELECT count(*) FROM '" + tableName + "';";

	OpenDatabase(dbName);

	rc = sqlite3_exec(db, (char*)query.c_str(), callbackTableExists, &rowCount, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);

	if (rowCount >= 1)
		return true;
	else
		return false;
}
