/**
 * @file lookups.h
 * @brief Lookupable and lookupable manager base classes.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */

#pragma once

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
    LookupManager(LookupableClass* items, const uint8_t count) : m_items(items), m_count(count) {}

    /**
     * @brief Gets the object with the given symbol.
     *
     * @param symbol
     * @return Lookupable*
     */
    LookupableClass *getWithSymbol(char symbol)
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

    /**
     * @brief Gets the object matching the given name.
     *
     */
    LookupableClass *getWithName(const char *name)
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

    LookupableClass* const m_items;
    const uint8_t m_count;
};
