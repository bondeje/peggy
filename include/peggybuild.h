#ifndef PEGGYBUILD_H
#define PEGGYBUILD_H

#include "peggyparser.h"
#include "peggystring.h"

PeggyString get_rule_resolution(char const * type_name);

int PeggyProduction_write_arg(void * data, PeggyString arg);

// signature is so that it can be used by for_each in the hash map
int PeggyProduction_define(void * parser_, PeggyString name, PeggyProduction prod);

int build_export_rules_resolved(void * parser_, PeggyString name, PeggyProduction prod);

void build_export_rules(PeggyParser * parser);

void build_destructor(PeggyParser * parser);

PeggyProduction PeggyProduction_build(PeggyParser * parser, ASTNode * id, RuleTypeID type);

void PeggyProduction_declare(PeggyParser * parser, PeggyProduction prod);

void production_init(PeggyParser * parser, PeggyString name, PeggyProduction * prod);

void prep_output_files(PeggyParser * parser);

err_type open_output_files(PeggyParser * parser);

void cleanup_header_file(PeggyParser * parser);

int PeggyProduction_cleanup(void * data, PeggyString name, PeggyProduction prod);

#endif

