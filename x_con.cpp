#include "xentax.h"
#include "x_con.h"

void test_map_deque(void)
{
 // Test #1
 // Basic Usage
 typedef map_deque<uint32, std::string> map_type;
 map_deque<uint32, std::string> m1;
 m1.insert(map_type::value_type(15, "15 - 1"));
 m1.insert(map_type::value_type(15, "15 - 2"));
 m1.insert(map_type::value_type(15, "15 - 3"));
 m1.insert(map_type::value_type(16, "16 - 1"));
 m1.insert(map_type::value_type(16, "16 - 2"));
 m1.insert(map_type::value_type(16, "16 - 3"));
 for(auto miter = m1.begin(); miter != m1.end(); miter++)
     for(auto diter = miter->second.begin(); diter != miter->second.end(); diter++)
         std::cout << *diter << std::endl;
}

void test_frequency_map(void)
{
 // Test #1
 // Basic Usage
 typedef frequency_map<std::string> map_type;
 map_type m1;
 m1.increment("how");
 m1.increment("now");
 m1.increment("brown");
 m1.increment("cow");
 m1.increment("how");
 m1.increment("now");
 m1.increment("?");
 for(auto iter = m1.begin(); iter != m1.end(); iter++)
     std::cout << iter->first << ": " << iter->second << " occurrences." << std::endl;
}