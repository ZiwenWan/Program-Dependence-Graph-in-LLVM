; ModuleID = 'test_arg_use.c'
source_filename = "test_arg_use.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

%struct.clothes = type { [10 x i8], float }
%struct.person = type { i32, [10 x i8], %struct.clothes* }

@.str = private unnamed_addr constant [23 x i8] c"Age is %d, name is %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [19 x i8] c"Changed age is %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [15 x i8] c"Now age is %d\0A\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { [10 x i8] c"red\00\00\00\00\00\00\00", float 0x400B333340000000 }, align 4
@.str.3 = private unnamed_addr constant [10 x i8] c"Jack\00\00\00\00\00\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @getAge(%struct.person*) #0 !dbg !8 {
  %2 = alloca %struct.person*, align 8
  store %struct.person* %0, %struct.person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %2, metadata !30, metadata !DIExpression()), !dbg !31
  %3 = load %struct.person*, %struct.person** %2, align 8, !dbg !32
  %4 = getelementptr inbounds %struct.person, %struct.person* %3, i32 0, i32 0, !dbg !33
  %5 = load i32, i32* %4, align 8, !dbg !33
  ret i32 %5, !dbg !34
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @changeAge(%struct.person*) #0 !dbg !35 {
  %2 = alloca %struct.person*, align 8
  store %struct.person* %0, %struct.person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %2, metadata !38, metadata !DIExpression()), !dbg !39
  %3 = load %struct.person*, %struct.person** %2, align 8, !dbg !40
  %4 = getelementptr inbounds %struct.person, %struct.person* %3, i32 0, i32 0, !dbg !41
  store i32 50, i32* %4, align 8, !dbg !42
  ret void, !dbg !43
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @printAge(%struct.person*) #0 !dbg !44 {
  %2 = alloca %struct.person*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i8*, align 8
  store %struct.person* %0, %struct.person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %2, metadata !45, metadata !DIExpression()), !dbg !46
  call void @llvm.dbg.declare(metadata i32* %3, metadata !47, metadata !DIExpression()), !dbg !48
  %5 = load %struct.person*, %struct.person** %2, align 8, !dbg !49
  %6 = call i32 @getAge(%struct.person* %5), !dbg !50
  store i32 %6, i32* %3, align 4, !dbg !48
  call void @llvm.dbg.declare(metadata i8** %4, metadata !51, metadata !DIExpression()), !dbg !53
  %7 = load %struct.person*, %struct.person** %2, align 8, !dbg !54
  %8 = getelementptr inbounds %struct.person, %struct.person* %7, i32 0, i32 1, !dbg !55
  %9 = getelementptr inbounds [10 x i8], [10 x i8]* %8, i32 0, i32 0, !dbg !54
  store i8* %9, i8** %4, align 8, !dbg !53
  %10 = load i32, i32* %3, align 4, !dbg !56
  %11 = load i8*, i8** %4, align 8, !dbg !57
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i32 0, i32 0), i32 %10, i8* %11), !dbg !58
  %13 = load %struct.person*, %struct.person** %2, align 8, !dbg !59
  call void @changeAge(%struct.person* %13), !dbg !60
  %14 = load %struct.person*, %struct.person** %2, align 8, !dbg !61
  %15 = getelementptr inbounds %struct.person, %struct.person* %14, i32 0, i32 0, !dbg !62
  %16 = load i32, i32* %15, align 8, !dbg !62
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.1, i32 0, i32 0), i32 %16), !dbg !63
  ret void, !dbg !64
}

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @changeAgePtr(i32*) #0 !dbg !65 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !69, metadata !DIExpression()), !dbg !70
  %3 = load i32*, i32** %2, align 8, !dbg !71
  store i32 100, i32* %3, align 4, !dbg !72
  ret void, !dbg !73
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @agePtr(%struct.person*) #0 !dbg !74 {
  %2 = alloca %struct.person*, align 8
  %3 = alloca i32*, align 8
  store %struct.person* %0, %struct.person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %2, metadata !75, metadata !DIExpression()), !dbg !76
  call void @llvm.dbg.declare(metadata i32** %3, metadata !77, metadata !DIExpression()), !dbg !78
  %4 = load %struct.person*, %struct.person** %2, align 8, !dbg !79
  %5 = getelementptr inbounds %struct.person, %struct.person* %4, i32 0, i32 0, !dbg !80
  store i32* %5, i32** %3, align 8, !dbg !78
  %6 = load i32*, i32** %3, align 8, !dbg !81
  call void @changeAgePtr(i32* %6), !dbg !82
  %7 = load i32*, i32** %3, align 8, !dbg !83
  %8 = load i32, i32* %7, align 4, !dbg !84
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.2, i32 0, i32 0), i32 %8), !dbg !85
  ret void, !dbg !86
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !87 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.clothes, align 4
  %3 = alloca %struct.person, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.clothes* %2, metadata !90, metadata !DIExpression()), !dbg !91
  %4 = bitcast %struct.clothes* %2 to i8*, !dbg !91
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %4, i8* getelementptr inbounds (%struct.clothes, %struct.clothes* @main.c, i32 0, i32 0, i32 0), i64 16, i32 4, i1 false), !dbg !91
  call void @llvm.dbg.declare(metadata %struct.person* %3, metadata !92, metadata !DIExpression()), !dbg !93
  %5 = getelementptr inbounds %struct.person, %struct.person* %3, i32 0, i32 0, !dbg !94
  store i32 20, i32* %5, align 8, !dbg !94
  %6 = getelementptr inbounds %struct.person, %struct.person* %3, i32 0, i32 1, !dbg !94
  %7 = bitcast [10 x i8]* %6 to i8*, !dbg !95
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.3, i32 0, i32 0), i64 10, i32 1, i1 false), !dbg !95
  %8 = getelementptr inbounds %struct.person, %struct.person* %3, i32 0, i32 2, !dbg !94
  store %struct.clothes* %2, %struct.clothes** %8, align 8, !dbg !94
  call void @printAge(%struct.person* %3), !dbg !96
  ret i32 0, !dbg !97
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #3

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Apple LLVM version 9.1.0 (clang-902.0.39.2)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_arg_use.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"Apple LLVM version 9.1.0 (clang-902.0.39.2)"}
!8 = distinct !DISubprogram(name: "getAge", scope: !1, file: !1, line: 16, type: !9, isLocal: false, isDefinition: true, scopeLine: 16, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 13, baseType: !14)
!14 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !1, line: 9, size: 192, elements: !15)
!15 = !{!16, !17, !22}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !14, file: !1, line: 10, baseType: !11, size: 32)
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !14, file: !1, line: 11, baseType: !18, size: 80, offset: 32)
!18 = !DICompositeType(tag: DW_TAG_array_type, baseType: !19, size: 80, elements: !20)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !{!21}
!21 = !DISubrange(count: 10)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "clothes", scope: !14, file: !1, line: 12, baseType: !23, size: 64, offset: 128)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "Clothes", file: !1, line: 7, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "clothes", file: !1, line: 4, size: 128, elements: !26)
!26 = !{!27, !28}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "color", scope: !25, file: !1, line: 5, baseType: !18, size: 80)
!28 = !DIDerivedType(tag: DW_TAG_member, name: "height", scope: !25, file: !1, line: 6, baseType: !29, size: 32, offset: 96)
!29 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!30 = !DILocalVariable(name: "p", arg: 1, scope: !8, file: !1, line: 16, type: !12)
!31 = !DILocation(line: 16, column: 20, scope: !8)
!32 = !DILocation(line: 17, column: 12, scope: !8)
!33 = !DILocation(line: 17, column: 15, scope: !8)
!34 = !DILocation(line: 17, column: 5, scope: !8)
!35 = distinct !DISubprogram(name: "changeAge", scope: !1, file: !1, line: 20, type: !36, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!36 = !DISubroutineType(types: !37)
!37 = !{null, !12}
!38 = !DILocalVariable(name: "p", arg: 1, scope: !35, file: !1, line: 20, type: !12)
!39 = !DILocation(line: 20, column: 24, scope: !35)
!40 = !DILocation(line: 21, column: 5, scope: !35)
!41 = !DILocation(line: 21, column: 8, scope: !35)
!42 = !DILocation(line: 21, column: 12, scope: !35)
!43 = !DILocation(line: 22, column: 1, scope: !35)
!44 = distinct !DISubprogram(name: "printAge", scope: !1, file: !1, line: 24, type: !36, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!45 = !DILocalVariable(name: "p", arg: 1, scope: !44, file: !1, line: 24, type: !12)
!46 = !DILocation(line: 24, column: 23, scope: !44)
!47 = !DILocalVariable(name: "age", scope: !44, file: !1, line: 25, type: !11)
!48 = !DILocation(line: 25, column: 9, scope: !44)
!49 = !DILocation(line: 25, column: 22, scope: !44)
!50 = !DILocation(line: 25, column: 15, scope: !44)
!51 = !DILocalVariable(name: "name", scope: !44, file: !1, line: 26, type: !52)
!52 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!53 = !DILocation(line: 26, column: 11, scope: !44)
!54 = !DILocation(line: 26, column: 18, scope: !44)
!55 = !DILocation(line: 26, column: 21, scope: !44)
!56 = !DILocation(line: 27, column: 39, scope: !44)
!57 = !DILocation(line: 27, column: 44, scope: !44)
!58 = !DILocation(line: 27, column: 5, scope: !44)
!59 = !DILocation(line: 28, column: 15, scope: !44)
!60 = !DILocation(line: 28, column: 5, scope: !44)
!61 = !DILocation(line: 29, column: 35, scope: !44)
!62 = !DILocation(line: 29, column: 38, scope: !44)
!63 = !DILocation(line: 29, column: 5, scope: !44)
!64 = !DILocation(line: 30, column: 1, scope: !44)
!65 = distinct !DISubprogram(name: "changeAgePtr", scope: !1, file: !1, line: 33, type: !66, isLocal: false, isDefinition: true, scopeLine: 33, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!66 = !DISubroutineType(types: !67)
!67 = !{null, !68}
!68 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!69 = !DILocalVariable(name: "age", arg: 1, scope: !65, file: !1, line: 33, type: !68)
!70 = !DILocation(line: 33, column: 24, scope: !65)
!71 = !DILocation(line: 34, column: 5, scope: !65)
!72 = !DILocation(line: 34, column: 9, scope: !65)
!73 = !DILocation(line: 35, column: 1, scope: !65)
!74 = distinct !DISubprogram(name: "agePtr", scope: !1, file: !1, line: 37, type: !36, isLocal: false, isDefinition: true, scopeLine: 37, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!75 = !DILocalVariable(name: "p", arg: 1, scope: !74, file: !1, line: 37, type: !12)
!76 = !DILocation(line: 37, column: 21, scope: !74)
!77 = !DILocalVariable(name: "age", scope: !74, file: !1, line: 38, type: !68)
!78 = !DILocation(line: 38, column: 10, scope: !74)
!79 = !DILocation(line: 38, column: 18, scope: !74)
!80 = !DILocation(line: 38, column: 21, scope: !74)
!81 = !DILocation(line: 39, column: 18, scope: !74)
!82 = !DILocation(line: 39, column: 5, scope: !74)
!83 = !DILocation(line: 40, column: 32, scope: !74)
!84 = !DILocation(line: 40, column: 31, scope: !74)
!85 = !DILocation(line: 40, column: 5, scope: !74)
!86 = !DILocation(line: 41, column: 1, scope: !74)
!87 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 44, type: !88, isLocal: false, isDefinition: true, scopeLine: 44, isOptimized: false, unit: !0, variables: !2)
!88 = !DISubroutineType(types: !89)
!89 = !{!11}
!90 = !DILocalVariable(name: "c", scope: !87, file: !1, line: 45, type: !24)
!91 = !DILocation(line: 45, column: 13, scope: !87)
!92 = !DILocalVariable(name: "p", scope: !87, file: !1, line: 46, type: !13)
!93 = !DILocation(line: 46, column: 12, scope: !87)
!94 = !DILocation(line: 46, column: 16, scope: !87)
!95 = !DILocation(line: 46, column: 22, scope: !87)
!96 = !DILocation(line: 48, column: 5, scope: !87)
!97 = !DILocation(line: 49, column: 5, scope: !87)
