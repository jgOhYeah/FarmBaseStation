/**
 * @file fields.h
 * @brief Contains fields classes.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-08
 */

/**
 * @brief Class for handling data fields in messages.
 * 
 * @tparam DataFormat the data type of the field.
 */
template<typename DataFormat>
class Field {
    public:
        Field(const char* name, char symbol): name(name), symbol(symbol) {} 
        const char* const name;
        const char symbol;

        /**
         * @brief Encodes the value as bytes ready to send.
         * 
         * @param value the value to encode.
         * @param bytes pointer to the memory to write the encoded value to.
         */
        virtual void encode(DataFormat value, uint8_t* bytes) {}

        /**
         * @brief Decodes the value from bytes.
         * 
         * @param bytes pointer to the memory to read from.
         * @return DataFormat the value.
         */
        virtual DataFormat decode(uint8_t* bytes) {}

        /**
         * @brief Returns the symbol in write mode (MSB set to 1).
         */
        char writeSymbol() {
            return symbol | 0x80;
        }
};

class Device {
    public:
        Device(const char* name, const char address): name(name), address(address) {}
        const char* const name;
        const char address;
};