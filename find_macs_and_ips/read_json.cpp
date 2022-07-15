#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
void test_read_json() {
  std::ifstream ifs { "ether_type_final.json" };
  if ( !ifs.is_open() )  {
    std::cerr << "Could not open file for reading!\n";
      return;
  }

  rapidjson::IStreamWrapper isw { ifs };

  rapidjson::Document doc {};
  doc.ParseStream( isw );

  std::cout << "done reading, find a value" << std::endl;
  
  std::cout << doc["2048"].GetString() << std::endl;

  const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

  for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
  {
    std::cout << "Type of member " << itr->name.GetString() << " is " << kTypeNames[itr->value.GetType()] << " with value " << itr->value.GetString() << std::endl;
  }
}
