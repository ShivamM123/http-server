#include <iostream>
#include "server.h" // <--- ADD THIS

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    char* port = argv[1];

    cout << "Starting server on port: " << port << "\n";
    
    Server server(port); 

    return 0;
}