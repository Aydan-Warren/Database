#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// Struct for key-value pair
struct KeyValue {
    std::string key;
    std::string value;
};

// Simple in-memory index
class KeyValueStore {
public:
    void set(const std::string& key, const std::string& value) {
        for (auto& kv : index) {
            if (kv.key == key) {
                kv.value = value;
                return;
            }
        }
        index.push_back({key, value});
    }

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

// Convert string to uppercase
std::string to_upper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Replay log file to rebuild state
void replayLog(KeyValueStore& store, const std::string& filename) {
    std::ifstream file(filename);

    // If the file doesn't exist yet, just return
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cmd, key, value;

        iss >> cmd;
        cmd = to_upper(cmd);

        if (cmd == "SET") {
            iss >> key >> value;
            store.set(key, value);
        }
    }
}

int main() {
    KeyValueStore store;
    const std::string dbFile = "data.db";

    // Load previous state
    replayLog(store, dbFile);

    // Open database log file
    std::ofstream db(dbFile, std::ios::app);

    std::string input;

    while (std::getline(std::cin, input)) {
        std::istringstream iss(input);

        std::string cmd, key, value;
        iss >> cmd;

        cmd = to_upper(cmd);

        if (cmd == "SET") {
            iss >> key >> value;

            store.set(key, value);

            db << "SET " << key << " " << value << std::endl;
            db.flush();
        }

        else if (cmd == "GET") {
            iss >> key;

            if (store.get(key, value)) {
                std::cout << value << std::endl;
            }
            // IMPORTANT: print nothing if key doesn't exist
        }

        else if (cmd == "EXIT") {
            break;
        }
    }

    return 0;
}