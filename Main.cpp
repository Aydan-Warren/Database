#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
struct KeyValue {
    std::string key;
    std::string value;
};

class KeyValueStore {
private:
    std::vector<KeyValue> data;

public:
    void set(const std::string& key, const std::string& value) {
        for (auto& kv : data) {
            if (kv.key == key) {
                kv.value = value; // last write wins
                return;
            }
        }
        data.push_back({key, value});
    }

    bool get(const std::string& key, std::string& value) {
        for (auto& kv : data) {
            if (kv.key == key) {
                value = kv.value;
                return true;
            }
        }
        return false;
    }
};

std::string upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

/*
Replay the append-only log to rebuild the in-memory index
*/
void replayLog(KeyValueStore& store, const std::string& file)
{
    std::ifstream in(file);
    if (!in.is_open())
        return;

    std::string line;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        std::istringstream iss(line);

        std::string cmd, key, value;

        iss >> cmd >> key >> value;

        if (upper(cmd) == "SET")
            store.set(key, value);
    }
}

int main() {

    const std::string DB = "data.db";

    KeyValueStore store;

    // rebuild index from disk
    replayLog(store, DB);

    std::ofstream log(DB, std::ios::app);

    std::string cmd;

    while (true) {

        if (!(std::cin >> cmd))
            break;

        cmd = upper(cmd);

        if (cmd == "SET") {

            std::string key, value;

            if (!(std::cin >> key >> value))
                break;

            store.set(key, value);

            log << "SET " << key << " " << value << std::endl;
            log.flush();
        }

        else if (cmd == "GET") {

            std::string key, value;

            if (!(std::cin >> key))
                break;

            if (store.get(key, value))
                std::cout << value << std::endl;
            else
                std::cout << std::endl;

            std::cout.flush();
        }

        else if (cmd == "EXIT") {
            break;
        }
    }

    return 0;
}