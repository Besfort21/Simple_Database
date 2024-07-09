#include "db.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <algorithm>

// Define a structure for a table
struct Table
{
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> records;

    // New index map
    std::unordered_map<std::string, std::map<std::string, int>> indexes;
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
    auto& table = tables[table_name];
    table.records.push_back(values);

    for (const auto& [column_name, index] : table.indexes) {
        auto col_it = std::find(table.columns.begin(), table.columns.end(), column_name);
        int col_index = std::distance(table.columns.begin(), col_it);
        table.indexes[column_name][values[col_index]] = table.records.size() - 1;
    }
}


// Query the contents of a table
std::vector<std::vector<std::string>> Database::queryTable(const std::string& table_name) {
    return tables[table_name].records;
}

std::vector<std::vector<std::string>> Database::queryTable(const std::string& table_name, const std::string& column_name, const std::string& value) {
    auto& table = tables[table_name];
    if (table.indexes.find(column_name) != table.indexes.end()) {
        auto it = table.indexes[column_name].find(value);
        if (it != table.indexes[column_name].end()) {
            return { table.records[it->second] };
        }
    }

    // If index is not found or value is not indexed, perform a full scan
    std::vector<std::vector<std::string>> results;
    for (const auto& record : table.records) {
        auto col_it = std::find(table.columns.begin(), table.columns.end(), column_name);
        int col_index = std::distance(table.columns.begin(), col_it);
        if (record[col_index] == value) {
            results.push_back(record);
        }
    }

    return results;
}


// Create Index
void Database::createIndex(const std::string& table_name, const std::string& column_name) {
    auto& table = tables[table_name];
    auto col_it = std::find(table.columns.begin(), table.columns.end(), column_name);
    if (col_it == table.columns.end()) {
        std::cerr << "Column " << column_name << " does not exist in table " << table_name << std::endl;
        return;
    }

    int col_index = std::distance(table.columns.begin(), col_it);
    std::map<std::string, int> index;

    for (size_t i = 0; i < table.records.size(); ++i) {
        index[table.records[i][col_index]] = i;
    }

    table.indexes[column_name] = index;
}
