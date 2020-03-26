; ModuleID = 'test_union.c'
source_filename = "test_union.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.B = type { i32 }
%struct.C = type { i32 }
%struct.S = type { i32, %union.anon }
%union.anon = type { %struct.B* }

@main.b = private unnamed_addr constant %struct.B { i32 5 }, align 4
@main.c = private unnamed_addr constant %struct.C { i32 10 }, align 4
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.B, align 4
  %3 = alloca %struct.C, align 4
  %4 = alloca %struct.S*, align 8
  store i32 0, i32* %1, align 4
  %5 = bitcast %struct.B* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* bitcast (%struct.B* @main.b to i8*), i64 4, i32 4, i1 false)
  %6 = bitcast %struct.C* %3 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %6, i8* bitcast (%struct.C* @main.c to i8*), i64 4, i32 4, i1 false)
  %7 = call i8* @malloc(i64 16) #4
  %8 = bitcast i8* %7 to %struct.S*
  store %struct.S* %8, %struct.S** %4, align 8
  %9 = load %struct.S*, %struct.S** %4, align 8
  %10 = getelementptr inbounds %struct.S, %struct.S* %9, i32 0, i32 0
  store i32 5, i32* %10, align 8
  %11 = load %struct.S*, %struct.S** %4, align 8
  %12 = getelementptr inbounds %struct.S, %struct.S* %11, i32 0, i32 1
  %13 = bitcast %union.anon* %12 to %struct.B**
  store %struct.B* %2, %struct.B** %13, align 8
  %14 = load %struct.S*, %struct.S** %4, align 8
  %15 = getelementptr inbounds %struct.S, %struct.S* %14, i32 0, i32 1
  %16 = bitcast %union.anon* %15 to %struct.C**
  store %struct.C* %3, %struct.C** %16, align 8
  %17 = load %struct.S*, %struct.S** %4, align 8
  %18 = getelementptr inbounds %struct.S, %struct.S* %17, i32 0, i32 1
  %19 = bitcast %union.anon* %18 to %struct.B**
  %20 = load %struct.B*, %struct.B** %19, align 8
  %21 = getelementptr inbounds %struct.B, %struct.B* %20, i32 0, i32 0
  %22 = load i32, i32* %21, align 4
  %23 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %22)
  %24 = load %struct.S*, %struct.S** %4, align 8
  %25 = getelementptr inbounds %struct.S, %struct.S* %24, i32 0, i32 1
  %26 = bitcast %union.anon* %25 to %struct.C**
  %27 = load %struct.C*, %struct.C** %26, align 8
  %28 = getelementptr inbounds %struct.C, %struct.C* %27, i32 0, i32 0
  %29 = load i32, i32* %28, align 4
  %30 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %29)
  %31 = load %struct.S*, %struct.S** %4, align 8
  %32 = bitcast %struct.S* %31 to i8*
  call void @free(i8* %32)
  ret i32 0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #1

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

declare i32 @printf(i8*, ...) #3

declare void @free(i8*) #3

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { allocsize(0) }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
