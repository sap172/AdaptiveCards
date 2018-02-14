#include "DateTimeParser.h"
#include <iomanip>
#include <sstream>

using namespace AdaptiveCards;

DateTimeParser::DateTimeParser(const std::string& language)
{
    try
    {
        m_language = std::locale(language.c_str());
    }
    catch (...)
    {
        m_language = std::locale("");
    }
}

std::string DateTimeParser::GenerateString(DateTimePreparser text)
{
    std::wostringstream parsedostr;

    for (const auto& textSection : text.GetTextSections())
    {
        struct tm result{};
        result.tm_mday = textSection->GetDay();
        result.tm_mon = textSection->GetMonth();
        result.tm_year = textSection->GetYear() >= 1900 ? textSection->GetYear() - 1900 : 0;

        switch (textSection->GetFormat())
        {
            // using the put_time function the 3 formats are locale dependent
            parsedostr.imbue(m_language);
            case DateTimePreparsedTokenFormat::DateCompact:
                parsedostr << std::put_time(&result, L"%Ex");
                break;
            case DateTimePreparsedTokenFormat::DateShort:
                mktime(&result);
                parsedostr << std::put_time(&result, L"%a, %b %e, %Y");
                break;
            case DateTimePreparsedTokenFormat::DateLong:
                mktime(&result);
                parsedostr << std::put_time(&result, L"%A, %B %e, %Y");
                break;
            case DateTimePreparsedTokenFormat::RegularString:
            default:
                parsedostr << StringToWstring(textSection->GetText());
                break;
        }
    }

    return WstringToString(parsedostr.str());
}

std::wstring DateTimeParser::StringToWstring(const std::string& in) const
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utfConverter;
    return utfConverter.from_bytes(in);
}

std::string DateTimeParser::WstringToString(const std::wstring& input) const
{
    if (sizeof(wchar_t) == 2)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utfConverter;
        return utfConverter.to_bytes(input);
    }
    else
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utfConverter;
        return utfConverter.to_bytes(input);
    }
}