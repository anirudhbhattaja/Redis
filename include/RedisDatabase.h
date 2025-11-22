#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include <string>
#include<vector>
#include <mutex>
#include<fstream>
#include<unordered_map>
#include <sstream>

class RedisDatabase{
    public:
        //Get the singleton instance
        static RedisDatabase & getInstance();

        //Persistance : Dump / load the database from and to memeory using file
        bool dump(const std::string& filename);
        bool load(const std::string& filename);

    private:
        RedisDatabase() = default;
        ~RedisDatabase() = default;
        RedisDatabase(const RedisDatabase&) = delete;
        RedisDatabase& operator = (const RedisDatabase&) = delete;

        std::mutex db_mutex;
        //simple key value pairs
        std::unordered_map<std::string,std::string> kv_store;
        //sotres list, key - array pairs
        std::unordered_map<std::string,std::vector<std::string>> list_store;
        //stores hash map itself, key - hashmap pairs
        std::unordered_map<std::string,std::unordered_map<std::string,std::string>> hash_store;
        


};

#endif