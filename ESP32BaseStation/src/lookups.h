/**
 * @file lookups.h
 * @brief Lookupable and lookupable manager base classes.
 *
 * Note that the LookupManager class is implemented in the header file so that
 * templates work more easily.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-12
 */

#pragma once
#include "../defines.h"
/**
 * @brief Class for fields and devices that can be managed by LookupManager.
 *
 */
class Lookupable
{
public:
    Lookupable(const char *name, char symbol) : name(name), symbol(symbol) {}
    const char *const name;
    const char symbol;
};

/**
 * @brief Class for looking up and managing devices and fields.
 *
 */
template <typename LookupableClass>
class LookupManager
{
public:
    LookupManager(LookupableClass **items, uint8_t count) : m_items(items), m_count(count) {}

    /**
     * @brief Gets the object with the given symbol.
     *
     * @param symbol
     * @return LookupableClass*
     */
    LookupableClass *getWithSymbol(char symbol)
    {
        for (uint8_t i = 0; i < m_count; i++)
        {
            if (m_items[i]->symbol == symbol)
            {
                // Found the item.
                return m_items[i];
            }
        }
        // Couldn't find the item.
        return NULL;
    }

    /**
     * @brief Gets the object matching the given name.
     *
     */
    LookupableClass *getWithName(const char *name)
    {
        for (uint8_t i = 0; i < m_count; i++)
        {
            if (STRINGS_MATCH(m_items[i]->name, name))
            {
                // Found the item.
                return m_items[i];
            }
        }
        // Couldn't find the item.
        return NULL;
    }

    LookupableClass **m_items;
    uint8_t m_count;
};
