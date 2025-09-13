#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef COMPRESSION_SERVICE_H
#define COMPRESSION_SERVICE_H

#include <windows.h>

#ifndef __TINYC__
    #include <compressapi.h>
#else
    // --- TinyCC fallback ---
    #define COMPRESS_ALGORITHM_MSZIP        2
    #define COMPRESS_ALGORITHM_XPRESS       3
    #define COMPRESS_ALGORITHM_XPRESS_HUFF  4
    #define COMPRESS_ALGORITHM_LZMS         5

    typedef PVOID COMPRESSOR_HANDLE;
    typedef PVOID DECOMPRESSOR_HANDLE;

    // Use void* instead of PCOMPRESS_ALLOCATION_ROUTINES
    BOOL WINAPI CreateCompressor(
        DWORD Algorithm,
        void* AllocationRoutines,
        COMPRESSOR_HANDLE *CompressorHandle
    );

    BOOL WINAPI Compress(
        COMPRESSOR_HANDLE CompressorHandle,
        LPCVOID UncompressedData,
        SIZE_T UncompressedDataSize,
        PVOID CompressedBuffer,
        SIZE_T CompressedBufferSize,
        SIZE_T *CompressedDataSize
    );

    BOOL WINAPI CloseCompressor(
        COMPRESSOR_HANDLE CompressorHandle
    );

    BOOL WINAPI CreateDecompressor(
        DWORD Algorithm,
        void* AllocationRoutines,
        DECOMPRESSOR_HANDLE *DecompressorHandle
    );

    BOOL WINAPI Decompress(
        DECOMPRESSOR_HANDLE DecompressorHandle,
        LPCVOID CompressedData,
        SIZE_T CompressedDataSize,
        PVOID UncompressedBuffer,
        SIZE_T UncompressedBufferSize,
        SIZE_T *UncompressedDataSize
    );

    BOOL WINAPI CloseDecompressor(
        DECOMPRESSOR_HANDLE DecompressorHandle
    );
#endif

// --- Service API ---
BOOL CompressData(const BYTE* in_buffer, DWORD in_size, BYTE** out_buffer, DWORD* out_size);
BOOL DecompressData(const BYTE* in_buffer, DWORD in_size, BYTE** out_buffer, DWORD* out_size);

#endif // COMPRESSION_SERVICE_H
