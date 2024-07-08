#ifndef DB_H
#define DB_H

#include <vector>
#include <string>

class Database {

    public:
        //Constructor that takes a filename
        Database(const std::string& filename);

        //Destructor
        ~Database();

        //Methods to interact with the database
        void createTable(const std::string& table_name,const std::vector<std::string>& columns);
        void insertRecord(const std::string& table_name, const std::vector<std::string>& values);
        std::vector<std::vector<std::string>> queryTable(const std::string& table_name);

    private:

        //Private members
        std::string filename;
        void load();
        void save();
};

#endif //DB_H