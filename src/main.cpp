#include "db.h"
#include <iostream>
#include <vector>

int main() {
    // Create a Database object
    Database db("test.db");

    // Create a table
    db.createTable("users", {"id", "name", "email"});

    // Insert records
    db.insertRecord("users", {"1", "Alice", "alice@example.com"});
    db.insertRecord("users", {"2", "Bob", "bob@example.com"});

    // Query the table
    std::vector<std::vector<std::string>> results = db.queryTable("users");

    // Display the results
    for (const auto& row : results) {
        for (const auto& column : row) {
            std::cout << column << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}