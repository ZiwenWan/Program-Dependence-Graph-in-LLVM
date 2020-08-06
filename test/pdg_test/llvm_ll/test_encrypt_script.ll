; ModuleID = 'test_encrypt_script.c'
source_filename = "test_encrypt_script.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.1 = private unnamed_addr constant [12 x i8] c", welcome!\0A\00", align 1
@key = common global i8* null, align 8, !dbg !0
@i = common global i32 0, align 4, !dbg !11
@ciphertext = common global i8* null, align 8, !dbg !9
@.str.2 = private unnamed_addr constant [17 x i8] c"Enter username: \00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"%19s\00", align 1
@.str.4 = private unnamed_addr constant [18 x i8] c"Enter plaintext: \00", align 1
@.str.5 = private unnamed_addr constant [7 x i8] c"%1023s\00", align 1
@.str.6 = private unnamed_addr constant [14 x i8] c"Cipher text: \00", align 1
@.str.7 = private unnamed_addr constant [4 x i8] c"%x \00", align 1
@.str.8 = private unnamed_addr constant [10 x i8] c"sensitive\00", section "llvm.metadata"
@.str.9 = private unnamed_addr constant [22 x i8] c"test_encrypt_script.c\00", section "llvm.metadata"
@llvm.global.annotations = appending global [1 x { i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i32 } { i8* bitcast (i8** @key to i8*), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.9, i32 0, i32 0), i32 5 }], section "llvm.metadata"

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @greeter(i8*) #0 !dbg !19 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  call void @llvm.dbg.declare(metadata i8** %2, metadata !22, metadata !23), !dbg !24
  %3 = load i8*, i8** %2, align 8, !dbg !25
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i8* %3), !dbg !26
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i32 0, i32 0)), !dbg !27
  ret void, !dbg !28
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @initkey(i32) #0 !dbg !29 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !33, metadata !23), !dbg !34
  %3 = load i32, i32* %2, align 4, !dbg !35
  %4 = sext i32 %3 to i64, !dbg !35
  %5 = call i8* @malloc(i64 %4) #4, !dbg !36
  store i8* %5, i8** @key, align 8, !dbg !37
  store i32 0, i32* @i, align 4, !dbg !38
  br label %6, !dbg !40

; <label>:6:                                      ; preds = %15, %1
  %7 = load i32, i32* @i, align 4, !dbg !41
  %8 = load i32, i32* %2, align 4, !dbg !43
  %9 = icmp ult i32 %7, %8, !dbg !44
  br i1 %9, label %10, label %18, !dbg !45

; <label>:10:                                     ; preds = %6
  %11 = load i8*, i8** @key, align 8, !dbg !46
  %12 = load i32, i32* @i, align 4, !dbg !47
  %13 = zext i32 %12 to i64, !dbg !46
  %14 = getelementptr inbounds i8, i8* %11, i64 %13, !dbg !46
  store i8 1, i8* %14, align 1, !dbg !48
  br label %15, !dbg !46

; <label>:15:                                     ; preds = %10
  %16 = load i32, i32* @i, align 4, !dbg !49
  %17 = add i32 %16, 1, !dbg !49
  store i32 %17, i32* @i, align 4, !dbg !49
  br label %6, !dbg !50, !llvm.loop !51

; <label>:18:                                     ; preds = %6
  ret void, !dbg !53
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @encrypt(i8*, i32) #0 !dbg !54 {
  %3 = alloca i8*, align 8
  %4 = alloca i32, align 4
  store i8* %0, i8** %3, align 8
  call void @llvm.dbg.declare(metadata i8** %3, metadata !57, metadata !23), !dbg !58
  store i32 %1, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !59, metadata !23), !dbg !60
  %5 = load i32, i32* %4, align 4, !dbg !61
  %6 = sext i32 %5 to i64, !dbg !61
  %7 = call i8* @malloc(i64 %6) #4, !dbg !62
  store i8* %7, i8** @ciphertext, align 8, !dbg !63
  store i32 0, i32* @i, align 4, !dbg !64
  br label %8, !dbg !66

; <label>:8:                                      ; preds = %31, %2
  %9 = load i32, i32* @i, align 4, !dbg !67
  %10 = load i32, i32* %4, align 4, !dbg !69
  %11 = icmp ult i32 %9, %10, !dbg !70
  br i1 %11, label %12, label %34, !dbg !71

; <label>:12:                                     ; preds = %8
  %13 = load i8*, i8** %3, align 8, !dbg !72
  %14 = load i32, i32* @i, align 4, !dbg !73
  %15 = zext i32 %14 to i64, !dbg !72
  %16 = getelementptr inbounds i8, i8* %13, i64 %15, !dbg !72
  %17 = load i8, i8* %16, align 1, !dbg !72
  %18 = sext i8 %17 to i32, !dbg !72
  %19 = load i8*, i8** @key, align 8, !dbg !74
  %20 = load i32, i32* @i, align 4, !dbg !75
  %21 = zext i32 %20 to i64, !dbg !74
  %22 = getelementptr inbounds i8, i8* %19, i64 %21, !dbg !74
  %23 = load i8, i8* %22, align 1, !dbg !74
  %24 = sext i8 %23 to i32, !dbg !74
  %25 = xor i32 %18, %24, !dbg !76
  %26 = trunc i32 %25 to i8, !dbg !72
  %27 = load i8*, i8** @ciphertext, align 8, !dbg !77
  %28 = load i32, i32* @i, align 4, !dbg !78
  %29 = zext i32 %28 to i64, !dbg !77
  %30 = getelementptr inbounds i8, i8* %27, i64 %29, !dbg !77
  store i8 %26, i8* %30, align 1, !dbg !79
  br label %31, !dbg !77

; <label>:31:                                     ; preds = %12
  %32 = load i32, i32* @i, align 4, !dbg !80
  %33 = add i32 %32, 1, !dbg !80
  store i32 %33, i32* @i, align 4, !dbg !80
  br label %8, !dbg !81, !llvm.loop !82

; <label>:34:                                     ; preds = %8
  ret void, !dbg !84
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !85 {
  %1 = alloca i32, align 4
  %2 = alloca [20 x i8], align 16
  %3 = alloca [1024 x i8], align 16
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata [20 x i8]* %2, metadata !88, metadata !23), !dbg !92
  call void @llvm.dbg.declare(metadata [1024 x i8]* %3, metadata !93, metadata !23), !dbg !97
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.2, i32 0, i32 0)), !dbg !98
  %5 = getelementptr inbounds [20 x i8], [20 x i8]* %2, i32 0, i32 0, !dbg !99
  %6 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i32 0, i32 0), i8* %5), !dbg !100
  %7 = getelementptr inbounds [20 x i8], [20 x i8]* %2, i32 0, i32 0, !dbg !101
  call void @greeter(i8* %7), !dbg !102
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.4, i32 0, i32 0)), !dbg !103
  %9 = getelementptr inbounds [1024 x i8], [1024 x i8]* %3, i32 0, i32 0, !dbg !104
  %10 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.5, i32 0, i32 0), i8* %9), !dbg !105
  %11 = getelementptr inbounds [1024 x i8], [1024 x i8]* %3, i32 0, i32 0, !dbg !106
  %12 = call i64 @strlen(i8* %11), !dbg !107
  %13 = trunc i64 %12 to i32, !dbg !107
  call void @initkey(i32 %13), !dbg !108
  %14 = getelementptr inbounds [1024 x i8], [1024 x i8]* %3, i32 0, i32 0, !dbg !109
  %15 = getelementptr inbounds [1024 x i8], [1024 x i8]* %3, i32 0, i32 0, !dbg !110
  %16 = call i64 @strlen(i8* %15), !dbg !111
  %17 = trunc i64 %16 to i32, !dbg !111
  call void @encrypt(i8* %14, i32 %17), !dbg !112
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.6, i32 0, i32 0)), !dbg !113
  store i32 0, i32* @i, align 4, !dbg !114
  br label %19, !dbg !116

; <label>:19:                                     ; preds = %33, %0
  %20 = load i32, i32* @i, align 4, !dbg !117
  %21 = zext i32 %20 to i64, !dbg !117
  %22 = getelementptr inbounds [1024 x i8], [1024 x i8]* %3, i32 0, i32 0, !dbg !119
  %23 = call i64 @strlen(i8* %22), !dbg !120
  %24 = icmp ult i64 %21, %23, !dbg !121
  br i1 %24, label %25, label %36, !dbg !122

; <label>:25:                                     ; preds = %19
  %26 = load i8*, i8** @ciphertext, align 8, !dbg !123
  %27 = load i32, i32* @i, align 4, !dbg !124
  %28 = zext i32 %27 to i64, !dbg !123
  %29 = getelementptr inbounds i8, i8* %26, i64 %28, !dbg !123
  %30 = load i8, i8* %29, align 1, !dbg !123
  %31 = sext i8 %30 to i32, !dbg !123
  %32 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.7, i32 0, i32 0), i32 %31), !dbg !125
  br label %33, !dbg !125

; <label>:33:                                     ; preds = %25
  %34 = load i32, i32* @i, align 4, !dbg !126
  %35 = add i32 %34, 1, !dbg !126
  store i32 %35, i32* @i, align 4, !dbg !126
  br label %19, !dbg !127, !llvm.loop !128

; <label>:36:                                     ; preds = %19
  ret i32 0, !dbg !130
}

declare i32 @scanf(i8*, ...) #2

declare i64 @strlen(i8*) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { allocsize(0) }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!14, !15, !16, !17}
!llvm.ident = !{!18}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "key", scope: !2, file: !3, line: 5, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !8)
!3 = !DIFile(filename: "test_encrypt_script.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!4 = !{}
!5 = !{!6}
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!8 = !{!0, !9, !11}
!9 = !DIGlobalVariableExpression(var: !10)
!10 = distinct !DIGlobalVariable(name: "ciphertext", scope: !2, file: !3, line: 6, type: !6, isLocal: false, isDefinition: true)
!11 = !DIGlobalVariableExpression(var: !12)
!12 = distinct !DIGlobalVariable(name: "i", scope: !2, file: !3, line: 7, type: !13, isLocal: false, isDefinition: true)
!13 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!14 = !{i32 2, !"Dwarf Version", i32 4}
!15 = !{i32 2, !"Debug Info Version", i32 3}
!16 = !{i32 1, !"wchar_size", i32 4}
!17 = !{i32 7, !"PIC Level", i32 2}
!18 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!19 = distinct !DISubprogram(name: "greeter", scope: !3, file: !3, line: 9, type: !20, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!20 = !DISubroutineType(types: !21)
!21 = !{null, !6}
!22 = !DILocalVariable(name: "str", arg: 1, scope: !19, file: !3, line: 9, type: !6)
!23 = !DIExpression()
!24 = !DILocation(line: 9, column: 21, scope: !19)
!25 = !DILocation(line: 10, column: 16, scope: !19)
!26 = !DILocation(line: 10, column: 2, scope: !19)
!27 = !DILocation(line: 11, column: 2, scope: !19)
!28 = !DILocation(line: 12, column: 1, scope: !19)
!29 = distinct !DISubprogram(name: "initkey", scope: !3, file: !3, line: 14, type: !30, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!30 = !DISubroutineType(types: !31)
!31 = !{null, !32}
!32 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!33 = !DILocalVariable(name: "sz", arg: 1, scope: !29, file: !3, line: 14, type: !32)
!34 = !DILocation(line: 14, column: 19, scope: !29)
!35 = !DILocation(line: 15, column: 26, scope: !29)
!36 = !DILocation(line: 15, column: 18, scope: !29)
!37 = !DILocation(line: 15, column: 6, scope: !29)
!38 = !DILocation(line: 17, column: 8, scope: !39)
!39 = distinct !DILexicalBlock(scope: !29, file: !3, line: 17, column: 2)
!40 = !DILocation(line: 17, column: 7, scope: !39)
!41 = !DILocation(line: 17, column: 12, scope: !42)
!42 = distinct !DILexicalBlock(scope: !39, file: !3, line: 17, column: 2)
!43 = !DILocation(line: 17, column: 14, scope: !42)
!44 = !DILocation(line: 17, column: 13, scope: !42)
!45 = !DILocation(line: 17, column: 2, scope: !39)
!46 = !DILocation(line: 17, column: 23, scope: !42)
!47 = !DILocation(line: 17, column: 27, scope: !42)
!48 = !DILocation(line: 17, column: 29, scope: !42)
!49 = !DILocation(line: 17, column: 19, scope: !42)
!50 = !DILocation(line: 17, column: 2, scope: !42)
!51 = distinct !{!51, !45, !52}
!52 = !DILocation(line: 17, column: 31, scope: !39)
!53 = !DILocation(line: 18, column: 1, scope: !29)
!54 = distinct !DISubprogram(name: "encrypt", scope: !3, file: !3, line: 20, type: !55, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!55 = !DISubroutineType(types: !56)
!56 = !{null, !6, !32}
!57 = !DILocalVariable(name: "plaintext", arg: 1, scope: !54, file: !3, line: 20, type: !6)
!58 = !DILocation(line: 20, column: 21, scope: !54)
!59 = !DILocalVariable(name: "sz", arg: 2, scope: !54, file: !3, line: 20, type: !32)
!60 = !DILocation(line: 20, column: 36, scope: !54)
!61 = !DILocation(line: 21, column: 33, scope: !54)
!62 = !DILocation(line: 21, column: 25, scope: !54)
!63 = !DILocation(line: 21, column: 13, scope: !54)
!64 = !DILocation(line: 22, column: 8, scope: !65)
!65 = distinct !DILexicalBlock(scope: !54, file: !3, line: 22, column: 2)
!66 = !DILocation(line: 22, column: 7, scope: !65)
!67 = !DILocation(line: 22, column: 12, scope: !68)
!68 = distinct !DILexicalBlock(scope: !65, file: !3, line: 22, column: 2)
!69 = !DILocation(line: 22, column: 14, scope: !68)
!70 = !DILocation(line: 22, column: 13, scope: !68)
!71 = !DILocation(line: 22, column: 2, scope: !65)
!72 = !DILocation(line: 23, column: 17, scope: !68)
!73 = !DILocation(line: 23, column: 27, scope: !68)
!74 = !DILocation(line: 23, column: 32, scope: !68)
!75 = !DILocation(line: 23, column: 36, scope: !68)
!76 = !DILocation(line: 23, column: 30, scope: !68)
!77 = !DILocation(line: 23, column: 3, scope: !68)
!78 = !DILocation(line: 23, column: 14, scope: !68)
!79 = !DILocation(line: 23, column: 16, scope: !68)
!80 = !DILocation(line: 22, column: 19, scope: !68)
!81 = !DILocation(line: 22, column: 2, scope: !68)
!82 = distinct !{!82, !71, !83}
!83 = !DILocation(line: 23, column: 37, scope: !65)
!84 = !DILocation(line: 24, column: 1, scope: !54)
!85 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 26, type: !86, isLocal: false, isDefinition: true, scopeLine: 26, isOptimized: false, unit: !2, variables: !4)
!86 = !DISubroutineType(types: !87)
!87 = !{!32}
!88 = !DILocalVariable(name: "username", scope: !85, file: !3, line: 27, type: !89)
!89 = !DICompositeType(tag: DW_TAG_array_type, baseType: !7, size: 160, elements: !90)
!90 = !{!91}
!91 = !DISubrange(count: 20)
!92 = !DILocation(line: 27, column: 7, scope: !85)
!93 = !DILocalVariable(name: "text", scope: !85, file: !3, line: 27, type: !94)
!94 = !DICompositeType(tag: DW_TAG_array_type, baseType: !7, size: 8192, elements: !95)
!95 = !{!96}
!96 = !DISubrange(count: 1024)
!97 = !DILocation(line: 27, column: 21, scope: !85)
!98 = !DILocation(line: 28, column: 2, scope: !85)
!99 = !DILocation(line: 29, column: 15, scope: !85)
!100 = !DILocation(line: 29, column: 2, scope: !85)
!101 = !DILocation(line: 30, column: 10, scope: !85)
!102 = !DILocation(line: 30, column: 2, scope: !85)
!103 = !DILocation(line: 31, column: 2, scope: !85)
!104 = !DILocation(line: 32, column: 17, scope: !85)
!105 = !DILocation(line: 32, column: 2, scope: !85)
!106 = !DILocation(line: 34, column: 17, scope: !85)
!107 = !DILocation(line: 34, column: 10, scope: !85)
!108 = !DILocation(line: 34, column: 2, scope: !85)
!109 = !DILocation(line: 35, column: 10, scope: !85)
!110 = !DILocation(line: 35, column: 23, scope: !85)
!111 = !DILocation(line: 35, column: 16, scope: !85)
!112 = !DILocation(line: 35, column: 2, scope: !85)
!113 = !DILocation(line: 36, column: 2, scope: !85)
!114 = !DILocation(line: 37, column: 8, scope: !115)
!115 = distinct !DILexicalBlock(scope: !85, file: !3, line: 37, column: 2)
!116 = !DILocation(line: 37, column: 7, scope: !115)
!117 = !DILocation(line: 37, column: 12, scope: !118)
!118 = distinct !DILexicalBlock(scope: !115, file: !3, line: 37, column: 2)
!119 = !DILocation(line: 37, column: 21, scope: !118)
!120 = !DILocation(line: 37, column: 14, scope: !118)
!121 = !DILocation(line: 37, column: 13, scope: !118)
!122 = !DILocation(line: 37, column: 2, scope: !115)
!123 = !DILocation(line: 38, column: 16, scope: !118)
!124 = !DILocation(line: 38, column: 27, scope: !118)
!125 = !DILocation(line: 38, column: 3, scope: !118)
!126 = !DILocation(line: 37, column: 29, scope: !118)
!127 = !DILocation(line: 37, column: 2, scope: !118)
!128 = distinct !{!128, !122, !129}
!129 = !DILocation(line: 38, column: 29, scope: !115)
!130 = !DILocation(line: 39, column: 5, scope: !85)
