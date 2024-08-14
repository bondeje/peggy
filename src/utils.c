#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <peggy/utils.h>

/**
 * @brief utility to get a string from the enum identifiers for RuleTypeID above
 */
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

/**
 * @brief check if a rule instance given by RuleTypeID is within the types 
 * supplied
 * @param[in] type the type of the rule: RuleType.id
 * @param[in] types the types to check against
 * @returns true if type is in types else false
 */
bool isinstance(RuleTypeID const type, RuleTypeID const * types) {
    while (*types != PEGGY_NOTRULE) {
        if (*types == type) {
            return true;
        }
        types++;
    }
    return false;
}

