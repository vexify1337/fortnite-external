#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>

uintptr_t virtualaddy;
uintptr_t cr3;

#define CODE_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4756, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4759, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_BA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3626, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_GET_DIR_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1348, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_SECURITY 0x357c1d6

typedef struct _RD {
    INT32 security;
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
} RD, * RDW;

typedef struct _SH {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} SH, * SHA;

typedef struct _FA {
    INT32 security;
    ULONGLONG* address;
} FA, * FGA;


typedef struct _MEMORY_OPERATION_DATA {
    uint32_t pid;
    ULONGLONG* cr3;
} MEMORY_OPERATION_DATA, * PMEMORY_OPERATION_DATA;



namespace PagefaultAPI {
    HANDLE driver_handle;
    INT32 process_id;

    bool AttachDriver() {
        driver_handle = CreateFileW((L"\\\\.\\{7281b137-1933-43c8-bcab-c7b3f19ba844}"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
            return false;

        return true;
    }

    void read_physical(PVOID address, PVOID buffer, DWORD size) {
        _RD arguments = { 0 };

        arguments.security = CODE_SECURITY;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;


        DeviceIoControl(driver_handle, CODE_READ_MEMORY, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    void write_physical(PVOID address, PVOID buffer, DWORD size) {
        _RD arguments = { 0 };

        arguments.security = CODE_SECURITY;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;

        DeviceIoControl(driver_handle, CODE_WRITE_MEMORY, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }


    uintptr_t FetchDtb() {
        uintptr_t cr3 = NULL;
        _MEMORY_OPERATION_DATA arguments = { 0 };

        arguments.pid = process_id;
        arguments.cr3 = (ULONGLONG*)&cr3;

        DeviceIoControl(driver_handle, CODE_GET_DIR_BASE, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return cr3;
    }

    uintptr_t Fetch_ProcessBaseAddress() {
        uintptr_t image_address = { NULL };
        _SH arguments = { NULL };

        arguments.security = CODE_SECURITY;
        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;

        DeviceIoControl(driver_handle, CODE_BA, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return image_address;
    }

    INT32 find_process(LPCTSTR process_name) {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);

        return { NULL };
    }
}

bool is_valid(const uint64_t address)
{
    if (address == 0 || address == 0xCCCCCCCCCCCCCCCC || address == 0xFFFFFFFFFFFFFFFF)
        return false;

    if (address <= 0x400000 || address > 0x7FFFFFFFFFFFFFFF)
        return false;

    return true;
}

template <typename T>
T ReadMemory(uint64_t address) {
    T buffer{ };
    if (is_valid(address))
        PagefaultAPI::read_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}

template <typename T>
void write(uint64_t address, T buffer) {
    if (is_valid(address))
        PagefaultAPI::write_physical((PVOID)address, &buffer, sizeof(T));
}

template <typename T>
std::vector<T> batch_read(const std::vector<uint64_t>& addresses) {
    size_t num_addresses = addresses.size();
    std::vector<T> results(num_addresses);
    std::vector<uint8_t> buffer(num_addresses * sizeof(T));

    for (size_t i = 0; i < num_addresses; ++i) {
        if (!is_valid(addresses[i])) {
            continue;
        }
        PagefaultAPI::read_physical(reinterpret_cast<PVOID>(addresses[i]), buffer.data() + i * sizeof(T), sizeof(T));
    }
    for (size_t i = 0; i < num_addresses; ++i) {
        results[i] = *reinterpret_cast<T*>(buffer.data() + i * sizeof(T));
    }
    return results;
}

inline bool read1(const std::uintptr_t address, void* buffer, const std::size_t size)
{
    if (buffer == nullptr || size == 0) {
        return false;
    }
    PagefaultAPI::read_physical(reinterpret_cast<PVOID>(address), buffer, static_cast<DWORD>(size));
}

struct BatchReadEntry {
    uint64_t address;
    PVOID buffer;
    size_t size;
};

std::vector<BatchReadEntry> readBatchQueue;

static void AddToBatchRead(uint64_t Address, PVOID Buffer, size_t Size) {
    BatchReadEntry entry;
    entry.address = Address;
    entry.buffer = Buffer;
    entry.size = Size;

    readBatchQueue.push_back(entry);
}

static void ReadBatch() {
    for (const auto& entry : readBatchQueue) {
        if (entry.buffer && entry.size > 0) {
            PagefaultAPI::read_physical((PVOID)entry.address, entry.buffer, static_cast<DWORD>(entry.size));
        }
    }

    readBatchQueue.clear();
}