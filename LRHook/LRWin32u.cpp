#include"LRWin32u.h"

LPVOID AllocateZeroedMemory(SIZE_T size) {
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

VOID InitEmptyLargeString(PLARGE_UNICODE_STRING String)
{
    ZeroMemory(String, sizeof(*String));
}

//VOID InitStringFromLargeString(PANSI_STRING AnsiString, PLARGE_UNICODE_STRING LargeString)
//{
//    AnsiString->Length = LargeString->Length;
//    AnsiString->MaximumLength = LargeString->MaximumLength;
//    AnsiString->Buffer = LargeString->AnsiBuffer;
//}

VOID InitStringFromLargeString(PUNICODE_STRING UnicodeString, PLARGE_UNICODE_STRING LargeString)
{
    UnicodeString->Length = LargeString->Length;
    UnicodeString->MaximumLength = LargeString->MaximumLength;
    UnicodeString->Buffer = LargeString->UnicodeBuffer;
}

NTSTATUS WINAPI RtlDuplicateUnicodeString(
    int add_nul,                  /* [I] flag */
    const UNICODE_STRING* source, /* [I] Unicode string to be duplicated */
    UNICODE_STRING* destination)  /* [O] destination for the duplicated unicode string */
{
    if (source == NULL || destination == NULL ||
        source->Length > source->MaximumLength ||
        (source->Length == 0 && source->MaximumLength > 0 && source->Buffer == NULL) ||
        add_nul == 2 || add_nul >= 4 || add_nul < 0) {
        return STATUS_INVALID_PARAMETER;
    }
    else {
        if (source->Length == 0 && add_nul != 3) {
            destination->Length = 0;
            destination->MaximumLength = 0;
            destination->Buffer = NULL;
        }
        else {
            unsigned int destination_max_len = source->Length;

            if (add_nul) {
                destination_max_len += sizeof(WCHAR);
            } /* if */
            destination->Buffer = (LPWSTR)AllocateZeroedMemory(destination_max_len);
            if (destination->Buffer == NULL) {
                return STATUS_NO_MEMORY;
            }
            else {
                memcpy(destination->Buffer, source->Buffer, source->Length);
                destination->Length = source->Length;
                destination->MaximumLength = source->Length;
                /* append terminating '\0' if enough space */
                if (add_nul) {
                    destination->MaximumLength = destination_max_len;
                    destination->Buffer[destination->Length / sizeof(WCHAR)] = 0;
                } /* if */
            } /* if */
        } /* if */
    } /* if */
    return STATUS_SUCCESS;
}

PLARGE_UNICODE_STRING LargeStringDuplicate(PLARGE_UNICODE_STRING LargeString, PLARGE_UNICODE_STRING Destination)
{
    union
    {
        ANSI_STRING     Ansi;
        UNICODE_STRING  Unicode;
    };

    union
    {
        ANSI_STRING     NewAnsi;
        UNICODE_STRING  NewUnicode;
    };

    if (LargeString->Ansi)
        return nullptr;

    InitStringFromLargeString(&Unicode, LargeString);
    if (RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &Unicode, &NewUnicode) != STATUS_SUCCESS)
        return nullptr;

    Destination->Ansi = FALSE;
    Destination->Length = NewUnicode.Length;
    Destination->MaximumLength = NewUnicode.MaximumLength;
    Destination->UnicodeBuffer = NewUnicode.Buffer;

    return Destination;
}

DWORD WINAPI RtlAnsiStringToUnicodeSize(PANSI_STRING str)
{
    DWORD ret = (str->Length + 1) << 1;
    return ret + sizeof(WCHAR);
}

NTSTATUS WINAPI RtlAnsiStringToUnicodeString(
    PUNICODE_STRING uni, /* [I/O] Destination for the unicode string */
    PANSI_STRING ansi,  /* [I]   Ansi string to be converted */
    BOOLEAN doalloc)     /* [I]   TRUE=Allocate new buffer for uni, FALSE=Use existing buffer */
{
    DWORD total = RtlAnsiStringToUnicodeSize(ansi);

    if (total > 0xffff) return STATUS_INVALID_PARAMETER_2;
    uni->Length = total - sizeof(WCHAR);
    if (doalloc)
    {
        uni->MaximumLength = total;
        if (!(uni->Buffer = (LPWSTR)AllocateZeroedMemory(total)))
            return STATUS_NO_MEMORY;
    }
    else if (total > uni->MaximumLength) return STATUS_BUFFER_OVERFLOW;

    MultiByteToWideChar(946, 0, ansi->Buffer, ansi->Length, uni->Buffer, uni->Length);
    uni->Buffer[uni->Length / sizeof(WCHAR)] = 0;
    return STATUS_SUCCESS;
}

PLARGE_UNICODE_STRING LargeStringAnsiToUnicode(PLARGE_UNICODE_STRING LargeAnsiString, PLARGE_UNICODE_STRING LargeUnicodeString)
{
    if (LargeAnsiString == nullptr)
        return nullptr;

    if (!LargeAnsiString->Ansi)
        return LargeStringDuplicate(LargeAnsiString, LargeUnicodeString);

    ANSI_STRING     AnsiString;
    UNICODE_STRING  UnicodeString;

    AnsiString.Buffer = LargeAnsiString->AnsiBuffer;
    AnsiString.Length = LargeAnsiString->Length;
    AnsiString.MaximumLength = LargeAnsiString->MaximumLength;

    if (RtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, TRUE) != STATUS_SUCCESS)
        return nullptr;

    LargeUnicodeString->Ansi = FALSE;
    LargeUnicodeString->Length = UnicodeString.Length;
    LargeUnicodeString->MaximumLength = UnicodeString.MaximumLength;
    LargeUnicodeString->Buffer = (ULONG64)UnicodeString.Buffer;

    return LargeUnicodeString;
}

PLARGE_UNICODE_STRING CaptureAnsiWindowName(PLARGE_UNICODE_STRING WindowName, PLARGE_UNICODE_STRING UnicodeWindowName)
{
    InitEmptyLargeString(UnicodeWindowName);

    if (WindowName == nullptr || WindowName->Buffer == 0)
        return nullptr;

    if (WindowName->UnicodeBuffer[0] == 0xFFFF)
    {
        WCHAR                   Buffer[0x10];
        ULONG_PTR               Length;
        LARGE_UNICODE_STRING    TitleAsResourceId;

        TitleAsResourceId.Ansi = FALSE;

        Length = WindowName->Ansi ? 3 : 4;

        CopyMemory(Buffer, WindowName->AnsiBuffer + WindowName->Ansi, Length);

        TitleAsResourceId.Length = Length;
        TitleAsResourceId.MaximumLength = Length;
        TitleAsResourceId.Buffer = (ULONG64)Buffer;

        return LargeStringDuplicate(&TitleAsResourceId, UnicodeWindowName);
    }

    return LargeStringAnsiToUnicode(WindowName, UnicodeWindowName);
}