#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>

void create_one_function(LLVMModuleRef mod, LLVMBuilderRef builder, LLVMContextRef ctx)
{
    LLVMTypeRef i64_type = LLVMInt64TypeInContext(ctx);

    LLVMTypeRef one_function_type = LLVMFunctionType(i64_type, NULL, 0, 0);
    LLVMValueRef one_function = LLVMAddFunction(mod, "one", one_function_type);

    LLVMBasicBlockRef one_entry = LLVMAppendBasicBlockInContext(ctx, one_function, "entry");
    LLVMPositionBuilderAtEnd(builder, one_entry);

    LLVMValueRef one_value = LLVMConstInt(i64_type, 1, 0);
    LLVMBuildRet(builder, one_value);
}

void create_num_function(LLVMModuleRef mod, LLVMBuilderRef builder, LLVMContextRef ctx)
{
    LLVMTypeRef i64_type = LLVMInt64TypeInContext(ctx);

    LLVMTypeRef param_s[1] = { LLVMInt64TypeInContext(ctx) };

    LLVMTypeRef num_function_type = LLVMFunctionType(i64_type, param_s, 1, 0);
    LLVMValueRef num_function = LLVMAddFunction(mod, "num", num_function_type);

    LLVMValueRef n = LLVMGetParam(num_function, 0);
    LLVMSetValueName(n, "n");

    LLVMBasicBlockRef num_entry = LLVMAppendBasicBlockInContext(ctx, num_function, "entry");
    LLVMPositionBuilderAtEnd(builder, num_entry);

    LLVMBuildRet(builder, n);
}

int main()
{
    LLVMContextRef ctx = LLVMContextCreate();
    LLVMModuleRef mod = LLVMModuleCreateWithNameInContext("a001.iz", ctx);
    char *triple = LLVMGetDefaultTargetTriple();
    LLVMSetTarget(mod, triple);
    LLVMSetDataLayout(mod, "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128");

    LLVMBuilderRef builder = LLVMCreateBuilder();

    create_one_function(mod, builder, ctx);

    create_num_function(mod, builder, ctx);

    char *llvm_ir = LLVMPrintModuleToString(mod);
    printf("%s\n", llvm_ir);

    LLVMDisposeMessage(llvm_ir);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(mod);
    LLVMContextDispose(ctx);

    return 0;
}