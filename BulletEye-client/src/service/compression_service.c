#include <service/compression_service.h>
#include <stdio.h>
#include <stdlib.h>

// --- COMPRESSION FUNCTION ---
BOOL CompressData(const BYTE* in_buffer, DWORD in_size, BYTE** out_buffer, DWORD* out_size) {
    if (!in_buffer || in_size == 0 || !out_buffer || !out_size) return FALSE;

    BOOL success = FALSE;
    COMPRESSOR_HANDLE compressor = NULL;

    // Use SIZE_T for API calls as required by the header
    SIZE_T compressed_data_size_t = 0;
    
    if (!CreateCompressor(
            COMPRESS_ALGORITHM_XPRESS_HUFF,
            NULL,
            &compressor)) {
        wprintf(L"Error creating compressor: %lu\n", GetLastError());
        return FALSE;
    }

    if (!Compress(
            compressor,
            (PVOID)in_buffer,         // Cast to PVOID to satisfy the warning
            in_size,
            NULL,
            0,
            &compressed_data_size_t)) { // Use the 64-bit SIZE_T variable
        
        DWORD last_error = GetLastError();
        if (last_error != ERROR_INSUFFICIENT_BUFFER) {
            wprintf(L"Error querying compressed buffer size: %lu\n", last_error);
            goto done;
        }
    }

    *out_buffer = (BYTE*)malloc(compressed_data_size_t);
    if (!*out_buffer) {
        wprintf(L"Error allocating memory for compressed buffer.\n");
        goto done;
    }

    if (!Compress(
            compressor,
            (PVOID)in_buffer,        // Cast to PVOID
            in_size,
            *out_buffer,
            compressed_data_size_t,
            &compressed_data_size_t)) { // Use the 64-bit SIZE_T variable
        
        wprintf(L"Error compressing data: %lu\n", GetLastError());
        free(*out_buffer);
        *out_buffer = NULL;
        goto done;
    }

    // Safely convert back to DWORD for the function's output
    *out_size = (DWORD)compressed_data_size_t;
    success = TRUE;

done:
    if (compressor) {
        CloseCompressor(compressor);
    }
    return success;
}

// --- DECOMPRESSION FUNCTION ---
BOOL DecompressData(const BYTE* in_buffer, DWORD in_size, BYTE** out_buffer, DWORD* out_size) {
    if (!in_buffer || in_size == 0 || !out_buffer || !out_size) return FALSE;

    BOOL success = FALSE;
    DECOMPRESSOR_HANDLE decompressor = NULL;

    // Use SIZE_T for API calls
    SIZE_T decompressed_data_size_t = 0;

    if (!CreateDecompressor(
            COMPRESS_ALGORITHM_XPRESS_HUFF,
            NULL,
            &decompressor)) {
        wprintf(L"Error creating decompressor: %lu\n", GetLastError());
        return FALSE;
    }

    if (!Decompress(
            decompressor,
            (PVOID)in_buffer,       // Cast to PVOID
            in_size,
            NULL,
            0,
            &decompressed_data_size_t)) { // Use the 64-bit SIZE_T variable
        
        DWORD last_error = GetLastError();
        if (last_error != ERROR_INSUFFICIENT_BUFFER) {
            wprintf(L"Error querying decompressed buffer size: %lu\n", last_error);
            goto done;
        }
    }

    *out_buffer = (BYTE*)malloc(decompressed_data_size_t);
    if (!*out_buffer) {
        wprintf(L"Error allocating memory for decompressed buffer.\n");
        goto done;
    }

    if (!Decompress(
            decompressor,
            (PVOID)in_buffer,       // Cast to PVOID
            in_size,
            *out_buffer,
            decompressed_data_size_t,
            &decompressed_data_size_t)) { // Use the 64-bit SIZE_T variable
        
        wprintf(L"Error decompressing data: %lu\n", GetLastError());
        free(*out_buffer);
        *out_buffer = NULL;
        goto done;
    }
    
    // Safely convert back to DWORD
    *out_size = (DWORD)decompressed_data_size_t;
    success = TRUE;

done:
    if (decompressor) {
        CloseDecompressor(decompressor);
    }
    return success;
}