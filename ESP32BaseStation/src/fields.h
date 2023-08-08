/**
 * @file fields.h
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-08
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
class LookupManager
{
public:
    LookupManager(Lookupable* items, const uint8_t count): m_items(items), m_count(count) {}

    /**
     * @brief Gets the object with the given symbol.
     * 
     * @param symbol 
     * @return Lookupable* 
     */
    Lookupable* getWithSymbol(char symbol)
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
    Lookupable* getWithName(const char* name)
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

private:
    Lookupable* const m_items;
    const uint8_t m_count;
};

/**
 * @brief Class for handling data fields in messages.
 *
 * @tparam DataFormat the data type of the field.
 */
template <typename DataFormat>
class Field : public Lookupable
{
public:
    Field(const char *name, char symbol) : Lookupable(name, symbol) {}

    /**
     * @brief Encodes the value as bytes ready to send.
     *
     * @param value the value to encode.
     * @param bytes pointer to the memory to write the encoded value to.
     */
    virtual void encode(DataFormat value, uint8_t *bytes) {}

    /**
     * @brief Decodes the value from bytes.
     *
     * @param bytes pointer to the memory to read from.
     * @return DataFormat the value.
     */
    virtual DataFormat decode(uint8_t *bytes) {}

    /**
     * @brief Returns the symbol in write mode (MSB set to 1).
     */
    char writeSymbol()
    {
        return symbol | 0x80;
    }
};

/**
 * @brief Each sensor / device on the PJON network.
 *
 */
class Device : public Lookupable
{
public:
    Device(const char *name, const char symbol) : Lookupable(name, symbol) {}
};