#include "peggybuild.h"
#include "peggystring.h"
#include "peggyparser.h"

PeggyString get_rule_resolution(char const * type_name) {
    size_t N = strlen(type_name);
    char * first_period = strchr(type_name, '.');
    return (PeggyString) {.len = N - (size_t)(first_period - type_name), .str = first_period};
}

int PeggyProduction_write_arg(void * data, PeggyString arg) {
    PeggyParser * parser = (PeggyParser *) data;
    fwrite(",\n\t", sizeof(char), 3, parser->source_file);
    PeggyString_fwrite(arg, parser->source_file, PSFO_NONE);
    return 0;
}

// signature is so that it can be used by for_each in the hash map
int PeggyProduction_define(void * parser_, PeggyString name, PeggyProduction prod) {
    PeggyParser * parser = (PeggyParser *)parser_;

    char const * type_name_cstr = get_type_name(prod.type);
    PeggyString type_main = {.len = strlen(type_name_cstr), .str = (char *)type_name_cstr};
    PeggyString_fwrite(type_main, parser->source_file, PSFO_UPPER);

    fputc('(', parser->source_file);
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_NONE);
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building definition for production %.*s\n", __func__, (int)prod.identifier.len, prod.identifier.str);
    fputc(',', parser->source_file);

    unsigned int offset = 0;
    if (PeggyString_startswith(prod.identifier, parser->export)) {
        offset = (unsigned int)(parser->export.len + 1);
    }
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_UPPER | PSFO_LOFFSET(offset));

    prod.args._class->for_each(&prod.args, &PeggyProduction_write_arg, (void*)parser);

    fwrite(");\n", sizeof(char), strlen(");\n"), parser->source_file);

    return 0;
}

int build_export_rules_resolved(void * parser_, PeggyString name, PeggyProduction prod) {
    PeggyParser * parser = (PeggyParser *) parser_;
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building export rules for %.*s\n", __func__, prod.name.len, prod.name.str);
    
    PeggyString arg = {.len = 9 + prod.identifier.len};
    arg.str = MemPoolManager_malloc(parser->str_mgr, sizeof(char) * (arg.len + 1));
    memcpy((void *)(arg.str), "(Rule *)", 8);
    arg.str[8] = '&';
    memcpy((void*)(arg.str + 9), (void*)prod.identifier.str, prod.identifier.len);

    PeggyString_fwrite(arg, parser->source_file, PSFO_NONE);
    char * buffer = ",\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->source_file);

    //free(arg.str);

    return 0;
}

void build_export_rules(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - building export rules\n", __func__);
    char const * buffer = NULL;
    FILE * file = parser->source_file;
    buffer = "\n\nRule * ";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_UPPER);

    buffer = "_NRULES + 1] = {\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), file);

    parser->productions._class->for_each(&parser->productions, &build_export_rules_resolved, (void*)parser);

    buffer = "NULL\n};\n\n";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    
}

void build_destructor(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - building destructor\n", __func__);
    char const * buffer = NULL;
    FILE * file = parser->source_file;
    buffer = "void ";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "_dest(void) {\n\tint i = 0;\n\twhile (";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]) {\n\t\t";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]->_class->dest(";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]);\n\t\ti++;\n\t}\n}\n";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
}

PeggyProduction PeggyProduction_build(PeggyParser * parser, ASTNode * id, RuleTypeID type) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building PeggyProduction rule id %d from line %u, col %u\n", __func__, id->rule->id, id->token_start->coords.line, id->token_start->coords.col);
    PeggyProduction prod;
    STACK_INIT(PeggyString)(&prod.args, 0);

    prod.name = get_string_from_parser(parser, id);
    prod.identifier.len = parser->export.len + 1 + prod.name.len;
    prod.identifier.str = MemPoolManager_malloc(parser->str_mgr, sizeof(char) * (prod.identifier.len + 1)); // +1 for underscore
    char * buffer = copy_export(parser, prod.identifier.str);
    buffer[0] = '_';
    buffer++;
    memcpy(buffer, prod.name.str, prod.name.len);
    if (type != PEGGY_NOTRULE) {
        prod.type = type;
    } else {
        prod.type = PEGGY_PRODUCTION;
    }    

    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - adding build of production rule id %d with name %.*s from line %u, col %u to production map\n", __func__, id->rule->id, prod.name.len, prod.name.str, id->token_start->coords.line, id->token_start->coords.col);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    return prod;
}

void PeggyProduction_declare(PeggyParser * parser, PeggyProduction prod) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - declaring type %.*s\n", __func__, prod.name.len, prod.name.str);
    //printf("declaring type: %s\n", prod.type_name);

    char const * main_type_end = get_type_name(prod.type);
    PeggyString type = {.len = strlen(main_type_end), .str = (char *)main_type_end};
    PeggyString_fwrite(type, parser->source_file, PSFO_NONE);
    fputc(' ', parser->source_file);
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_NONE);
    // for testing
    fwrite("; // ", 1, strlen("; // "), parser->source_file);
    
    unsigned int offset = 0;
    if (PeggyString_startswith(prod.identifier, parser->export)) {
        offset = (unsigned int)(parser->export.len + 1);
    }

    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_UPPER | PSFO_LOFFSET(offset));
    fputc('\n', parser->source_file);

    PeggyString_fwrite(prod.identifier, parser->header_file, PSFO_UPPER | PSFO_LOFFSET(offset));
    fwrite(",\n\t", 1, strlen(",\n\t"), parser->header_file);
    fflush(parser->header_file);
    fflush(parser->source_file);
}

void production_init(PeggyParser * parser, PeggyString name, PeggyProduction * prod) {
    STACK_INIT(PeggyString)(&prod->args, 0);
    prod->name = name;
}



#define PREP_OUTPUT_VAR_BUFFER_SIZE 256
void prep_output_files(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - prepping output files\n", __func__);
    char var_buffer[PREP_OUTPUT_VAR_BUFFER_SIZE];
    char const * buffer = NULL;
    int nbytes = 0;

    // write header in .h file
    buffer = "/* this file is auto-generated, do not modify */\n#ifndef ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    buffer = "_H\n#define ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    buffer = "_H\n\n#include <peggy/utils.h>\n#include <peggy/rule.h>\n#include <peggy/parser.h>\n\ntypedef enum ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);
    buffer = "rule {\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    // write header in .c file

    nbytes = snprintf(var_buffer, PREP_OUTPUT_VAR_BUFFER_SIZE, "/* this file is auto-generated, do not modify */\n#include <peggy/parser_gen.h>\n#include \"%s\"\n", parser->header_name);
    fwrite(var_buffer, 1, nbytes, parser->source_file);
    fflush(parser->header_file);
}

err_type open_output_files(PeggyParser * parser) {
    size_t name_length = parser->export.len;
    parser->header_name = MemPoolManager_malloc(parser->str_mgr, 2* (sizeof(*parser->header_name) * (name_length + 3)));
    if (!parser->header_name) {
        return PEGGY_MALLOC_FAILURE;
    }
    
    memcpy((void*)parser->header_name, (void*)parser->export.str, name_length);
    parser->header_name[name_length] = '.';
    parser->header_name[name_length+1] = 'h';
    parser->header_name[name_length+2] = '\0';
    parser->source_name = parser->header_name + name_length + 3;
    memcpy((void*)parser->source_name, (void*)parser->export.str, name_length);
    parser->source_name[name_length] = '.';
    parser->source_name[name_length+1] = 'c';
    parser->source_name[name_length+2] = '\0';

    if (!(parser->header_file = fopen(parser->header_name, "w"))) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to open header file %s\n", __func__, parser->header_name);
        return PEGGY_FILE_IO_ERROR;
    }
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - opened header file %s\n", __func__, parser->header_name);
    if (!(parser->source_file = fopen(parser->source_name, "w"))) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to open source file %s\n", __func__, parser->source_name);
        return PEGGY_FILE_IO_ERROR;
    }
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - opened source file %s\n", __func__, parser->source_name);
    return PEGGY_SUCCESS;
}

void cleanup_header_file(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - cleaning up header file %s\n", __func__, parser->header_name);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    char * buffer = "_NRULES\n} ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);
    buffer = "rule;";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    buffer = "\n\nextern Production ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_token;\nextern Production ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = ";\n\nvoid ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_dest(void);\n\n#endif //";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);

    buffer = "_H\n";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    fflush(parser->header_file);
}

int PeggyProduction_cleanup(void * data, PeggyString name, PeggyProduction prod) {
    prod.args._class->dest(&prod.args);
    return 0;
}

