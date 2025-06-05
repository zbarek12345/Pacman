//
// Created by Wiktor on 29.03.2025.
//

#include "DatabaseController.h"

#include <vector>

DatabaseController::DatabaseController(std::string path) {
	_rc = sqlite3_open_v2(path.c_str(), &_db, SQLITE_OPEN_READWRITE, nullptr);
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
		out.best_score = sqlite3_column_int(_stmt, 2);
		out.best_time = sqlite3_column_double(_stmt, 3);
	}

	return out;
}

void DatabaseController::updateLevel(level stats) {
	// Build the SQL update query
	std::string query = "UPDATE Levels SET ";
	std::vector<std::string> updates;

	// Add the fields that need to be updated
	if (stats.best_score > 0) {
		updates.push_back("best_score = " + std::to_string(stats.best_score));
	}

	if (stats.best_time > 0) {
		updates.push_back("best_time = " + std::to_string(stats.best_time));
	}

	// If no fields to update, return early
	if (updates.empty()) {
		return;
	}

	// Join the updates with commas
	query += updates[0];
	for (size_t i = 1; i < updates.size(); ++i) {
		query += ", " + updates[i];
	}

	// Add the WHERE clause to identify the level
	query += " WHERE level = " + std::to_string(stats.id) + ";";

	// Prepare and execute the query
	auto ret = sqlite3_prepare_v2(_db, query.c_str(), -1, &_stmt, nullptr);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(_db));
		return;
	}

	if (sqlite3_step(_stmt) != SQLITE_DONE) {
		fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(_db));
	}

	sqlite3_finalize(_stmt);
}
