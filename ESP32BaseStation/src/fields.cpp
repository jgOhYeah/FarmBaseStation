/**
 * @file fields.cpp
 * @brief Contains fields classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-09
 */
#include "fields.h"

char Field::writeSymbol()
{
    return symbol | 0x80;
}

