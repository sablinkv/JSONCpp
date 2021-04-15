#include <json.h>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    using namespace JSONCpp;

    try
    {
        std::string Students = R"(
            {
                "students": [
                    {
                        "id": 1,
                        "first_name": "Jamie",
                        "last_name": "Smith",
                        "email": "jsmith@email.com"
                    },
                    {
                        "id": 2,
                        "first_name": "Laura",
                        "last_name": "Grey",
                        "email": "lgrey@email.com"
                    }
                ]
            }
        )";

        std::shared_ptr<JsonValue> Document;
        auto Reader = JsonReaderFactory::Create(std::move(Students));
        Deserializer()(*Reader, Document);
        std::cout << *Document;
    }
    catch(const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}