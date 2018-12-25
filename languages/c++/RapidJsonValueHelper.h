#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>


template<typename ...Args>
int ValueGetInt(const rapidjson::GenericValue<Args...>& value)
{
    if (value.IsInt())
        return value.GetInt();
    if (value.IsBool())
        return value.GetBool();
    //if (value.IsDouble())
    //    return value.GetDouble();
    if (value.IsString()
            && StrIsDigit(value.GetString()))
        return atoi(value.GetString());
    throw bad_cast();
}

template<typename ...Args>
bool ValueGetBool(const rapidjson::GenericValue<Args...>& value)
{
    if (value.IsBool())
        return value.GetBool();
    if (value.IsInt() && InRange(value.GetInt(), 0, 2))
        return value.GetInt() == 1;
    //if (value.IsDouble())
    //    return value.GetDouble();
    if (value.IsString())
    {
        if (StrIsDigit(value.GetString()))
        {
            auto v = atoi(value.GetString());
            if (InRange(v, 0, 2))
                return v == 1;
        }
        else
        {
            if (stricmp(value.GetString(), "false") == 0)
                return false;
            else if (stricmp(value.GetString(), "true") == 0)
                return true;
        }
    }
    throw bad_cast();
}

