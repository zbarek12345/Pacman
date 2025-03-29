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

	struct level {
		int id;
		std::string map;
		int best_score;
		double best_time;
	};

	DatabaseController(std::string path);
	~DatabaseController();

	std::vector<std::string> getLevels();

	level getLevel(int level);
};
#endif //DATABASECONTROLLER_H
