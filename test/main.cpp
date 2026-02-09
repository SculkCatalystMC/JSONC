#include <fstream>
#include <iostream>
#include <jsonc/jsonc.hpp>
#include <print>

int main() {
    auto        file = std::ifstream("./bin/test.jsonc");
    std::string content{std::istreambuf_iterator<char>(file), {}};
#ifdef JSONC_USE_EXPECTED
    auto res = jsonc::ordered_json::parse(content, false, true);
    if (!res) {
        std::println("{}", res.error().mErrorInfo);
        return 1;
    }
    auto json              = *res;
    json["new"]->get()     = 3.233;
    json["new_map"]->get() = {
        {"5", 6},
        {"7", 8},
        {"1", 2},
        {"3", 4},
    };
    std::map<int, int> a{};
    json["new_vec"]->get()         = jsonc::ordered_json::array({1, 2, 3, 4, 5, 6, 7, 8});
    std::map<std::string, int> map = json["new_map"]->get();
    auto                       vec = *json["new_vec"]->get().get<std::vector<int>>();
    std::println("{}", json.dump());
    std::println("map {}", map);
    std::println("vec {}", vec);
    const auto& vec_value = json["new_vec"];
    for (int val : vec_value->get()) { std::println("{}", val); }
    for (auto& [k, v] : *json["new_map"]->get().items()) { std::println("{}: {}", k, (int)v); }
    return 0;
#else
    try {
#ifdef JSONC_NO_EXCEPTION
        auto res = jsonc::ordered_json::parse(content, false, true);
        if (!res) {
            std::println("parse failed");
            return 1;
        }
        auto json = *res;
#else
        auto json = jsonc::ordered_json::parse(content, false, true);
#endif
        json["new_map"] = {
            {"5", 6},
            {"7", 8},
            {"1", 2},
            {"3", 4},
        };
        json["new_vec"]                = jsonc::ordered_json::array({1, 2, 3, 4, 5, 6, 7, 8});
        std::map<std::string, int> map = json["new_map"];
        auto                       vec = json["new_vec"].get<std::vector<int>>();
        std::println("{}", json.dump());
        std::println("map {}", map);
        std::println("vec {}", vec);
        const auto& vec_value = json["new_vec"];
        for (int val : vec_value) { std::println("{}", val); }
        for (auto& [k, v] : json["new_map"].items()) { std::println("{}: {}", k, (int)v); }
        return 0;
    } catch (const std::exception& e) { std::println("{}", e.what()); }
#endif
}