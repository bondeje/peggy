#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <peggy/utils.h>

#if CHAR_BIT == 8
unsigned char const CHAR_BIT_DIV_SHIFT = 3;
unsigned char const CHAR_BIT_MOD_MASK = 7;
#elif CHAR_BIT == 4
unsigned char const CHAR_BIT_MOD_SHIFT = 2;
unsigned char const CHAR_BIT_MOD_MASK = 3;
#elif CHAR_BIT == 16
unsigned char const CHAR_BIT_MOD_SHIFT = 4;
unsigned char const CHAR_BIT_MOD_MASK = 15;
#endif

char const * get_type_name(RuleTypeID type) {
    char const * rule_resolve = NULL;
    switch (type) {
        case PEGGY_SEQUENCE: {
            rule_resolve = "SequenceRule";
            break;
        }
        case PEGGY_CHOICE: {
            rule_resolve = "ChoiceRule";
            break;
        }
        case PEGGY_LITERAL: {
            rule_resolve = "LiteralRule";
            break;
        }
        case PEGGY_LIST: {
            rule_resolve = "ListRule";
            break;
        }
        case PEGGY_REPEAT: {
            rule_resolve = "RepeatRule";
            break;
        }
        case PEGGY_NEGATIVELOOKAHEAD: {
            rule_resolve = "NegativeLookahead";
            break;
        }
        case PEGGY_POSITIVELOOKAHEAD: {
            rule_resolve = "PositiveLookahead";
            break;
        }
        case PEGGY_PRODUCTION: {
            rule_resolve = "Production";
            break;
        }
        default: {
            // do nothing
        }
    }
    return rule_resolve;
}

// is here only due because of a potential use case for identifying parser types. Otherwise only used in rule.c
bool isinstance(RuleTypeID const type, RuleTypeID const * types) {
    while (*types != PEGGY_NOTRULE) {
        if (*types == type) {
            return true;
        }
        types++;
    }
    return false;
    /*
    char buffer[256] = {'\0'}; // very much should not be static otherwise thread safety of the library is out the window
    while (*types != NULL) {
        unsigned char len = (unsigned char)(strchr(*types, '.') - *types);
        if (!len) {
            len = (unsigned char) strlen(*types);
        }
        memcpy((void *) buffer, *types, len);
        buffer[len] = '\0';
        if (strstr(type, buffer)) {
            return true;
        }
        types++;
    }
    return false;
    */
}

