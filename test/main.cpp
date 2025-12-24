#include <iostream>
#include <jsonc/jsonc.hpp>

#include <print>

std::string test = R"(    
/*test1
 *test2
* test3
test4
*
*/
// test5
{
    // test
    "xxx": "测试",
    "yyy": 26783921,
    "test": {
    "aa":"aa",
    "bb":[
    
    1,2,3,4,888]
    }
})";

int main() {
    try {
        auto json = jsonc::parse(test);
        // std::println("{}", json["xxx"].is_string());
        std::println("{}", json.dump());
        return 0;
    } catch (const std::exception& e) { std::println("{}", e.what()); } catch (...) {
        std::println("SEH Exception");
    }
}