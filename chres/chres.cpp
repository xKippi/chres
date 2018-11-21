#include "stdafx.h"
#include "chres.h"

#include <stdio.h> 
#include <winuser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#pragma warning(disable : 4996)


void GetDesktopResolution(unsigned int& horizontal, unsigned int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
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
	GetDesktopResolution(width, heigth);
	conf += std::to_string(width) + " " + std::to_string(heigth) + "\n1280 800";

	
	if (!fileExists(confFile))
	{
		std::ofstream confStream;
		confStream.open(confFile);
		confStream << conf;
		confStream.close();
	}
	else
	{
		std::ifstream confStream;
		confStream.open(confFile);
		conf = slurp(confStream);
		confStream.close();
	}

	std::vector<std::string> splitStr = split(conf, '\n'); 
	std::vector<std::string> values;
	if (fileExists(tmpfile))
	{
		values = split(*splitStr.begin(), ' ');

		remove(tmpfile.c_str());
	}
	else
	{
		values = split(*++splitStr.begin(), ' ');

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




/*
std::wstring getWString(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
    std::string fileName = getenv("temp");
	fileName.append("\\test.txt");

	HANDLE hFile = CreateFile(getWString(fileName).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		1,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_TEMPORARY,
		NULL);
*/