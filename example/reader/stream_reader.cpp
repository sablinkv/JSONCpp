#include <json.h>
#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
    using namespace JSONCpp;
    
    std::string Path(argv[1]);
    std::ifstream File(Path);
    
    if (!File.is_open())
    {
        printf("File '%s' - not found.");
        return EXIT_FAILURE;
    }
    
    try
    {
        std::shared_ptr<JsonValue> Document;
        auto Reader = JsonReaderFactory::Create(File);
        Deserializer()(*Reader, Document);
        std::cout << *Document;
    }
    catch(const std::exception& error)
    {
        std::cerr << error.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}