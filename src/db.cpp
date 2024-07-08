#include "db.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

// Define a structure for a table
struct Table
{
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> records;
};

// Global map to store tables
std::unordered_map<std::string, Table> tables;

// Constructor: Initialize with a filename
Database::Database(const std::string& filename) : filename(filename) {
    load(); // Load existing data if any
}

// Destructor: Save data when object is destroyed
Database::~Database(){
    save();
}

// Load the database from the file
void Database::load() {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    Table* current_table = nullptr;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line.rfind("#table:",0) == 0){ // Identify table start
            std::string table_name = line.substr(9);
            current_table = &tables[table_name];
        } else if (current_table) { // Read columns or records
            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> values;

            while (std::getline(ss,item, ',')) {
                values.push_back(item);
            }

            if (current_table->columns.empty()) {
                current_table->columns = values;
            } else {
                current_table->records.push_back(values);
            }
        }
    }

    file.close();
}

// Save the database to the file
void Database::save() {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& pair : tables) {
        file << "# table: " << pair.first << "\n";
        for (const auto& col : pair.second.columns) {
            file << col << ",";
        }
        file.seekp(-1,std::ios_base::cur); // Remove last comma
        file << "\n";

        for (const auto& record : pair.second.records) {
            for (const auto& val : record) {
                file << val << ",";
            }
            file.seekp(-1,std::ios_base::cur); // Remove last comma
            file << "\n";
        }
    }

    file.close();
}

// Create a new table
void Database::createTable(const std::string& table_name, const std::vector<std::string>& columns) {
    tables[table_name] = Table{columns, {}};
}

// Insert a record into a table
void Database::insertRecord(const std::string& table_name, const std::vector<std::string>& values) {
    tables[table_name].records.push_back(values);
}

// Query the contents of a table
std::vector<std::vector<std::string>> Database::queryTable(const std::string& table_name) {
    return tables[table_name].records;
}