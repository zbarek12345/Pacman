//
// Created by Wiktor on 29.03.2025.
//

#include "DatabaseController.h"

#include <vector>

DatabaseController::DatabaseController(std::string path) {
	_rc = sqlite3_open_v2(path.c_str(), &_db, SQLITE_OPEN_READONLY, nullptr);
	if (_rc != SQLITE_OK) {
		// Handle the error here, e.g., log it or throw an exception
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(_db));
		sqlite3_close(_db); // Ensure the database connection is closed
		_db = nullptr;
	}
	_stmt = nullptr;
}

DatabaseController::~DatabaseController() {
	sqlite3_close(_db);
}

std::vector<std::string> DatabaseController::getLevels() {
	char msg[] = "Select level from Levels;";
	std::vector<std::string> out;
	auto ret = sqlite3_prepare_v2(_db, msg, -1, &_stmt, nullptr);

	if (ret != SQLITE_OK) {
		// Handle the error here, e.g., log it or throw an exception
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(_db));
		return {};
	}

	while (sqlite3_step(_stmt) == SQLITE_ROW) {
		out.emplace_back("Level " + std::to_string(sqlite3_column_int(_stmt, 0)));
	}

	sqlite3_finalize(_stmt);
	return out;
}

DatabaseController::level DatabaseController::getLevel(int level) {
	std::string msg = "Select * from Levels where level = " + std::to_string(level) + ";";
	DatabaseController::level out{};
	auto ret = sqlite3_prepare_v2(_db, msg.c_str(), -1, &_stmt, nullptr);

	if (ret != SQLITE_OK) {
		// Handle the error here, e.g., log it or throw an exception
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(_db));
		return {};
	}

	if (sqlite3_step(_stmt) == SQLITE_ROW) {
		out.id = sqlite3_column_int(_stmt, 0);
		out.map = reinterpret_cast<const char*>(sqlite3_column_text(_stmt, 1));
		out.best_score = sqlite3_column_int(_stmt, 3);
		out.best_time = sqlite3_column_double(_stmt, 4);
	}

	return out;
}
