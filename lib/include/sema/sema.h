#include "ast/ast.h"

typedef  struct sema_t*  sema_t;

sema_t sema_new(array_t(unit_t) unit_s);
void   sema_free(sema_t sema);

size_t sema_errors(sema_t sema);
void   sema_analysis(sema_t sema);