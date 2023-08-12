/**
 * @file lookups.h
 * @brief Lookupable and lookupable manager base classes.
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
    LookupManager(const LookupableClass *const *items, const uint8_t count) : m_items(items), m_count(count) {}

    /**
     * @brief Gets the object with the given symbol.
     *
     * @param symbol
     * @return LookupableClass*
     */
    LookupableClass *getWithSymbol(char symbol);

    /**
     * @brief Gets the object matching the given name.
     *
     */
    LookupableClass *getWithName(const char *name);

    const LookupableClass *const *m_items;
    const uint8_t m_count;
};
