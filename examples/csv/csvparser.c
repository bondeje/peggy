#include "csv.h"
#include "csvparser.h"

char const * csvrule_names[NRULES] = {
    "punctuator_re",
    "punctuator",
    "crlf_re",
    "crlf",
    "whitespace_re",
    "whitespace",
    "crlf|whitespace",
    "(crlf|whitespace)*",
    "string_re"
    "string",
    "nonstring_field_re",
    "nonstring_field",
    "string|nonstring_field",
    "field",
    "crlf|punctuator|whitespace|field",
    "token",
    "','.field",
    "record",
    "crlf.record",
    "crlc.record,(crlf|whitespace)*",
    "csv"
};

LiteralRule csv_punctuator_re = {
    ._class = &LiteralRule_class,
    .Rule = {
        ._class = &(LiteralRule_class.Rule_class),
        .id = PUNCTUATOR_RE,
        .cache_ = DEFAULT_PACKRATCACHE
    },
    .regex_s = "^(,)",
    .compiled = false
};
Production csv_punctuator = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = PUNCTUATOR,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_punctuator_re.Rule
        }
    },
    .build_action = &build_action_default,
};
LiteralRule csv_crlf_re = {
    ._class = &LiteralRule_class,
    .Rule = {
        ._class = &(LiteralRule_class.Rule_class),
        .id = CRLF_RE,
        .cache_ = DEFAULT_PACKRATCACHE
    },
    .regex_s = "^\r\n",
    .compiled = false
};
Production csv_crlf = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class), 
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = CRLF,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_crlf_re.Rule
        }
    },
    .build_action = &build_action_default,
};
LiteralRule csv_whitespace_re = {
    ._class = &LiteralRule_class,
    .Rule = {
        ._class = &(LiteralRule_class.Rule_class),
        .id = WHITESPACE_RE,
        .cache_ = DEFAULT_PACKRATCACHE
    },
    .regex_s = "^[[:space:]]+",
    .compiled = false
};
Production csv_whitespace = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = WHITESPACE,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_whitespace_re.Rule
        }
    },
    .build_action = &build_action_default,
};
ChoiceRule csv_choice_crlf_whitespace = {
    ._class = &ChoiceRule_class,
    .ChainRule = {
        ._class = &(ChoiceRule_class.ChainRule_class),
        .Rule = {
            ._class = &(ChoiceRule_class.ChainRule_class.Rule_class),
            .id = CHOICE_CRLF_WHITESPACE,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .deps = (Rule * [2]) {&csv_crlf.AnonymousProduction.DerivedRule.Rule, &csv_whitespace.AnonymousProduction.DerivedRule.Rule},
        .deps_size = 2
    }
};
RepeatRule csv_repeat_choice_crlf_whitespace = {
    ._class = &RepeatRule_class,
    .DerivedRule = {
        ._class = &(RepeatRule_class.DerivedRule_class),
        .Rule = {
            ._class = &(RepeatRule_class.DerivedRule_class.Rule_class),
            .id = REPEAT_CHOICE_CRLF_WHITESPACE,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .rule = &csv_choice_crlf_whitespace.ChainRule.Rule
    },
    .min_rep = 0,
    .max_rep = 0
};
LiteralRule csv_string_re = {
    ._class = &LiteralRule_class,
    .Rule = {
        ._class = &(LiteralRule_class.Rule_class),
        .id = STRING_RE,
        .cache_ = DEFAULT_PACKRATCACHE
    },
    .regex_s = "^\"((\\\\\")|[^\"])*\"",
    .compiled = false
};
Production csv_string = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = STRING,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_string_re.Rule
        }
    },
    .build_action = &build_action_default,
};
LiteralRule csv_nonstring_field_re = {
    ._class = &LiteralRule_class,
    .Rule = {
        ._class = &(LiteralRule_class.Rule_class),
        .id = NONSTRING_FIELD_RE,
        .cache_ = DEFAULT_PACKRATCACHE
    },
    .regex_s = "^[^,\r\n]*",
    .compiled = false
};
Production csv_nonstring_field = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = NONSTRING_FIELD,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_nonstring_field_re.Rule
        }
    },
    .build_action = &build_action_default,
};
ChoiceRule csv_choice_string_nonstring_field = {
    ._class = &ChoiceRule_class,
    .ChainRule = {
        ._class = &(ChoiceRule_class.ChainRule_class),
        .Rule = {
            ._class = &(ChoiceRule_class.ChainRule_class.Rule_class),
            .id = CHOICE_STRING_NONSTRING_FIELD,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .deps = (Rule * [2]) {&csv_string.AnonymousProduction.DerivedRule.Rule, &csv_nonstring_field.AnonymousProduction.DerivedRule.Rule},
        .deps_size = 2
    }
};
Production csv_field = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = FIELD,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_choice_string_nonstring_field.ChainRule.Rule
        }
    },
    .build_action = &build_action_default,
};
ChoiceRule csv_choice_crlf_punctuator_whitespace_field;
Production csv_token = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = TOKEN,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_choice_crlf_punctuator_whitespace_field.ChainRule.Rule
        }
    },
    .build_action = &token_action,
};
ListRule csv_list_field = {
    ._class = &ListRule_class,
    .DerivedRule = {
        ._class = &(ListRule_class.DerivedRule_class),
        .Rule = {
            ._class = &(ListRule_class.DerivedRule_class.Rule_class),
            .id = LIST_FIELD,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .rule = &csv_field.AnonymousProduction.DerivedRule.Rule
    },
    .delim = &csv_punctuator_re.Rule
};
Production csv_record = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = RECORD,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_list_field.DerivedRule.Rule
        }
    },
    .build_action = &build_action_default,
};
ListRule csv_list_crlf_record = {
    ._class = &ListRule_class,
    .DerivedRule = {
        ._class = &(ListRule_class.DerivedRule_class),
        .Rule = {
            ._class = &(ListRule_class.DerivedRule_class.Rule_class),
            .id = LIST_FIELD,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .rule = &csv_record.AnonymousProduction.DerivedRule.Rule
    },
    .delim = &csv_crlf.AnonymousProduction.DerivedRule.Rule
};
SequenceRule csv_sequence_list_crlf_record_repeat_choice_crlf_whitespace = {
    ._class = &SequenceRule_class,
    .ChainRule = {
        ._class = &(SequenceRule_class.ChainRule_class),
        .Rule = {
            ._class = &(SequenceRule_class.ChainRule_class.Rule_class),
            .id = SEQUENCE_LIST_CRLF_RECORD_REPEAT_CHOICE_CRLF_WHITESPACE,
            .cache_ = DEFAULT_PACKRATCACHE
        },
        .deps = NULL,
        .deps_size = 0
    }
};
Production csv_csv = {
    ._class = &Production_class,
    .AnonymousProduction = {
        ._class = &(Production_class.AnonymousProduction_class),
        .DerivedRule = {
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class),
            .Rule = {
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class),
                .id = CSV,
                .cache_ = DEFAULT_PACKRATCACHE
            },
            .rule = &csv_sequence_list_crlf_record_repeat_choice_crlf_whitespace.ChainRule.Rule
        }
    },
    .build_action = &handle_csv,
};


Rule * csvrules[NRULES] = {
    &csv_punctuator_re.Rule,
    &csv_punctuator.AnonymousProduction.DerivedRule.Rule,
    &csv_crlf_re.Rule,
    &csv_crlf.AnonymousProduction.DerivedRule.Rule,
    &csv_whitespace_re.Rule,
    &csv_whitespace.AnonymousProduction.DerivedRule.Rule,
    &csv_choice_crlf_whitespace.ChainRule.Rule,
    &csv_repeat_choice_crlf_whitespace.DerivedRule.Rule,
    &csv_string_re.Rule,
    &csv_string.AnonymousProduction.DerivedRule.Rule,
    &csv_nonstring_field_re.Rule,
    &csv_nonstring_field.AnonymousProduction.DerivedRule.Rule,
    &csv_choice_string_nonstring_field.ChainRule.Rule,
    &csv_field.AnonymousProduction.DerivedRule.Rule,
    &csv_choice_crlf_punctuator_whitespace_field.ChainRule.Rule,
    &csv_token.AnonymousProduction.DerivedRule.Rule,
    &csv_list_field.DerivedRule.Rule,
    &csv_record.AnonymousProduction.DerivedRule.Rule,
    &csv_list_crlf_record.DerivedRule.Rule,
    &csv_sequence_list_crlf_record_repeat_choice_crlf_whitespace.ChainRule.Rule,
    &csv_csv.AnonymousProduction.DerivedRule.Rule
};

void csv_dest(void) {
    printf("tearing down csv\n");
    for (int i = 0; i < NRULES; i++) {
        csvrules[i]->_class->dest(csvrules[i]);
    }
}