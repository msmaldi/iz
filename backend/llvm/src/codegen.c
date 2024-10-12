#include "codegen.h"
#include "map.h"
#include "common/mem.h"

#include <stdbool.h>
#include <string.h>

#include <llvm-c/Linker.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

LLVMValueRef LLVMAddFunction2(LLVMModuleRef M, span_t name, LLVMTypeRef FunctionTy)
{
    char name_sz[name.size + 1];
    memcpy(name_sz, name.data, name.size);
    name_sz[name.size] = 0;

    return LLVMAddFunction(M, name_sz, FunctionTy);
}

struct codegen_t
{
    array_t(unit_t) unit_s;

    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMContextRef context;
    map_t values;
};

codegen_t codegen_new(array_t(unit_t) unit_s)
{
    codegen_t codegen = mem_alloc(sizeof(struct codegen_t));

    codegen->unit_s = unit_s;

    codegen->module = LLVMModuleCreateWithName("code");
    codegen->context = LLVMContextCreate();
    codegen->builder = LLVMCreateBuilder();
    codegen->values = map_new();

    return codegen;
}

void codegen_free(codegen_t codegen)
{
    array_cleanup_free(codegen->unit_s, (release_t)unit_free);

    LLVMDisposeBuilder(codegen->builder);
    LLVMContextDispose(codegen->context);
    LLVMDisposeModule(codegen->module);
    map_free(codegen->values);

    mem_free(codegen);
}

LLVMTypeRef codegen_type(codegen_t codegen, type_t type);

LLVMTypeRef codegen_callable(codegen_t codegen, callable_t callable)
{
    LLVMTypeRef llvm_return_type = codegen_type(codegen, callable_return_type(callable));

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
    case TYPE_INT:
        return LLVMInt32Type();
    case TYPE_CALLABLE:
        return codegen_callable(codegen, CALLABLE(type));
    default: __builtin_unreachable();
        return NULL;
    }
}

void codegen_function_prototype(codegen_t codegen, function_t function)
{
    LLVMTypeRef llvm_fn_type = codegen_type(codegen, function_type(function));

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

LLVMValueRef codegen_constant(codegen_t codegen, constant_t constant)
{
    switch (constant_kind(constant)) // LCOV_EXCL_LINE
    {
    case CONSTANT_U64:
    {
        u_int64_t u64 = constant_u64(constant);
        return LLVMConstInt(LLVMInt32Type(), u64, 0);
    }
    default:
        __builtin_unreachable();
    }
}

LLVMValueRef codegen_expression(codegen_t codegen, expression_t expression)
{
    switch (expression_kind(expression)) // LCOV_EXCL_LINE
    {
    case EXPRESSION_CONSTANT:
        return codegen_constant(codegen, CONSTANT(expression));
    case EXPRESSION_IDENTIFIER:
    {
        declaration_t declaration = identifier_declaration(IDENTIFIER(expression));
        return map_get(codegen->values, declaration);
    }
    default:
        __builtin_unreachable();
    }
}

void codegen_return(codegen_t codegen, return_t ret)
{
    LLVMValueRef llvm_expression = codegen_expression(codegen, return_expression(ret));
    LLVMBuildRet(codegen->builder, llvm_expression);
}

void codegen_statement(codegen_t codegen, statement_t statement)
{
    switch (statement_kind(statement)) // LCOV_EXCL_LINE
    {
    case STATEMENT_RETURN:
    {
        codegen_return(codegen, RETURN(statement));
        break;
    }
    default:
        __builtin_unreachable();
    }
}

void codegen_function(codegen_t codegen, function_t function)
{
    LLVMValueRef llvm_function = map_get(codegen->values, (declaration_t)function);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(llvm_function, "entry");
    LLVMPositionBuilderAtEnd(codegen->builder, entry);

    statement_t statement = function_statement(function);
    codegen_statement(codegen, statement);
}

void codegen_unit(codegen_t codegen, unit_t unit)
{
    size_t size = array_size(unit_declaration_s(unit));
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        if (declaration_kind(declaration) == DECLARATION_FUNCTION) // LCOV_EXCL_LINE
        {
            function_t function = FUNCTION(declaration);
            codegen_function_prototype(codegen, function);
        }
    }
    for (int i = 0; i < size; i++)
    {
        declaration_t declaration = unit_declaration_s(unit)[i];
        if (declaration_kind(declaration) == DECLARATION_FUNCTION) // LCOV_EXCL_LINE
        {
            function_t function = FUNCTION(declaration);
            codegen_function(codegen, function);
        }
    }
}

void codegen_generate(codegen_t codegen)
{
    size_t size = array_size(codegen->unit_s);
    for (int i = 0; i < size; i++)
    {
        unit_t unit = codegen->unit_s[i];
        codegen_unit(codegen, unit);
    }
}

bool codegen_validate(codegen_t codegen)
{
    char *error = NULL;
    bool failed = LLVMVerifyModule(codegen->module, LLVMPrintMessageAction, &error);
    LLVMDisposeMessage(error);

    return !failed;
}

void codegen_dump(codegen_t codegen)
{
    LLVMDumpModule(codegen->module);
}

bool codegen_emit_object(codegen_t codegen, const char *filename)
{
    LLVMInitializeNativeTarget();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();

    LLVMTargetRef target = NULL;
    char *error = NULL;

    if (LLVMGetTargetFromTriple(LLVMGetDefaultTargetTriple(), &target, &error)) // LCOV_EXCL_LINE
    {
        LLVMDisposeMessage(error);    // LCOV_EXCL_LINE
        return false;                 // LCOV_EXCL_LINE
    }

    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine
    (
        target,
        LLVMGetDefaultTargetTriple(),
        LLVMGetHostCPUName(),
        LLVMGetHostCPUFeatures(),
        LLVMCodeGenLevelAggressive,
        LLVMRelocDefault,
        LLVMCodeModelDefault
    );

    if (LLVMTargetMachineEmitToFile(target_machine, codegen->module, filename, LLVMObjectFile, &error))// LCOV_EXCL_LINE
    {
        LLVMDisposeMessage(error); // LCOV_EXCL_LINE
        return false;              // LCOV_EXCL_LINE
    }

    LLVMDisposeMessage(error);
    LLVMDisposeTargetMachine(target_machine);
    return true;
}
