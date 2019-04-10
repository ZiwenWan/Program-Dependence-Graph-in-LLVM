; ModuleID = 'test_loading_inst.c'
source_filename = "test_loading_inst.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.clothes = type { i32, i8* }
%struct.Person = type { i32, %struct.clothes* }

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"red\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { i32 10, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0) }, align 8
@main.b = private unnamed_addr constant [3 x i32] [i32 1, i32 2, i32 3], align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  %3 = load %struct.Person*, %struct.Person** %2, align 8
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %3, i32 0, i32 1
  %5 = load %struct.clothes*, %struct.clothes** %4, align 8
  %6 = getelementptr inbounds %struct.clothes, %struct.clothes* %5, i32 0, i32 1
  %7 = load i8*, i8** %6, align 8
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i8* %7)
  %9 = load %struct.Person*, %struct.Person** %2, align 8
  %10 = getelementptr inbounds %struct.Person, %struct.Person* %9, i32 0, i32 0
  %11 = load i32, i32* %10, align 8
  ret i32 %11
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @ff(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @fff(i8*, i32*, i32*) #0 {
  %4 = alloca i8*, align 8
  %5 = alloca i32*, align 8
  %6 = alloca i32*, align 8
  store i8* %0, i8** %4, align 8
  store i32* %1, i32** %5, align 8
  store i32* %2, i32** %6, align 8
  %7 = load i8*, i8** %4, align 8
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i8* %7)
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca %struct.clothes, align 8
  %2 = alloca %struct.Person, align 8
  %3 = alloca i32, align 4
  %4 = alloca [3 x i32], align 4
  %5 = bitcast %struct.clothes* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* bitcast (%struct.clothes* @main.c to i8*), i64 16, i32 8, i1 false)
  %6 = getelementptr inbounds %struct.Person, %struct.Person* %2, i32 0, i32 0
  store i32 21, i32* %6, align 8
  %7 = getelementptr inbounds %struct.Person, %struct.Person* %2, i32 0, i32 1
  store %struct.clothes* %1, %struct.clothes** %7, align 8
  %8 = call i32 @f(%struct.Person* %2)
  %9 = call i32 @ff(i32 5)
  store i32 5, i32* %3, align 4
  %10 = bitcast [3 x i32]* %4 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %10, i8* bitcast ([3 x i32]* @main.b to i8*), i64 12, i32 4, i1 false)
  %11 = getelementptr inbounds %struct.clothes, %struct.clothes* %1, i32 0, i32 1
  %12 = load i8*, i8** %11, align 8
  %13 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i32 0, i32 0
  call void @fff(i8* %12, i32* %3, i32* %13)
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
