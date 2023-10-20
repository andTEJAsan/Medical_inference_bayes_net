#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string input = "The price is $19.99";
    double price;
    std::stringstream ss(input);
    std::string text;
    ss >> text >> text >> text >> text;
	std::cout << "Text : "<< text << std::endl;
    std::cout << "Price: " << price << std::endl;

    return 0;
}