#include <fstream>
#include <iostream>
#include <jsonc/jsonc.hpp>
#include <print>


int main() {
    try {
        auto        file = std::ifstream("./bin/test.jsonc");
        std::string content{std::istreambuf_iterator<char>(file), {}};
#ifdef JSONC_USE_EXPECTED
        auto json              = *jsonc::parse(content);
        json["new"]->get()     = 3.233;
        json["new_map"]->get() = {
            {"5", 6},
            {"7", 8},
            {"1", 2},
            {"3", 4},
        };
        std::map<std::string, int> map = json["new_map"]->get();
#else
        auto json       = jsonc::parse(content);
        json["new_map"] = {
            {"5", 6},
            {"7", 8},
            {"1", 2},
            {"3", 4},
        };
        json["new_vec"]                = jsonc::array({1, 2, 3, 4, 5, 6, 7, 8});
        std::map<std::string, int> map = json["new_map"];
        auto                       vec = json["new_vec"].get<std::vector<int>>();
#endif
        std::println("{}", json.dump());
        std::println("map {}", map);
        std::println("vec {}", vec);
        for (int val : json["new_vec"]) { std::println("{}", val); }
        return 0;
    } catch (const std::exception& e) { std::println("{}", e.what()); }
}