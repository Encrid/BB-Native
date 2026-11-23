/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#if defined(_WIN32)
    #define HELPER_EXPORT __declspec(dllexport)
#else
    #define HELPER_EXPORT [[gnu::visibility("default")]]
#endif

HELPER_EXPORT int nhelper_value(void);

int nhelper_value(void)
{
    return (HELPED);
}
