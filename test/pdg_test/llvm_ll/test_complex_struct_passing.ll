; ModuleID = 'test_complex_struct_passing.c'
source_filename = "test_complex_struct_passing.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.clothes = type { [10 x i8], i32 }
%struct.person_t = type { i32, [10 x i8], %struct.clothes* }

@.str = private unnamed_addr constant [24 x i8] c"%s is wearing %s today.\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { [10 x i8] c"red\00\00\00\00\00\00\00", i32 5 }, align 4
@.str.1 = private unnamed_addr constant [10 x i8] c"Jack\00\00\00\00\00\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f(%struct.person_t*) #0 {
  %2 = alloca %struct.person_t*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca i8*, align 8
  store %struct.person_t* %0, %struct.person_t** %2, align 8
  %5 = load %struct.person_t*, %struct.person_t** %2, align 8
  %6 = getelementptr inbounds %struct.person_t, %struct.person_t* %5, i32 0, i32 1
  %7 = getelementptr inbounds [10 x i8], [10 x i8]* %6, i32 0, i32 0
  store i8* %7, i8** %3, align 8
  %8 = load %struct.person_t*, %struct.person_t** %2, align 8
  %9 = getelementptr inbounds %struct.person_t, %struct.person_t* %8, i32 0, i32 2
  %10 = load %struct.clothes*, %struct.clothes** %9, align 8
  %11 = getelementptr inbounds %struct.clothes, %struct.clothes* %10, i32 0, i32 0
  %12 = getelementptr inbounds [10 x i8], [10 x i8]* %11, i32 0, i32 0
  store i8* %12, i8** %4, align 8
  %13 = load %struct.person_t*, %struct.person_t** %2, align 8
  %14 = getelementptr inbounds %struct.person_t, %struct.person_t* %13, i32 0, i32 0
  store i32 10, i32* %14, align 8
  %15 = load i8*, i8** %3, align 8
  %16 = load i8*, i8** %4, align 8
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str, i32 0, i32 0), i8* %15, i8* %16)
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.clothes, align 4
  %3 = alloca %struct.person_t, align 8
  %4 = alloca %struct.person_t*, align 8
  store i32 0, i32* %1, align 4
  %5 = bitcast %struct.clothes* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* getelementptr inbounds (%struct.clothes, %struct.clothes* @main.c, i32 0, i32 0, i32 0), i64 16, i32 4, i1 false)
  %6 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 0
  store i32 10, i32* %6, align 8
  %7 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 1
  %8 = bitcast [10 x i8]* %7 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %8, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0), i64 10, i32 1, i1 false)
  %9 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 2
  store %struct.clothes* %2, %struct.clothes** %9, align 8
  store %struct.person_t* %3, %struct.person_t** %4, align 8
  %10 = load %struct.person_t*, %struct.person_t** %4, align 8
  call void @f(%struct.person_t* %10)
  ret i32 0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
