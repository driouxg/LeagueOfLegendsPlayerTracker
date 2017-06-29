#include "HttpRestClient.cpp"
#include "Database.cpp"
#include "FileManager.cpp"
#include <conio.h>
#include <windows.h>

#define SUM_ID_URL L"https://na1.api.riotgames.com/lol/summoner/v3/summoners/by-name/"
#define SUM_RANK_URL L"https://na1.api.riotgames.com/lol/league/v3/positions/by-summoner/"
#define API_KEY L"?api_key=RGAPI-6bb999dc-0aea-4aa3-98d8-c81764b0784a"
#define TABLE_NAME "LEAGUEOFLEGENDS"
#define DATABASE_NAME "database.db"
#define FILE_NAME "sumNames.txt"

int wmain()
{
	HttpRestClient *client = new HttpRestClient();
	FileManager *fileMang = new FileManager();
	Database *db = new Database();
	std::string stringRank;
	utility::string_t rank;
	utility::string_t s;
	std::string t;
	int sumID;

	std::vector<std::string> sumNames;
	fileMang->GetSumNames(FILE_NAME, sumNames);

	int i = 0;
	try {
		while (i < sumNames.size()) {
			client->BuildUrl(SUM_ID_URL, utility::conversions::to_string_t(sumNames[i]), API_KEY);

			sumID = client->GetSummonerID();

			std::cout << "Updating Summoner: " << client->sumName << std::endl;

			//Convert sumID to utility::string_t data type
			t = std::to_string(sumID);
			s = utility::conversions::to_string_t(t);

			client->BuildUrl(SUM_RANK_URL, s, API_KEY);

			rank = client->GetSummonerRank();

			// Convert string_t to const char
			stringRank = utility::conversions::to_utf8string(rank);

			// Create Database, table, column, and insert values into columns
			db->CreateSqlDatabase(DATABASE_NAME);
			db->CreateTable(TABLE_NAME, DATABASE_NAME);
			db->InsertOrReplace(client->sumName, stringRank, client->leaguePoints, TABLE_NAME, DATABASE_NAME);	// <---- Causing read access violations
			db->Update(client->sumName, stringRank, client->leaguePoints, TABLE_NAME, DATABASE_NAME);

			//db->query("SELECT sumName FROM LEAGUEOFLEGENDS", DATABASE_NAME);
			i++;

			//Infinite Loop, not good coding practice
			if (i == sumNames.size())
				i = 0;

			//Sleep to prevent overuse of api key
			Sleep(2500);
		}
	}
	catch (const std::exception& e) {
		std::wcout << e.what() << std::endl;
	}
	// Halt console
	_getch();
}