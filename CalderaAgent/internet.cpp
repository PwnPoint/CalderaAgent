#include "internet.h"  // Include the header file where function declarations for this implementation are found.

#pragma comment (lib, "Wininet.lib")  // Link with the Wininet library, which provides functions for internet operations.

// Important Strings for the HTTP request
LPCSTR UserAgent = "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; InfoPath.1)";  // User agent string to mimic a browser.
LPCSTR acceptTypes[] = { "application/x-www-form-urlencoded", NULL };  // Specify accepted content types for the HTTP request.

// Function to send an HTTP request using Wininet APIs.
// Arguments:
// - Domain: The server domain (e.g., "www.example.com").
// - Port: The port number to connect to (e.g., 80 for HTTP).
// - URI: The specific resource to access (e.g., "/path/resource").
// - Verb: The HTTP method (e.g., "GET" or "POST").
// - Params: Data to send with the request (for POST).
HINTERNET SendRequest(LPCSTR Domain, int Port, LPCSTR URI, LPCSTR Verb, const char* Params)
{
    HINTERNET hSession, hConnect, hFile;  // Handles for session, connection, and file.

    // Open an internet session using the specified user agent.
    if ((hSession = InternetOpen(UserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) == NULL)
        return NULL;

    // Connect to the specified server using the domain and port.
    if ((hConnect = InternetConnect(hSession, Domain, Port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0)) == NULL)
        return NULL;

    // Open an HTTP request to the server using the provided HTTP verb (e.g., GET/POST) and URI.
    if ((hFile = HttpOpenRequest(hConnect, Verb, URI, NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD, 0)) == NULL)
        return NULL;

    // Send the HTTP request with the specified parameters (for POST requests).


    unsigned long dataLen = 0;
    if (Params != NULL)
        dataLen = strlen(Params); // Get the length of the parameters to send.

    if (HttpSendRequest(hFile, NULL, 0, (char*)Params, dataLen) == TRUE)
        return hFile;  // Return the request handle if successful.
    else
        return NULL;  // Return NULL if the request fails.
}

// Function to get the HTTP status code from the response.
// Arguments:
// - hFile: Handle to the HTTP request.
int GetStatusCode(HINTERNET hFile)
{
    int statusCode = 0;  // Variable to store the status code.
    DWORD nStatusCodeLength = sizeof(statusCode);  // Size of the status code variable.

    // Query the HTTP status code from the server's response.
    if (!HttpQueryInfo(hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &nStatusCodeLength, NULL))
        return 0;  // Return 0 if the query fails.

    return statusCode;  // Return the extracted status code.
}

// Function to retrieve the response body from the server.
// Arguments:
// - hFile: Handle to the HTTP request.
// - Length: Pointer to store the length of the response.
char* GetResponse(HINTERNET hFile, DWORD* Length)
{
    DWORD ContentLength = 0;  // Variable to store the content length of the response.
    DWORD varLength = sizeof(DWORD);  // Size of the content length variable.
    DWORD dwIndex = 0;
    *Length = 0;

    // Query the Content-Length of the response (how much data will be returned).
    if (!HttpQueryInfo(hFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &ContentLength, &varLength, &dwIndex))
        return NULL;  // Return NULL if the query fails.

    *Length = ContentLength;  // Store the content length.

    // Allocate a buffer to store the response content.
    char* Buffer = (char*)malloc(ContentLength + 1);
    memset(Buffer, 0, ContentLength + 1);  // Zero out the buffer to ensure it is null-terminated.

    // Read the response data from the server.
    if (InternetReadFile(hFile, Buffer, ContentLength, &dwIndex))
        return Buffer;  // Return the buffer if the read operation is successful.
    else
        return NULL;  // Return NULL if the read operation fails.
}

// Function to download a file from a specified URL.
// Arguments:
// - URL: The URL of the file to download.
// - FileLength: Pointer to store the size of the downloaded file.
// - FileToDownload: (Optional) Specific file information, used when downloading from Caldera.
char* DownloadFile(LPCSTR URL, DWORD* FileLength, LPCSTR FileToDownload = NULL)
{
    HINTERNET hSession, hURL;  // Handles for the internet session and the URL request.
    BOOL bResult;
    DWORD dwBytesRead = 1;

    // Open an internet session with the predefined user agent.
    hSession = InternetOpen(UserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    char* FileHeader = NULL;  // Buffer to store the custom file header.
    int FileHeaderLength = 0;

    // If a specific file is to be downloaded, create a custom header for it.
    if (FileToDownload)
    {
        // Format the file header with the specified file name.
        FileHeaderLength = snprintf(NULL, 0, "file:%s", FileToDownload);
        FileHeader = (char*)malloc(FileHeaderLength + 1);
        memset(FileHeader, 0, FileHeaderLength + 1);
        snprintf(FileHeader, FileHeaderLength + 1, "file:%s", FileToDownload);
    }

    // Open the URL for the download, optionally adding the file header.
    hURL = InternetOpenUrl(hSession, URL, FileHeader, FileHeaderLength, 0, 0);

    // Use the GetResponse function to retrieve the file data.
    DWORD Length = 0;
    char* FileData = GetResponse(hURL, &Length);
    *FileLength = Length;  // Store the length of the downloaded file.

    // Close the handles to clean up.
    InternetCloseHandle(hURL);
    InternetCloseHandle(hSession);

    // Return the file data.
    return FileData;
}
