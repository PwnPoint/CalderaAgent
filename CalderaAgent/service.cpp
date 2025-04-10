#include "service.h" // Including the header file where the service-related declarations are made.

// Function to install the service. Takes the service name as a parameter.
VOID InstallService(LPSTR SVCNAME)
{
    SC_HANDLE schSCManager;  // Handle to the Service Control Manager (SCM).
    SC_HANDLE schService;    // Handle to the service once it's created.
    TCHAR szPath[MAX_PATH];  // Array to hold the full path to the service's executable.

    // Retrieve the full path of the executable that is currently running.
    if (!GetModuleFileName(NULL, szPath, MAX_PATH))
    {
        // If it fails to get the path, print an error message and exit.
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the Service Control Manager (SCM) database.
    schSCManager = OpenSCManager(
        NULL,                    // Connect to the local computer.
        NULL,                    // Use the ServicesActive database (default database for services).
        SC_MANAGER_ALL_ACCESS);  // Request full access rights to manage services.

    // If opening the SCM database fails, print an error message and exit.
    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service within the SCM database.
    schService = CreateService(
        schSCManager,              // Handle to the SCM database.
        SVCNAME,                   // Name of the service.
        SVCNAME,                   // Display name of the service (usually the same as the service name).
        SERVICE_ALL_ACCESS,        // Full access to control the service (start, stop, etc.).
        SERVICE_WIN32_OWN_PROCESS, // The service will run in its own process (not shared with other services).
        SERVICE_DEMAND_START,      // The service will start manually (not automatically on boot).
        SERVICE_ERROR_NORMAL,      // Normal error control; logs error but continues running other services if this one fails.
        szPath,                    // Path to the service's binary executable (retrieved earlier).
        NULL,                      // No specific load ordering group.
        NULL,                      // No tag identifier (used for driver services).
        NULL,                      // No service dependencies (other services this one relies on).
        NULL,                      // Use the LocalSystem account (no specific user account required).
        NULL);                     // No password for the LocalSystem account.

    // If service creation fails, print an error message and clean up the SCM handle.
    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager); // Close the SCM handle before exiting.
        return;
    }
    else
    {
        // If the service is successfully created, notify the user.
        printf("Service installed successfully\n");
    }

    // Close the handles to both the service and the SCM to free up resources.
    CloseServiceHandle(schService);     // Close the service handle.
    CloseServiceHandle(schSCManager);   // Close the SCM handle.
}
