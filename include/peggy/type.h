#ifndef PEGGY_TYPE_H
#define PEGGY_TYPE_H

#include <stddef.h>
#include <stdbool.h>

#include <peggy/utils.h>
#define Type_DEFAULT_INIT {._class = &Type_class, \
                           .type_name = Type_NAME, \
                           .new = &Type_new, \
                           .init = &Type_init, \
                           .del = &Type_del \
                           }

#define DOWNCAST_P(parent_p, parent_t, child_t) ((child_t *)(((char *)(parent_p)) - offsetof(child_t, parent_t)))
#define UPCAST_P(child_p, child_t, parent_t) CAT((child_p)->parent_t, _class)
#define UPCAST(child, child_t, parent_t) CAT((child).parent_t)

/* TODO: change Type to some meta-class when I can properly inherit type */
#define type(instance_p) ((instance_p)->_class)
#define type_name(instance_p) (((Type *)(type(instance_p)->_class))->type_name)
#define super(instance_p, parent_class) CAT((instance_p)->_class->, CAT(parent_class, _class))

/*
#define dir_(instance_p, include_private) (void)print_dir(((Type *)(type(instance_p)->_class)), include_private)
#define dir_2(instance_p, include_private) dir_(instance_p, include_private)
#define dir_1(instance_p) dir_(instance_p, 0)
#define dir(...) CAT(dir, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)
*/
typedef struct Type Type;

struct Type {
    //StructInfo * _info;
    Type const * _class;
    char const * type_name;    
    //Type * (*new)(StructInfo *, char const *);
    Type * (*new)(char const *);
    //err_type (*init)(Type *, StructInfo *, char const *);
    err_type (*init)(Type *, char const *);
    void (*del)(Type *);
};

//Type * Type_new(StructInfo * info, char const * type_name);
Type * Type_new(char const * type_name);
//err_type Type_init(Type * type, StructInfo * info, char const * type_name);
err_type Type_init(Type * self, char const * type_name);
void Type_del(Type * self);
//void print_dir(Type * type, bool include_private);

extern Type Type_class;

//bool isinstance(Type const * type, unsigned short ntypes, char const * types[ntypes]);
bool isinstance(Type const * type, unsigned short ntypes, Type const * types[ntypes]);

#endif // PEGGY_TYPE_H