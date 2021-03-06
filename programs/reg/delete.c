/*
 * Copyright 2016-2017, 2021 Hugh McMaster
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "reg.h"

int reg_delete(HKEY root, WCHAR *path, WCHAR *key_name, WCHAR *value_name,
               BOOL value_empty, BOOL value_all, BOOL force)
{
    HKEY key;

    if (!force)
    {
        BOOL ret;

        if (value_name || value_empty)
            ret = ask_confirm(STRING_DELETE_VALUE, value_name);
        else if (value_all)
            ret = ask_confirm(STRING_DELETE_VALUEALL, key_name);
        else
            ret = ask_confirm(STRING_DELETE_SUBKEY, key_name);

        if (!ret)
        {
            output_message(STRING_CANCELLED);
            return 0;
        }
    }

    /* Delete subtree only if no /v* option is given */
    if (!value_name && !value_empty && !value_all)
    {
        if (RegDeleteTreeW(root, path) != ERROR_SUCCESS)
        {
            output_message(STRING_CANNOT_FIND);
            return 1;
        }
        output_message(STRING_SUCCESS);
        return 0;
    }

    if (RegOpenKeyW(root, path, &key) != ERROR_SUCCESS)
    {
        output_message(STRING_CANNOT_FIND);
        return 1;
    }

    if (value_all)
    {
        DWORD max_value_len = 256, value_len;
        WCHAR *value_name;
        LONG rc;

        value_name = heap_xalloc(max_value_len * sizeof(WCHAR));

        while (1)
        {
            value_len = max_value_len;
            rc = RegEnumValueW(key, 0, value_name, &value_len, NULL, NULL, NULL, NULL);
            if (rc == ERROR_SUCCESS)
            {
                rc = RegDeleteValueW(key, value_name);
                if (rc != ERROR_SUCCESS)
                {
                    heap_free(value_name);
                    RegCloseKey(key);
                    output_message(STRING_VALUEALL_FAILED, key_name);
                    return 1;
                }
            }
            else if (rc == ERROR_MORE_DATA)
            {
                max_value_len *= 2;
                value_name = heap_xrealloc(value_name, max_value_len * sizeof(WCHAR));
            }
            else break;
        }
        heap_free(value_name);
    }
    else if (value_name || value_empty)
    {
        if (RegDeleteValueW(key, value_empty ? NULL : value_name) != ERROR_SUCCESS)
        {
            RegCloseKey(key);
            output_message(STRING_CANNOT_FIND);
            return 1;
        }
    }

    RegCloseKey(key);
    output_message(STRING_SUCCESS);
    return 0;
}
