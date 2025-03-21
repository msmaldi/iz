#include "llvm/backend.h"
#include "common/mem.h"
#include "map.h"

#include <string.h>
#include <llvm-c/Linker.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Support.h>

#include <llvm-c/Linker.h>

struct backend_t
{
    compilation_t           compilation;

    LLVMContextRef          context;
    array_t(LLVMModuleRef)  module_s;

    LLVMTargetRef           target;
    LLVMTargetMachineRef    machine;
};

typedef struct codegen_t* codegen_t;
struct codegen_t
{
    LLVMModuleRef  module;
    LLVMBuilderRef builder;
    LLVMContextRef context;
    map_t values;

    LLVMValueRef function;
};

array_t(LLVMModuleRef) codegen_compilation(codegen_t codegen, compilation_t compilation);

static
codegen_t codegen_new()
{
    codegen_t codegen = mem_alloc(sizeof(struct codegen_t));

    codegen->module = NULL;
    codegen->context = LLVMContextCreate();
    codegen->builder = LLVMCreateBuilderInContext(codegen->context);
    codegen->values = map_new();

    return codegen;
}

static
void codegen_free(codegen_t codegen)
{
    LLVMDisposeBuilder(codegen->builder);
    map_free(codegen->values);
    mem_free(codegen);
}

backend_t backend_new(compilation_t compilation, LLVMContextRef context, array_t(LLVMModuleRef) module_s)
{
    backend_t backend = mem_alloc(sizeof(struct backend_t));
    backend->compilation = compilation;
    backend->context = context;
    backend->module_s = module_s;

    return backend;
}

void backend_free(backend_t backend)
{
    compilation_free(backend->compilation);
    array_cleanup_free(backend->module_s, (release_t)LLVMDisposeModule);
    LLVMContextDispose(backend->context);
    mem_free(backend);
}

void backend_dump(backend_t backend)
{
    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);

    for (size_t i = 0 ; i < size; i++)
        LLVMDumpModule(module_s[i]);
}

void backend_emit_object(backend_t backend)
{
    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);

    for (size_t i = 0 ; i < size; i++)
    {
        char *error = NULL;
        LLVMModuleRef module = module_s[i];

        source_t source = unit_source(compilation_unit_s(backend->compilation)[i]);
        char *filename = source_object_name(source);
        LLVMTargetMachineEmitToFile(backend->machine, module, filename, LLVMObjectFile, &error);

        mem_free(filename);
    }
}

void backend_emit_assembly(backend_t backend)
{
    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);

    for (size_t i = 0 ; i < size; i++)
    {
        char *error = NULL;
        LLVMModuleRef module = module_s[i];

        source_t source = unit_source(compilation_unit_s(backend->compilation)[i]);
        char *filename = source_assembly_name(source);
        LLVMTargetMachineEmitToFile(backend->machine, module, filename, LLVMAssemblyFile, &error);

        mem_free(filename);
    }
}

bool backend_validate(backend_t backend)
{
    bool failed = false;
    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);
    for (size_t i = 0 ; i < size; i++)
    {
        char *error = NULL;
        failed = LLVMVerifyModule(module_s[i], LLVMPrintMessageAction, &error);
        LLVMDisposeMessage(error);
    }

    return !failed;
}

void backend_optimize(backend_t backend)
{
    LLVMPassBuilderOptionsRef pass_builder_options = LLVMCreatePassBuilderOptions();

    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);
    for (size_t i = 0 ; i < size; i++)
    {
        LLVMRunPasses(module_s[i], "default<O3>", backend->machine, pass_builder_options);
    }

    LLVMDisposePassBuilderOptions(pass_builder_options);
}

void backend_emit_llvm(backend_t backend)
{
    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);

    for (size_t i = 0 ; i < size; i++)
    {
        char *error = NULL;
        LLVMModuleRef module = module_s[i];

        source_t source = unit_source(compilation_unit_s(backend->compilation)[i]);
        char *filename = source_llvm_name(source);
        LLVMPrintModuleToFile(module, filename, &error);

        mem_free(filename);
    }
}

void backend_setup(backend_t backend)
{
    LLVMInitializeNativeTarget();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();

    char *error = NULL;
    LLVMTargetRef target = NULL;
    LLVMGetTargetFromTriple(LLVMGetDefaultTargetTriple(), &target, &error);

    LLVMTargetMachineRef machine = LLVMCreateTargetMachine
    (
        target,
        LLVMGetDefaultTargetTriple(),
        LLVMGetHostCPUName(),
        LLVMGetHostCPUFeatures(),
        LLVMCodeGenLevelAggressive,
        LLVMRelocPIC,
        LLVMCodeModelLarge
    );

    backend->target = target;
    backend->machine = machine;

    array_t(LLVMModuleRef) module_s = backend->module_s;
    size_t size = array_size(module_s);

    for (size_t i = 0 ; i < size; i++)
        LLVMSetTarget(module_s[i], LLVMGetDefaultTargetTriple());
}

backend_t backend_codegen(compilation_t compilation)
{
    codegen_t codegen = codegen_new();
    array_t(LLVMModuleRef) module_s = codegen_compilation(codegen, compilation);

    backend_t backend = backend_new(compilation, codegen->context, module_s);

    codegen_free(codegen);

    backend_setup(backend);

    return backend;
}

LLVMTypeRef codegen_type(codegen_t codegen, type_t type);

LLVMTypeRef codegen_callable(codegen_t codegen, callable_t callable)
{
    type_t return_type = callable_return_type(callable);
    LLVMTypeRef llvm_return_type = codegen_type(codegen, return_type);

    array_t(type_t) param_s = callable_param_s(callable);
    size_t size = array_size(param_s);

    LLVMTypeRef llvm_param_s[size];
    for (size_t i = 0; i < size; i++)
        llvm_param_s[i] = codegen_type(codegen, param_s[i]);

    LLVMTypeRef llvm_callable = LLVMFunctionType(llvm_return_type, llvm_param_s, size, 0);
    return llvm_callable;
}

LLVMTypeRef codegen_type(codegen_t codegen, type_t type)
{
    switch (type_kind(type)) // LCOV_EXCL_LINE
    {
        case TYPE_BOOL:
            return LLVMInt1TypeInContext(codegen->context);
        case TYPE_INT:
            return LLVMInt32TypeInContext(codegen->context);
        case TYPE_CALLABLE:
            return codegen_callable(codegen, CALLABLE(type));
    }
    return NULL;
}


LLVMValueRef codegen_expression(codegen_t codegen, expression_t expression);

LLVMValueRef codegen_constant(codegen_t codegen, constant_t constant)
{
    switch (constant_kind(constant)) // LCOV_EXCL_LINE
    {
        case CONSTANT_BOOL:
        {
            bool _bool = constant_bool(constant);
            return LLVMConstInt(LLVMInt1TypeInContext(codegen->context), _bool, 0);
        }
        case CONSTANT_U64:
        {
            u_int64_t u64 = constant_u64(constant);
            return LLVMConstInt(LLVMInt32TypeInContext(codegen->context), u64, 0);
        }
    }
    return NULL;
}

LLVMValueRef codegen_identifier(codegen_t codegen, identifier_t identifier)
{
    declaration_t declaration = identifier_declaration(identifier);

    if (declaration_kind(declaration) == DECLARATION_FUNCTION)
        return map_get(codegen->values, declaration);

    return map_get(codegen->values, declaration);

    //LLVMValueRef alloca = map_get(codegen->values, declaration);
    //LLVMTypeRef type = codegen_type(codegen, declaration_type(declaration));

    //return LLVMBuildLoad2(codegen->builder, type, alloca, "");
}

LLVMValueRef codegen_binary(codegen_t codegen, binary_t binary)
{
    LLVMValueRef lhs = codegen_expression(codegen, binary_lhs(binary));
    LLVMValueRef rhs = codegen_expression(codegen, binary_rhs(binary));

    switch (binary_op(binary)) // LCOV_EXCL_LINE
    {
        case BINARY_LT:
            return LLVMBuildICmp(codegen->builder, LLVMIntSLT, lhs, rhs, "");
        case BINARY_LE:
            return LLVMBuildICmp(codegen->builder, LLVMIntSLE, lhs, rhs, "");
        case BINARY_GT:
            return LLVMBuildICmp(codegen->builder, LLVMIntSGT, lhs, rhs, "");
        case BINARY_GE:
            return LLVMBuildICmp(codegen->builder, LLVMIntSGE, lhs, rhs, "");
        case BINARY_EQ:
            return LLVMBuildICmp(codegen->builder, LLVMIntEQ, lhs, rhs, "");
        case BINARY_NE:
            return LLVMBuildICmp(codegen->builder, LLVMIntNE, lhs, rhs, "");
        case BINARY_ADD:
            return LLVMBuildAdd(codegen->builder, lhs, rhs, "");
        case BINARY_SUB:
            return LLVMBuildSub(codegen->builder, lhs, rhs, "");
        case BINARY_MUL:
            return LLVMBuildMul(codegen->builder, lhs, rhs, "");
        case BINARY_DIV:
            return LLVMBuildSDiv(codegen->builder, lhs, rhs, "");
        case BINARY_REM:
            return LLVMBuildSRem(codegen->builder, lhs, rhs, "");
    }
    return NULL;
}

LLVMValueRef codegen_call(codegen_t codegen, call_t call)
{
    expression_t callee = call_callee(call);
    type_t callee_ty = expression_type(callee);
    LLVMTypeRef llvm_call_ty = codegen_type(codegen, callee_ty);

    LLVMValueRef llvm_callee = codegen_expression(codegen, callee);
    array_t(expression_t) argument_s = call_argument_s(call);
    size_t size = array_size(argument_s);

    LLVMValueRef llvm_argument_s[size];
    for (size_t i = 0; i < size; i++)
        llvm_argument_s[i] = codegen_expression(codegen, argument_s[i]);

    return LLVMBuildCall2(codegen->builder, llvm_call_ty, llvm_callee, llvm_argument_s, size, "");
}

LLVMValueRef codegen_assignment(codegen_t codegen, assignment_t assignment)
{
    expression_t lvalue = assignment_lvalue(assignment);
    identifier_t identifier = IDENTIFIER(lvalue);

    LLVMValueRef llvm_lvalue = map_get(codegen->values, identifier_declaration(identifier));

    expression_t rvalue = assignment_rvalue(assignment);
    LLVMValueRef llvm_rvalue = codegen_expression(codegen, rvalue);

    LLVMBuildStore(codegen->builder, llvm_rvalue, llvm_lvalue);

    LLVMTypeRef type = codegen_type(codegen, expression_type(lvalue));

    return LLVMBuildLoad2(codegen->builder, type, llvm_lvalue, "");
}

LLVMValueRef codegen_implicit_cast(codegen_t codegen, implicit_cast_t implicit_cast)
{
    type_t type = implicit_cast_type(implicit_cast);
    LLVMTypeRef type_llvm = codegen_type(codegen, type);
    LLVMValueRef expression_llvm = codegen_expression(codegen, implicit_cast_expression(implicit_cast));

    switch (implicit_cast_kind(implicit_cast))
    {
    case IMPLICIT_CAST_LVALUE_TO_RVALUE:
        return LLVMBuildLoad2(codegen->builder, type_llvm, expression_llvm, "");
        break;
    default:
        return NULL;
    }
    return NULL;
}

LLVMValueRef codegen_conditional(codegen_t codegen, conditional_t conditional)
{
    LLVMValueRef lhs = codegen_expression(codegen, conditional_lhs(conditional));
    LLVMValueRef rhs = codegen_expression(codegen, conditional_rhs(conditional));

    LLVMBasicBlockRef rhsBlock = LLVMAppendBasicBlock(codegen->function, "cond.rhs");
    LLVMBasicBlockRef mergeBlock = LLVMAppendBasicBlock(codegen->function, "cond.merge");
    LLVMValueRef temp = LLVMBuildAlloca(codegen->builder, LLVMTypeOf(lhs), "");

    LLVMBuildStore(codegen->builder, lhs, temp);

    if (conditional_op(conditional) == CONDITIONAL_AND)
        LLVMBuildCondBr(codegen->builder, lhs, rhsBlock, mergeBlock);
    if (conditional_op(conditional) == CONDITIONAL_OR)
        LLVMBuildCondBr(codegen->builder, lhs, mergeBlock, rhsBlock);

    LLVMPositionBuilderAtEnd(codegen->builder, rhsBlock);
    LLVMBuildStore(codegen->builder, rhs, temp);
    LLVMBuildBr(codegen->builder, mergeBlock);

    LLVMPositionBuilderAtEnd(codegen->builder, mergeBlock);
    return LLVMBuildLoad2(codegen->builder, LLVMInt1TypeInContext(codegen->context), temp, "");
}

LLVMValueRef codegen_expression(codegen_t codegen, expression_t expression)
{
    switch (expression_kind(expression)) // LCOV_EXCL_LINE
    {
        case EXPRESSION_CONSTANT:
            return codegen_constant(codegen, CONSTANT(expression));
        case EXPRESSION_IDENTIFIER:
            return codegen_identifier(codegen, IDENTIFIER(expression));
        case EXPRESSION_BINARY:
            return codegen_binary(codegen, BINARY(expression));
        case EXPRESSION_CALL:
            return codegen_call(codegen, CALL(expression));
        case EXPRESSION_ASSIGNMENT:
            return codegen_assignment(codegen, ASSIGNMENT(expression));
        case EXPRESSION_IMPLICIT_CAST:
            return codegen_implicit_cast(codegen, IMPLICIT_CAST(expression));
        case EXPRESSION_CONDITIONAL:
            return codegen_conditional(codegen, CONDITIONAL(expression));
    }
    return NULL;
}


void codegen_statement(codegen_t codegen, statement_t statement);

static
void codegen_block(codegen_t codegen, block_t block)
{
    array_t(statement_t) statement_s = block_statement_s(block);
    size_t size = array_size(statement_s);
    for (int i = 0; i < size; i++)
    {
        statement_t statement = statement_s[i];
        codegen_statement(codegen, statement);
    }
}

static
void codegen_return(codegen_t codegen, return_t ret)
{
    LLVMValueRef llvm_expression = codegen_expression(codegen, return_expression(ret));
    LLVMBuildRet(codegen->builder, llvm_expression);
}

static
void codegen_if_only(codegen_t codegen, if_t ifelse)
{
    LLVMValueRef llvm_condition = codegen_expression(codegen, if_condition(ifelse));

    LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(codegen->function, "if.then");
    LLVMBasicBlockRef join_block = LLVMAppendBasicBlock(codegen->function, "if.join");

    LLVMBuildCondBr(codegen->builder, llvm_condition, then_block, join_block);

    LLVMPositionBuilderAtEnd(codegen->builder, then_block);
    codegen_statement(codegen, if_then_branch(ifelse));

    LLVMPositionBuilderAtEnd(codegen->builder, join_block);
}

static
void codegen_if_else(codegen_t codegen, if_t ifelse)
{
    LLVMValueRef llvm_condition = codegen_expression(codegen, if_condition(ifelse));

    bool then_terminator = statement_all_path_return_value(if_then_branch(ifelse));
    bool else_terminator = statement_all_path_return_value(if_else_branch(ifelse));
    bool both_terminator = then_terminator && else_terminator;

    LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(codegen->function, "if.then");
    LLVMBasicBlockRef else_block = LLVMAppendBasicBlock(codegen->function, "if.else");

    LLVMBasicBlockRef join_block = both_terminator ? NULL : LLVMAppendBasicBlock(codegen->function, "if.join");

    LLVMBuildCondBr(codegen->builder, llvm_condition, then_block, else_block);

    LLVMPositionBuilderAtEnd(codegen->builder, then_block);
    codegen_statement(codegen, if_then_branch(ifelse));

    if (!then_terminator)
        LLVMBuildBr(codegen->builder, join_block);

    LLVMPositionBuilderAtEnd(codegen->builder, else_block);
    codegen_statement(codegen, if_else_branch(ifelse));

    if (!else_terminator)
        LLVMBuildBr(codegen->builder, join_block);

    if (!both_terminator)
        LLVMPositionBuilderAtEnd(codegen->builder, join_block);
}

static
void codegen_if(codegen_t codegen, if_t ifelse)
{
    if (if_else_branch(ifelse) == NULL)
        codegen_if_only(codegen, ifelse);
    else
        codegen_if_else(codegen, ifelse);
}

void codegen_var(codegen_t codegen, var_t var)
{
    array_t(declaration_t) variable_s = var_variable_s(var);

    size_t size = array_size(variable_s);

    for (size_t i = 0; i < size; i++)
    {
        declaration_t declaration = variable_s[i];
        type_t type = declaration_type(declaration);
        LLVMTypeRef llvm_type = codegen_type(codegen, type);

        LLVMValueRef llvm_var = LLVMBuildAlloca(codegen->builder, llvm_type, "");
        map_at(codegen->values, declaration, llvm_var);

        variable_t variable = VARIABLE(declaration);
        expression_t expression = variable_initializer(variable);
        if (expression == NULL)
            continue;

        LLVMValueRef llvm_expression = codegen_expression(codegen, expression);

        LLVMBuildStore(codegen->builder, llvm_expression, llvm_var);
    }
}

void codegen_act(codegen_t codegen, act_t act)
{
    array_t(expression_t) expression_s = act_expression_s(act);
    size_t size = array_size(expression_s);
    for (int i = 0; i < size; i++)
    {
        expression_t expression = expression_s[i];
        codegen_expression(codegen, expression);
    }
}

void codegen_statement(codegen_t codegen, statement_t statement)
{
    switch (statement_kind(statement)) // LCOV_EXCL_LINE
    {
        case STATEMENT_BLOCK:
            return codegen_block(codegen, BLOCK(statement));
        case STATEMENT_RETURN:
            return codegen_return(codegen, RETURN(statement));
        case STATEMENT_IF:
            return codegen_if(codegen, IF(statement));
        case STATEMENT_VAR:
            return codegen_var(codegen, VAR(statement));
        case STATEMENT_ACT:
            return codegen_act(codegen, ACT(statement));
    }
}

LLVMValueRef LLVMAddFunction2(LLVMModuleRef M, span_t name, LLVMTypeRef FunctionTy)
{
    char name_sz[name.size + 1];
    memcpy(name_sz, name.data, name.size);
    name_sz[name.size] = 0;

    return LLVMAddFunction(M, name_sz, FunctionTy);
}

void codegen_function_prototype(codegen_t codegen, function_t function)
{
    type_t fn_type = function_type(function);
    LLVMTypeRef llvm_fn_type = codegen_type(codegen, fn_type);

    LLVMValueRef llvm_function = LLVMAddFunction2(codegen->module, function_name(function), llvm_fn_type);
    map_at(codegen->values, (declaration_t)function, llvm_function);

    array_t(declaration_t) argument_s = function_argument_s(function);
    size_t size = array_size(argument_s);
    for (size_t i = 0; i < size; i++)
    {
        declaration_t argument = argument_s[i];
        LLVMValueRef llvm_arg = LLVMGetParam(llvm_function, i);

        span_t argument_name = declaration_name(argument);
        LLVMSetValueName2(llvm_arg, argument_name.data, argument_name.size);

        map_at(codegen->values, argument, llvm_arg);
    }
}

void alloca_and_store_arguments(codegen_t codegen, function_t function)
{
    LLVMValueRef llvm_function = codegen->function;

    array_t(declaration_t) argument_s = function_argument_s(function);
    size_t size = array_size(argument_s);

    LLVMValueRef llvmarg_s[size];
    LLVMValueRef alloca_s[size];

    for (size_t i = 0; i < size; i++)
    {
        llvmarg_s[i] = LLVMGetParam(llvm_function, i);
        alloca_s[i] = LLVMBuildAlloca(codegen->builder, LLVMTypeOf(llvmarg_s[i]), "");
    }

    for (size_t i = 0; i < size; i++)
    {
        LLVMBuildStore(codegen->builder, llvmarg_s[i], alloca_s[i]);
        declaration_t argument = argument_s[i];
        map_at(codegen->values, argument, alloca_s[i]);
    }
}

void codegen_function(codegen_t codegen, function_t function)
{
    LLVMValueRef llvm_function = map_get(codegen->values, (declaration_t)function);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(codegen->context, llvm_function, "entry");
    LLVMPositionBuilderAtEnd(codegen->builder, entry);

    codegen->function = llvm_function;

    alloca_and_store_arguments(codegen, function);

    statement_t statement = function_statement(function);
    codegen_statement(codegen, statement);

    codegen->function = NULL;
}

LLVMModuleRef codegen_unit(codegen_t codegen, unit_t unit)
{
    source_t source = unit_source(unit);
    const char *source_name = source_path(source);

    LLVMModuleRef module = LLVMModuleCreateWithNameInContext(source_name, codegen->context);

    codegen->module = module;
    size_t size = array_size(unit_declaration_s(unit));
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        function_t function = FUNCTION(declaration);
        codegen_function_prototype(codegen, function);
    }
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        function_t function = FUNCTION(declaration);
        codegen_function(codegen, function);
    }
    codegen->module = NULL;

    return module;
}

array_t(LLVMModuleRef) codegen_compilation(codegen_t codegen, compilation_t compilation)
{
    array_t(LLVMModuleRef) module_s = array_empty();
    array_t(unit_t) unit_s = compilation_unit_s(compilation);
    size_t size = array_size(unit_s);
    for (int i = 0; i < size; i++)
    {
        unit_t unit = unit_s[i];
        LLVMModuleRef module = codegen_unit(codegen, unit);
        module_s = array_add(module_s, module);
    }

    return module_s;
}
