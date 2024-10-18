#ifndef P4CBUILD_H
#define P4CBUILD_H

#include "peg4cparser.h"
#include "peg4cstring.h"

P4CString get_rule_resolution(char const * type_name);

int P4CProduction_write_arg(void * data, P4CString arg);

// signature is so that it can be used by for_each in the hash map
int P4CProduction_define(void * parser_, P4CString name, P4CProduction prod);

int build_export_rules_resolved(void * parser_, P4CString name, P4CProduction prod);

void build_export_rules(P4CParser * parser);

void build_destructor(P4CParser * parser);

P4CProduction P4CProduction_build(P4CParser * parser, ASTNode * id, RuleTypeID type);

void P4CProduction_declare(P4CParser * parser, P4CProduction prod);

void production_init(P4CParser * parser, P4CString name, P4CProduction * prod);

void prep_output_files(P4CParser * parser);

err_type open_output_files(P4CParser * parser);

void cleanup_header_file(P4CParser * parser);

int P4CProduction_cleanup(void * data, P4CString name, P4CProduction prod);

#endif

