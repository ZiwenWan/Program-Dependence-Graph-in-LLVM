; ModuleID = 'func_partition.c'
source_filename = "func_partition.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }
%struct.passwd = type { i8*, i8*, i8* }

@.str = private unnamed_addr constant [7 x i8] c"passwd\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"r\00", align 1
@__stderrp = external global %struct.__sFILE*, align 8
@.str.2 = private unnamed_addr constant [24 x i8] c"Unable to open passwd!\0A\00", align 1
@.str.3 = private unnamed_addr constant [16 x i8] c"Out of memory!\0A\00", align 1
@.str.4 = private unnamed_addr constant [23 x i8] c"Error reading passwd!\0A\00", align 1
@.str.5 = private unnamed_addr constant [3 x i8] c" \09\00", align 1
@.str.6 = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str.7 = private unnamed_addr constant [17 x i8] c"func_partition.c\00", section "llvm.metadata"
@.str.8 = private unnamed_addr constant [30 x i8] c"Usage: %s uid home_dir shell\0A\00", align 1
@.str.9 = private unnamed_addr constant [8 x i8] c"Error!\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.passwd* @pw_locate(i8*) #0 {
  %2 = alloca %struct.passwd*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca %struct.__sFILE*, align 8
  %5 = alloca [256 x i8], align 16
  %6 = alloca %struct.passwd*, align 8
  %7 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  %8 = call %struct.__sFILE* @"\01_fopen"(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i32 0, i32 0))
  store %struct.__sFILE* %8, %struct.__sFILE** %4, align 8
  %9 = load %struct.__sFILE*, %struct.__sFILE** %4, align 8
  %10 = icmp eq %struct.__sFILE* %9, null
  br i1 %10, label %11, label %14

; <label>:11:                                     ; preds = %1
  %12 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %13 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %12, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.2, i32 0, i32 0))
  store %struct.passwd* null, %struct.passwd** %2, align 8
  br label %80

; <label>:14:                                     ; preds = %1
  %15 = call i8* @malloc(i64 24) #6
  %16 = bitcast i8* %15 to %struct.passwd*
  store %struct.passwd* %16, %struct.passwd** %6, align 8
  %17 = load %struct.passwd*, %struct.passwd** %6, align 8
  %18 = icmp eq %struct.passwd* %17, null
  br i1 %18, label %19, label %22

; <label>:19:                                     ; preds = %14
  %20 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %21 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %20, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.3, i32 0, i32 0))
  store %struct.passwd* null, %struct.passwd** %2, align 8
  br label %80

; <label>:22:                                     ; preds = %14
  br label %23

; <label>:23:                                     ; preds = %76, %46, %22
  %24 = load %struct.__sFILE*, %struct.__sFILE** %4, align 8
  %25 = call i32 @feof(%struct.__sFILE* %24)
  %26 = icmp ne i32 %25, 0
  %27 = xor i1 %26, true
  br i1 %27, label %28, label %77

; <label>:28:                                     ; preds = %23
  %29 = getelementptr inbounds [256 x i8], [256 x i8]* %5, i32 0, i32 0
  %30 = load %struct.__sFILE*, %struct.__sFILE** %4, align 8
  %31 = call i8* @fgets(i8* %29, i32 256, %struct.__sFILE* %30)
  %32 = icmp eq i8* %31, null
  br i1 %32, label %33, label %36

; <label>:33:                                     ; preds = %28
  %34 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %35 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %34, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.4, i32 0, i32 0))
  br label %77

; <label>:36:                                     ; preds = %28
  %37 = getelementptr inbounds [256 x i8], [256 x i8]* %5, i32 0, i32 0
  %38 = call i8* @strtok(i8* %37, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0))
  store i8* %38, i8** %7, align 8
  %39 = load i8*, i8** %7, align 8
  %40 = icmp ne i8* %39, null
  br i1 %40, label %41, label %46

; <label>:41:                                     ; preds = %36
  %42 = load i8*, i8** %7, align 8
  %43 = call i8* @strdup(i8* %42)
  %44 = load %struct.passwd*, %struct.passwd** %6, align 8
  %45 = getelementptr inbounds %struct.passwd, %struct.passwd* %44, i32 0, i32 0
  store i8* %43, i8** %45, align 8
  br label %47

; <label>:46:                                     ; preds = %36
  br label %23

; <label>:47:                                     ; preds = %41
  %48 = load %struct.passwd*, %struct.passwd** %6, align 8
  %49 = getelementptr inbounds %struct.passwd, %struct.passwd* %48, i32 0, i32 0
  %50 = load i8*, i8** %49, align 8
  %51 = load i8*, i8** %3, align 8
  %52 = call i32 @strcmp(i8* %50, i8* %51)
  %53 = icmp eq i32 %52, 0
  br i1 %53, label %54, label %76

; <label>:54:                                     ; preds = %47
  %55 = call i8* @strtok(i8* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0))
  store i8* %55, i8** %7, align 8
  %56 = load i8*, i8** %7, align 8
  %57 = icmp ne i8* %56, null
  br i1 %57, label %58, label %63

; <label>:58:                                     ; preds = %54
  %59 = load i8*, i8** %7, align 8
  %60 = call i8* @strdup(i8* %59)
  %61 = load %struct.passwd*, %struct.passwd** %6, align 8
  %62 = getelementptr inbounds %struct.passwd, %struct.passwd* %61, i32 0, i32 1
  store i8* %60, i8** %62, align 8
  br label %64

; <label>:63:                                     ; preds = %54
  br label %77

; <label>:64:                                     ; preds = %58
  %65 = call i8* @strtok(i8* null, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i32 0, i32 0))
  store i8* %65, i8** %7, align 8
  %66 = load i8*, i8** %7, align 8
  %67 = icmp ne i8* %66, null
  br i1 %67, label %68, label %73

; <label>:68:                                     ; preds = %64
  %69 = load i8*, i8** %7, align 8
  %70 = call i8* @strdup(i8* %69)
  %71 = load %struct.passwd*, %struct.passwd** %6, align 8
  %72 = getelementptr inbounds %struct.passwd, %struct.passwd* %71, i32 0, i32 2
  store i8* %70, i8** %72, align 8
  br label %74

; <label>:73:                                     ; preds = %64
  br label %77

; <label>:74:                                     ; preds = %68
  %75 = load %struct.passwd*, %struct.passwd** %6, align 8
  store %struct.passwd* %75, %struct.passwd** %2, align 8
  br label %80

; <label>:76:                                     ; preds = %47
  br label %23

; <label>:77:                                     ; preds = %73, %63, %33, %23
  %78 = load %struct.passwd*, %struct.passwd** %6, align 8
  %79 = bitcast %struct.passwd* %78 to i8*
  call void @free(i8* %79)
  store %struct.passwd* null, %struct.passwd** %2, align 8
  br label %80

; <label>:80:                                     ; preds = %77, %74, %19, %11
  %81 = load %struct.passwd*, %struct.passwd** %2, align 8
  ret %struct.passwd* %81
}

declare %struct.__sFILE* @"\01_fopen"(i8*, i8*) #1

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #1

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

declare i32 @feof(%struct.__sFILE*) #1

declare i8* @fgets(i8*, i32, %struct.__sFILE*) #1

declare i8* @strtok(i8*, i8*) #1

declare i8* @strdup(i8*) #1

declare i32 @strcmp(i8*, i8*) #1

declare void @free(i8*) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @pw_update(%struct.passwd*) #0 {
  %2 = alloca %struct.passwd*, align 8
  store %struct.passwd* %0, %struct.passwd** %2, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main(i32, i8**) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca %struct.passwd*, align 8
  %7 = alloca %struct.passwd, align 8
  %8 = alloca [3 x i8*], align 16
  %9 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %10 = bitcast %struct.passwd** %6 to i8*
  call void @llvm.var.annotation(i8* %10, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.6, i32 0, i32 0), i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.7, i32 0, i32 0), i32 55)
  %11 = load i32, i32* %4, align 4
  %12 = icmp ne i32 %11, 4
  br i1 %12, label %13, label %18

; <label>:13:                                     ; preds = %2
  %14 = load i8**, i8*** %5, align 8
  %15 = getelementptr inbounds i8*, i8** %14, i64 0
  %16 = load i8*, i8** %15, align 8
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.8, i32 0, i32 0), i8* %16)
  call void @exit(i32 0) #7
  unreachable

; <label>:18:                                     ; preds = %2
  store i32 1, i32* %9, align 4
  br label %19

; <label>:19:                                     ; preds = %34, %18
  %20 = load i32, i32* %9, align 4
  %21 = load i32, i32* %4, align 4
  %22 = icmp slt i32 %20, %21
  br i1 %22, label %23, label %37

; <label>:23:                                     ; preds = %19
  %24 = load i8**, i8*** %5, align 8
  %25 = load i32, i32* %9, align 4
  %26 = sext i32 %25 to i64
  %27 = getelementptr inbounds i8*, i8** %24, i64 %26
  %28 = load i8*, i8** %27, align 8
  %29 = call i8* @strdup(i8* %28)
  %30 = load i32, i32* %9, align 4
  %31 = sub nsw i32 %30, 1
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [3 x i8*], [3 x i8*]* %8, i64 0, i64 %32
  store i8* %29, i8** %33, align 8
  br label %34

; <label>:34:                                     ; preds = %23
  %35 = load i32, i32* %9, align 4
  %36 = add nsw i32 %35, 1
  store i32 %36, i32* %9, align 4
  br label %19

; <label>:37:                                     ; preds = %19
  %38 = getelementptr inbounds [3 x i8*], [3 x i8*]* %8, i64 0, i64 0
  %39 = load i8*, i8** %38, align 16
  %40 = call %struct.passwd* @pw_locate(i8* %39)
  store %struct.passwd* %40, %struct.passwd** %6, align 8
  %41 = load %struct.passwd*, %struct.passwd** %6, align 8
  %42 = icmp eq %struct.passwd* %41, null
  br i1 %42, label %43, label %46

; <label>:43:                                     ; preds = %37
  %44 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %45 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %44, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.9, i32 0, i32 0))
  store i32 0, i32* %3, align 4
  br label %56

; <label>:46:                                     ; preds = %37
  %47 = load %struct.passwd*, %struct.passwd** %6, align 8
  %48 = bitcast %struct.passwd* %7 to i8*
  %49 = bitcast %struct.passwd* %47 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %48, i8* %49, i64 24, i32 8, i1 false)
  %50 = getelementptr inbounds [3 x i8*], [3 x i8*]* %8, i64 0, i64 1
  %51 = load i8*, i8** %50, align 8
  %52 = getelementptr inbounds %struct.passwd, %struct.passwd* %7, i32 0, i32 1
  store i8* %51, i8** %52, align 8
  %53 = getelementptr inbounds [3 x i8*], [3 x i8*]* %8, i64 0, i64 2
  %54 = load i8*, i8** %53, align 16
  %55 = getelementptr inbounds %struct.passwd, %struct.passwd* %7, i32 0, i32 2
  store i8* %54, i8** %55, align 8
  call void @pw_update(%struct.passwd* %7)
  store i32 0, i32* %3, align 4
  br label %56

; <label>:56:                                     ; preds = %46, %43
  %57 = load i32, i32* %3, align 4
  ret i32 %57
}

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #3

declare i32 @printf(i8*, ...) #1

; Function Attrs: noreturn
declare void @exit(i32) #4

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #5

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { argmemonly nounwind }
attributes #6 = { allocsize(0) }
attributes #7 = { noreturn }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
