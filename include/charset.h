#define wchar               unsigned short // Unicode is encoded in 16 bits

#define DEST_TOO_SMALL        0
#define UTF8_BAD_STRING      -1
#define INCOMPATIBLE_UNICODE -1
#define INCOMPATIBLE_EASCII  -1
#define INCOMPATIBLE_ISO8859 -1

unsigned int easciiToIso8859(const char * capSrc,unsigned int iSrcLen,char * capDest,unsigned int iDestLen);
unsigned int iso8859ToUnicode(const char * capSrc,unsigned int iSrcLen,wchar * wcapDest,unsigned int iDestLen);
unsigned int unicodeToUtf8(const wchar * wcapSrc,unsigned int iSrcLen,char * capDest,unsigned int iDestLen);
unsigned int utf8ToUnicode(const char * capSrc,unsigned int iSrcLen,wchar * wcapDest,unsigned int iDestLen);
unsigned int unicodeToIso8859(const wchar * wcapSrc,unsigned int iSrcLen,char * capDest,unsigned int iDestLen);
unsigned int iso8859ToEascii(const char * capSrc,unsigned int iSrcLen,char * capDest,unsigned int iDestLen);
