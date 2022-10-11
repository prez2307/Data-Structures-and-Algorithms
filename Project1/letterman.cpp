#include "letterman.h"
#include <getopt.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);
	Letterman letter;
	letter.getOptions(argc, argv);
	letter.output();

	return 0; 
}
