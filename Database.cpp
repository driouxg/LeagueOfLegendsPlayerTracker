#include <sqlite3.h> 
#include <iostream>
#include "Database.h"

#define SQLITE_OK 0

Database::Database() {
	// Constructor
}

static int callback(void *rowCount, int argc, char **argv, char **azColName) {

	int i;
	for (i = 0; i<argc; i++)
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

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

void Database::CreateTable(std::string tableName, std::string dbName) {

	// First check to see if the table exists
	std::string str1 = "SELECT count(*) FROM '" + tableName + "';";

	char *quer = (char*)str1.c_str();

	//TODO: Get TableExists function to work correctly
	if (TableExists(quer, dbName))
		std::cout << "Table Already Exists" << std::endl;
	else
	{
		//std::cout << "Creating table " << tableName << std::endl;
		std::string str = "CREATE TABLE " + tableName + "(sumName TEXT, rank TEXT NOT NULL, leaguePoints INTEGER NOT NULL)";
		query(str, dbName);
		CreateUniqueIndex(tableName, dbName);
	}
}

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
	rc = sqlite3_exec(db, sql1, callback, NULL, &zErrMsg);

	if (rc != SQLITE_OK) {
		//fprintf(stderr, "SQL error: %s\n", zErrMsg);		<--- unsilence query errors here
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

bool Database::TableExists(char *query, std::string dbName) {

	/*OpenDatabase(dbName);

	rc = sqlite3_exec(db, query, callback, &rowCount, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);

	std::cout << "ROWCOUNT: " + rowCount << std::endl;

	if (rowCount >= 1)
		return true;
	else*/
		return false;
}
