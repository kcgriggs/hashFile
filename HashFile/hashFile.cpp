/******************************************************************************
Program Name: dataFile.cpp
Des: This program takes a hash file (pinsAndKeys) and a list of pins (pins.txt)
and locates the pins within the hash file. It then displays a report of the 
pins, keys, and the number of accesses taken to find the pin, in a report file
(hashReport.cpp). It also calculates and reports the average number of accesses
to find a pin within the hash file.
	Author: Casey Griggs
	Due Date: 12/05/19
*******************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

string convertToString(char*, int);

const short PINSIZE = 3;
const short KEYSIZE = 5;

struct HashRecord
{
	char Pin[PINSIZE];
	char Key[KEYSIZE];
	int Top;
};

void main()
{
	ifstream pinsFin("pins.txt", ios::in);
	ifstream hashFin("pinsAndKeys.dat", ios::in | ios::binary);

	if (pinsFin.is_open() & hashFin.is_open())
	{
		int hashSize;
		int numHashSlots;
		int nextRRN;

		bool isFound = false;
		int numAccesses = 0;
		int sumAccesses = 0;
		int numPins = 0;

		hashFin.read((char*)& hashSize, sizeof hashSize);
		hashFin.read((char*)& numHashSlots, sizeof numHashSlots);
		hashFin.read((char*)& nextRRN, sizeof nextRRN);

		string pin;
		int RRN;
		int headSize = sizeof numHashSlots * 3;
		int overflowBase = (numHashSlots * hashSize) + headSize;

		ofstream reportFout("hashReport.txt", ios::out);

		// Read in pins from pins.txt
		if (reportFout.is_open())
		{
			reportFout << left << setw(6) << "PIN" << setw(8) << "KEY" << setw(14) << "# OF ACCESSES" << endl << endl;
			while (!pinsFin.eof())
			{
				HashRecord currentRecord;
				getline(pinsFin, pin);
				numPins++;
				isFound = false;
				RRN = (pin[0] * pin[1] * pin[2]) % numHashSlots;
			
				numAccesses = 0;
				string pinString;
				string keyString;

				int readPosition = headSize + (hashSize * RRN);

				// Search for pin in hash file
				while (!isFound)
				{
					hashFin.seekg(readPosition);
					hashFin.read((char*)&currentRecord, sizeof currentRecord);
					pinString = convertToString(currentRecord.Pin, sizeof currentRecord.Pin);

					// Pin found
					if (pinString == pin)
					{
						numAccesses++;
						isFound = true;
						keyString = convertToString(currentRecord.Key, sizeof currentRecord.Key);
					}
					// Pin not found
					else if (currentRecord.Pin[0] == '*' || currentRecord.Top == -1)
					{
						numAccesses++;
						isFound = true;
						keyString = "*****";
					}
					// Pin not located at current location, move to next overflow location
					else
					{
						numAccesses++;
						readPosition = overflowBase + (hashSize * currentRecord.Top);
					}
				}
				// Output result to hashReport.txt
				reportFout << left << setw(6) << pin << setw(8) << keyString << right << setw(7) << numAccesses << endl;
				sumAccesses += numAccesses;
			}
			// Calculate and output average accesses
			float avgAccess = (float)sumAccesses / (float)numPins;
			reportFout << "Average access to locate a pin or determine pin does not exist: " << setprecision(2) << avgAccess << endl;

			pinsFin.close();
			hashFin.close();
			reportFout.close();
		}
		else
		{
			cout << "Unable to open output file." << endl;
			exit(0);
		}
		
	}
	else
	{
		cout << "Unable to open input file." << endl;
		exit(0);
	}
}

/******************************************************************************
Name: convertToString
Des: This function accepts a character array parameter and an integer parameter
which holds the length of the array. It then converts the character array into
a std::string.
******************************************************************************/
string convertToString(char* arrayToConvert, int size)
{
	string result = "";
	for (int i = 0; i < size; i++) 
	{
		result = result + arrayToConvert[i];
	}
	return result;
}

