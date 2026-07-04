#include <iostream>
#include "../include/core/server.h" // Path to your server header

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    const char* port = argv[1];
    
    try {
        Server server(port);
    } catch (const exception& e) {
        cerr << "Fatal Server Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}