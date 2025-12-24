#include <fstream>
#include <iostream>
#include <jsonc/jsonc.hpp>
#include <print>


int main() {
    try {
        auto        file = std::ifstream("./bin/test.jsonc");
        std::string content{std::istreambuf_iterator<char>(file), {}};
        auto        json = jsonc::parse(content);
        json["new"]      = 3.233;
        std::println("{}", json.dump());
        return 0;
    } catch (const std::exception& e) { std::println("{}", e.what()); } catch (...) {
        std::println("SEH Exception");
    }
}