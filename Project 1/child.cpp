#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    // Error checking to ensure that the correct number of arguments are provided

    if (argc != 3)
    {
        cerr << "Please follow this format: " << argv[0] << " child_number name_of_child\n";
        return 1;
    }

    // Printing the statment
    cout << "I am child number " << argv[1] << ", and my name is " << argv[2] << ".\n";

    return 0;
}
