#include <stdio.h> 
#include <Windows.h>
#include <string>
using namespace std;


//Executing Applications
wstring ConvertAnsiToWide(const string& str);
char* ExecuteApplication(string cmd, size_t& OutputLength);
char* ExtractString(string src, char* start, char* end, int& pos);