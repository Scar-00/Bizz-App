; ModuleID = 'probe4.64d1ced41606775e-cgu.0'
source_filename = "probe4.64d1ced41606775e-cgu.0"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@alloc_7a60fb313ae9c3172821a6dfb2be3420 = private unnamed_addr constant <{ [75 x i8] }> <{ [75 x i8] c"/rustc/903e279f468590fa3425f8aff7f3d61a5a873dbb\\library\\core\\src\\num\\mod.rs" }>, align 1
@alloc_796ad40fc7c42f9c0b5f806aae0816be = private unnamed_addr constant <{ ptr, [16 x i8] }> <{ ptr @alloc_7a60fb313ae9c3172821a6dfb2be3420, [16 x i8] c"K\00\00\00\00\00\00\00w\04\00\00\05\00\00\00" }>, align 8
@str.0 = internal constant [25 x i8] c"attempt to divide by zero"

; probe4::probe
; Function Attrs: uwtable
define void @_ZN6probe45probe17h92829a2cc31a024bE() unnamed_addr #0 {
start:
  %0 = call i1 @llvm.expect.i1(i1 false, i1 false)
  br i1 %0, label %panic.i, label %"_ZN4core3num21_$LT$impl$u20$u32$GT$10div_euclid17h71f6ebb0c2fa50eaE.exit"

panic.i:                                          ; preds = %start
; call core::panicking::panic
  call void @_ZN4core9panicking5panic17h4840b15293184e84E(ptr align 1 @str.0, i64 25, ptr align 8 @alloc_796ad40fc7c42f9c0b5f806aae0816be) #3
  unreachable

"_ZN4core3num21_$LT$impl$u20$u32$GT$10div_euclid17h71f6ebb0c2fa50eaE.exit": ; preds = %start
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare i1 @llvm.expect.i1(i1, i1) #1

; core::panicking::panic
; Function Attrs: cold noinline noreturn uwtable
declare void @_ZN4core9panicking5panic17h4840b15293184e84E(ptr align 1, i64, ptr align 8) unnamed_addr #2

attributes #0 = { uwtable "target-cpu"="x86-64" }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(none) }
attributes #2 = { cold noinline noreturn uwtable "target-cpu"="x86-64" }
attributes #3 = { noreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 8, !"PIC Level", i32 2}
