#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// Struct for key-value pair
struct KeyValue {
    std::string key;
    std::string value;
};

// Simple in-memory index (array)
class KeyValueStore {
public:
    // Add or update key-value pair
    void set(const std::string& key, const std::string& value) {
        for (auto& kv : index) {
            if (kv.key == key) {
                kv.value = value;
                return;
            }
        }
        index.push_back({key, value});
    }

    // Get value for key, return true if found
    bool get(const std::string& key, std::string& value) {
        for (const auto& kv : index) {
            if (kv.key == key) {
                value = kv.value;
                return true;
            }
        }
        return false;
    }

private:
    std::vector<KeyValue> index;
};

void replayLog(KeyValueStore& store, const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cmd, key, value;
        iss >> cmd;
        if (cmd == "SET") {
            iss >> key;
            iss >> value;
            store.set(key, value);
        }
    }
}

int main() {
    KeyValueStore store;
    const std::string dbFile = "data.db";

    // Replay log to rebuild index
    replayLog(store, dbFile);

    // Open file for appending
    std::ofstream db(dbFile, std::ios::app);

    std::string input;
    while (std::getline(std::cin, input)) {
        std::istringstream iss(input);
        std::string cmd, key, value;
        iss >> cmd;
        if (cmd == "SET") {
            iss >> key;
            iss >> value;
            store.set(key, value);
            db << "SET " << key << " " << value << std::endl;
            db.flush();
        } else if (cmd == "GET") {
            iss >> key;
            if (store.get(key, value)) {
                std::cout << value << std::endl;
            } else {
                std::cout << "NULL" << std::endl;
            }
        } else if (cmd == "EXIT") {
            break;
        }
    }
    return 0;
}