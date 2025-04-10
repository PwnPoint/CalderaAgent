#include "execute.h"  // Include the header file where function declarations for this implementation are found.

std::wstring ConvertAnsiToWide(const std::string& str)
{
    // Convert ANSI (multi-byte) string to a wide string (Unicode).
    // First, we calculate the number of wide characters needed to hold the ANSI string.
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0);

    // Allocate a wide string (std::wstring) of the necessary size.
    std::wstring wstr(count, 0);

    // Perform the actual conversion of the ANSI string to a wide string.
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &wstr[0], count);

    // Return the converted wide string.
    return wstr;
}

// Function to remove any characters from a string that are not part of the first 127 ASCII characters.
string RemoveNonLetters(string& input)
{
    // Iterate through each character in the input string.
    for (int i = 0; i < input.length(); i++)
    {
        // If the character's ASCII value is greater than 127 (non-ASCII), it is removed.
        if ((unsigned char)input.c_str()[i] > 127)
        {
            // Print the non-ASCII character's value for debugging.
            printf("%d\n", (unsigned char)input.c_str()[i]);

            // Remove the non-ASCII character from the string.
            input.erase(i, 1);
        }
    }

    // Return the modified string with only ASCII characters.
    return input;
}

// Function to copy a string, ensuring it is null-terminated.
char* CopyStr(char* src, int length)
{
    // Allocate memory for the destination string, including space for the null terminator.
    char* dest = (char*)malloc(length + 1);

    // If memory allocation fails, return NULL.
    if (dest == NULL)
        return NULL;

    // Initialize the allocated memory to zero to ensure the string is properly null-terminated.
    memset(dest, 0, length + 1);

    // Copy the source string to the destination string.
    memcpy(dest, src, length);

    // Return the copied string.
    return dest;
}

char* ExtractString(string src, char* start, char* end, int& pos)
{
    char* result = NULL;
    int size = 0;
    string source = src;
    pos = source.find(start, pos);
    if (pos == -1)
        return result;
    pos = pos + strlen(start);
    int endpos = source.find(end, pos);

    if (endpos == -1)
        size = source.length() - pos;
    else    
        size = endpos - pos;    

    result = (char*)malloc(size + 1);
    memset(result, 0, size + 1);
    source.copy(result, size, pos);
    pos = endpos;
    return result;
}

// Define the maximum wait time for the application to execute (30 seconds).
#define MAX_WAITTIME 100 * 30 

// Function to execute a command-line application and capture its output.
char* ExecuteApplication(string cmd, size_t& OutputLength)
{
    char buf[1024 + 1] = {};  // Buffer to store output from the application.
    DWORD bytesRead = 0;  // Variable to store the number of bytes read from the application's output.
    char* output = NULL;  // Pointer to store the final output string.
    std::string result = "";  // String to accumulate the output.

    BOOL ok = TRUE;  // Boolean variable to track success/failure of API calls.
    HANDLE hStdInPipeRead = NULL;  // Handle for reading from the stdin pipe.
    HANDLE hStdInPipeWrite = NULL;  // Handle for writing to the stdin pipe.
    HANDLE hStdOutPipeRead = NULL;  // Handle for reading from the stdout pipe.
    HANDLE hStdOutPipeWrite = NULL;  // Handle for writing to the stdout pipe.

    // Create two pipes for inter-process communication (stdin and stdout).
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };  // Set the security attributes for pipe handles.
    ok = CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0);  // Create stdin pipe.
    ok = CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0);  // Create stdout pipe.

    // Set up the startup information for creating the process.
    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);  // Set the size of the STARTUPINFO structure.
    si.dwFlags = STARTF_USESTDHANDLES;  // Specify that the process should use custom handles for stdin, stdout, and stderr.
    si.hStdError = hStdOutPipeWrite;  // Redirect stderr to the stdout pipe.
    si.hStdOutput = hStdOutPipeWrite;  // Redirect stdout to the stdout pipe.
    si.hStdInput = hStdInPipeRead;  // Redirect stdin from the stdin pipe.

    // Create a PROCESS_INFORMATION structure to hold information about the created process.
    PROCESS_INFORMATION pi = {};
    LPSTR lpCommandLine = (LPSTR)cmd.c_str();  // Convert the command string to a LPSTR for CreateProcess.

    // Create the process for the specified command.
    ok = CreateProcess(NULL, lpCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    // If process creation failed, return NULL.
    if (ok == FALSE) return NULL;

    // Wait for the process to finish execution, but do not exceed the maximum wait time.
    WaitForSingleObject(pi.hProcess, MAX_WAITTIME);

    // Ensure the process is terminated after the wait.
    TerminateProcess(pi.hProcess, 0);

    // Close the pipes that are no longer needed.
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);

    // Read the output from the application's stdout.
    DWORD dwRead = 0;
    DWORD dwAvail = 0;

    // Continuously read from the stdout pipe until there is no more data.
    ok = ReadFile(hStdOutPipeRead, buf, 1024, &dwRead, NULL);
    while (ok == TRUE)
    {
        buf[dwRead] = '\0';  // Null-terminate the buffer to treat it as a string.
        result += buf;  // Append the buffer content to the result string.
        ok = ReadFile(hStdOutPipeRead, buf, 1024, &dwRead, NULL);  // Read the next chunk of data.
    }

    // Close the remaining pipe handles.
    CloseHandle(hStdOutPipeRead);
    CloseHandle(hStdInPipeWrite);

    // Remove any non-ASCII characters from the result string.
    RemoveNonLetters(result);

    // Set the output length to the length of the result string.
    OutputLength = result.length();

    // If the result is empty, return NULL.
    if (result.length() == 0)
        return NULL;

    // Copy the result string into a new allocated memory block and return it.
    output = CopyStr((char*)result.c_str(), result.length());
    return output;
}
