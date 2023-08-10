/**
 * @file lookups.cpp
 * @brief Lookupable and lookupable manager base classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */

#include "lookups.h"

template <typename LookupableClass>
LookupableClass *LookupManager<LookupableClass>::getWithSymbol(char symbol)
{
    for (uint8_t i = 0; i < m_count; i++)
    {
        if (m_items[i].symbol == symbol)
        {
            // Found the item.
            return &m_items[i];
        }
    }
    // Couldn't find the item.
    return NULL;
}

template <typename LookupableClass>
LookupableClass *LookupManager<LookupableClass>::getWithName(const char *name)
{
    for (uint8_t i = 0; i < m_count; i++)
    {
        if (strcmp(m_items[i].name, name))
        {
            // Found the item.
            return &m_items[i];
        }
    }
    // Couldn't find the item.
    return NULL;
}