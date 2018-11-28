#include "stdafx.h"
#include "chres.h"

#include <stdio.h>
#include <winuser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#pragma warning(disable : 4996)

void GetCurrentResolution(unsigned int& horizontal, unsigned int& vertical)
{
	DEVMODE dm;

	//Getting current display settings and write them  into the dm variable
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	horizontal = dm.dmPelsWidth;
	vertical = dm.dmPelsHeight;
}

//Splits a string using char as delimeter
std::vector<std::string> split(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}

//Reads a whole file and returns it as string
std::string slurp(std::ifstream& in) {
	return static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str();
}

//Checks if the specified file exists
inline bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}


//This is the main method, the method which will be called when the program is run
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	std::string tmpfile = getenv("temp");
	tmpfile.append("\\chres.tmp");
	std::string conf;
	std::string confFile = getenv("appdata");
	confFile.append("\\chres.conf");

	unsigned int width, heigth;
	GetCurrentResolution(width, heigth);
	conf += std::to_string(width) + "x" + std::to_string(heigth) + "\n1280x800";


	//if config does not exist create config, else read config
	if (!fileExists(confFile))
	{
		//create config
		std::ofstream confStream;
		confStream.open(confFile);
		confStream << conf;
		confStream.close();
	}
	else
	{
		//read config
		std::ifstream confStream;
		confStream.open(confFile);
		conf = slurp(confStream);
		confStream.close();
	}

	//Parse config
	std::vector<std::string> splitStr = split(conf, '\n');
	std::vector<std::string> values;
	if (fileExists(tmpfile))
	{
		values = split(*splitStr.begin(), 'x');

		remove(tmpfile.c_str());
	}
	else
	{
		values = split(*++splitStr.begin(), 'x');

		std::ofstream outfile;
		outfile.open(tmpfile);
		outfile << '\x01';
		outfile.close();
	}
	width = stoi(*values.begin());
	heigth = stoi(*++values.begin());

	DEVMODE dm, maxdm;
	maxdm.dmDisplayFrequency = 0;
	for (int i = 0; EnumDisplaySettings(NULL, i, &dm); i++)
	{
		//Get the display with the rigth resolution and the highest Frequency
		if (dm.dmPelsWidth == width && dm.dmPelsHeight == heigth && dm.dmDisplayFrequency > maxdm.dmDisplayFrequency)
			maxdm = dm;
		//std::cout << dm.dmPelsWidth << "x" << dm.dmPelsHeight << " @ " << dm.dmDisplayFrequency << "Hz, " << dm.dmBitsPerPel << "   " << dm.dmDisplayFlags << "\n";
	}
	ChangeDisplaySettings(&maxdm, 0);

	return 0;
}
