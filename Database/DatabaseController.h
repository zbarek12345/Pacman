//
// Created by Wiktor on 29.03.2025.
//

#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H
#include <sqlite3.h>
#include <string>
#include <vector>

class DatabaseController {
	sqlite3 *_db;
	sqlite3_stmt *_stmt;
	int _rc;

public:

	///Level instance descriptor
	struct level {
		int id;
		std::string map;
		int best_score;
		double best_time;
	};

	/// Initializes Database connection, for db file
	/// @param path Path to database file
	DatabaseController(std::string path);
	~DatabaseController();

	///
	/// @return list of levels
	std::vector<std::string> getLevels();

	///
	/// @param level identifier of level
	/// @return level descriptor object
	level getLevel(int level);
};
#endif //DATABASECONTROLLER_H
