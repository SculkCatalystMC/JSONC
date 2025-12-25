#include <fstream>
#include <iostream>
#include <jsonc/jsonc.hpp>
#include <print>


int main() {
    try {
        auto        file = std::ifstream("./bin/test.jsonc");
        std::string content{std::istreambuf_iterator<char>(file), {}};
#ifdef JSONC_USE_EXPECTED
        auto json          = *jsonc::parse(content);
        json["new"]->get() = 3.233;
#else
        auto json   = jsonc::parse(content);
        json["new"] = 3.233;
#endif
        std::println("{}", json.dump());
        return 0;
    } catch (const std::exception& e) { std::println("{}", e.what()); }
}