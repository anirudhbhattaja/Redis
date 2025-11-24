#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include <string>
#include<vector>
#include <mutex>
#include<fstream>
#include<unordered_map>
#include <sstream>
#include <chrono>

class RedisDatabase{
    public:
        //Get the singleton instance
        static RedisDatabase & getInstance();

        //Common Commands
        bool flushAll();

        //key value ops
        void set(const std::string& key, const std::string& value);
        bool get(const std::string&key, std::string& value);
        std::vector<std::string> keys();
        std::string type(const std::string& key);
        bool del(const std::string& key);
        bool expire(const std::string& key,const int& seconds);
        bool rename(const std::string& oldKey,const std::string& newKey);


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
        
        std::unordered_map<std::string,std::chrono::steady_clock::time_point> expiry_map;


};

#endif