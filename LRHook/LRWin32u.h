#include<Windows.h>

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)    // ntsubauth
#define STATUS_INVALID_PARAMETER_2       ((NTSTATUS)0xC00000F0L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
#define RTL_DUPSTR_ADD_NULL                                     (0x00000001)

#define LOOP_ONCE   for (BOOL __condition_ = TRUE; __condition_; __condition_ = FALSE)

typedef struct _LARGE_UNICODE_STRING
{
	ULONG Length;
	ULONG MaximumLength : 31;
	ULONG Ansi : 1;

	union
	{
		PWSTR   UnicodeBuffer;
		PSTR    AnsiBuffer;
		ULONG64 Buffer;
	};

} LARGE_UNICODE_STRING, * PLARGE_UNICODE_STRING;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} ANSI_STRING, * PANSI_STRING;

LPVOID AllocateZeroedMemory(SIZE_T size);
VOID InitEmptyLargeString(PLARGE_UNICODE_STRING String);
VOID InitStringFromLargeString(PUNICODE_STRING UnicodeString, PLARGE_UNICODE_STRING LargeString);
NTSTATUS WINAPI RtlDuplicateUnicodeString(
	int add_nul,
	const UNICODE_STRING* source,
	UNICODE_STRING* destination);
PLARGE_UNICODE_STRING LargeStringDuplicate(PLARGE_UNICODE_STRING LargeString, PLARGE_UNICODE_STRING Destination);
DWORD WINAPI RtlAnsiStringToUnicodeSize(PANSI_STRING str);
NTSTATUS WINAPI RtlAnsiStringToUnicodeString(
	PUNICODE_STRING uni,
	PANSI_STRING ansi,
	BOOLEAN doalloc);
PLARGE_UNICODE_STRING LargeStringAnsiToUnicode(PLARGE_UNICODE_STRING LargeAnsiString, PLARGE_UNICODE_STRING LargeUnicodeString);
PLARGE_UNICODE_STRING CaptureAnsiWindowName(PLARGE_UNICODE_STRING WindowName, PLARGE_UNICODE_STRING UnicodeWindowName);