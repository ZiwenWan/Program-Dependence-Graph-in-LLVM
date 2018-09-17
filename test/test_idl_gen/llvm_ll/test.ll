; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

%struct.Person = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"new_name\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 15, [10 x i8] c"Jack\00\00\00\00\00\00" }, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @func1(i8*) #0 !dbg !8 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  call void @llvm.dbg.declare(metadata i8** %2, metadata !13, metadata !14), !dbg !15
  %3 = load i8*, i8** %2, align 8, !dbg !16
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i8* %3), !dbg !17
  %5 = load i8*, i8** %2, align 8, !dbg !18
  %6 = load i8*, i8** %2, align 8, !dbg !18
  %7 = call i64 @llvm.objectsize.i64.p0i8(i8* %6, i1 false, i1 true), !dbg !18
  %8 = call i8* @__strncpy_chk(i8* %5, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i32 0, i32 0), i64 10, i64 %7) #5, !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: nounwind
declare i8* @__strncpy_chk(i8*, i8*, i64, i64) #3

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @func2(i32*) #0 !dbg !20 {
  %2 = alloca i32*, align 8
  %3 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !25, metadata !14), !dbg !26
  call void @llvm.dbg.declare(metadata i32** %3, metadata !27, metadata !14), !dbg !28
  %4 = load i32*, i32** %2, align 8, !dbg !29
  %5 = getelementptr inbounds i32, i32* %4, i64 1, !dbg !30
  store i32* %5, i32** %3, align 8, !dbg !28
  %6 = load i32*, i32** %2, align 8, !dbg !31
  %7 = load i32, i32* %6, align 4, !dbg !32
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i32 0, i32 0), i32 %7), !dbg !33
  %9 = load i32*, i32** %2, align 8, !dbg !34
  store i32 20, i32* %9, align 4, !dbg !35
  ret void, !dbg !36
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @func3(i32) #0 !dbg !37 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !40, metadata !14), !dbg !41
  %3 = load i32, i32* %2, align 4, !dbg !42
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i32 0, i32 0), i32 %3), !dbg !43
  ret void, !dbg !44
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !45 {
  %1 = alloca %struct.Person, align 4
  call void @llvm.dbg.declare(metadata %struct.Person* %1, metadata !48, metadata !14), !dbg !57
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !57
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 4, i1 false), !dbg !57
  %3 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !58
  call void @func2(i32* %3), !dbg !59
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 1, !dbg !60
  %5 = getelementptr inbounds [10 x i8], [10 x i8]* %4, i32 0, i32 0, !dbg !61
  call void @func1(i8* %5), !dbg !62
  %6 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !63
  %7 = load i32, i32* %6, align 4, !dbg !63
  call void @func3(i32 %7), !dbg !64
  ret i32 0, !dbg !65
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #4

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "func1", scope: !1, file: !1, line: 9, type: !9, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!13 = !DILocalVariable(name: "name", arg: 1, scope: !8, file: !1, line: 9, type: !11)
!14 = !DIExpression()
!15 = !DILocation(line: 9, column: 18, scope: !8)
!16 = !DILocation(line: 10, column: 18, scope: !8)
!17 = !DILocation(line: 10, column: 5, scope: !8)
!18 = !DILocation(line: 11, column: 5, scope: !8)
!19 = !DILocation(line: 12, column: 1, scope: !8)
!20 = distinct !DISubprogram(name: "func2", scope: !1, file: !1, line: 14, type: !21, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!21 = !DISubroutineType(types: !22)
!22 = !{null, !23}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!25 = !DILocalVariable(name: "age", arg: 1, scope: !20, file: !1, line: 14, type: !23)
!26 = !DILocation(line: 14, column: 17, scope: !20)
!27 = !DILocalVariable(name: "tmp", scope: !20, file: !1, line: 15, type: !23)
!28 = !DILocation(line: 15, column: 10, scope: !20)
!29 = !DILocation(line: 15, column: 16, scope: !20)
!30 = !DILocation(line: 15, column: 20, scope: !20)
!31 = !DILocation(line: 16, column: 19, scope: !20)
!32 = !DILocation(line: 16, column: 18, scope: !20)
!33 = !DILocation(line: 16, column: 5, scope: !20)
!34 = !DILocation(line: 17, column: 6, scope: !20)
!35 = !DILocation(line: 17, column: 10, scope: !20)
!36 = !DILocation(line: 18, column: 1, scope: !20)
!37 = distinct !DISubprogram(name: "func3", scope: !1, file: !1, line: 20, type: !38, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!38 = !DISubroutineType(types: !39)
!39 = !{null, !24}
!40 = !DILocalVariable(name: "age", arg: 1, scope: !37, file: !1, line: 20, type: !24)
!41 = !DILocation(line: 20, column: 16, scope: !37)
!42 = !DILocation(line: 21, column: 20, scope: !37)
!43 = !DILocation(line: 21, column: 5, scope: !37)
!44 = !DILocation(line: 22, column: 1, scope: !37)
!45 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 24, type: !46, isLocal: false, isDefinition: true, scopeLine: 24, isOptimized: false, unit: !0, variables: !2)
!46 = !DISubroutineType(types: !47)
!47 = !{!24}
!48 = !DILocalVariable(name: "p", scope: !45, file: !1, line: 25, type: !49)
!49 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 7, baseType: !50)
!50 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !1, line: 4, size: 128, elements: !51)
!51 = !{!52, !53}
!52 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !50, file: !1, line: 5, baseType: !24, size: 32)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !50, file: !1, line: 6, baseType: !54, size: 80, offset: 32)
!54 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 80, elements: !55)
!55 = !{!56}
!56 = !DISubrange(count: 10)
!57 = !DILocation(line: 25, column: 12, scope: !45)
!58 = !DILocation(line: 26, column: 14, scope: !45)
!59 = !DILocation(line: 26, column: 5, scope: !45)
!60 = !DILocation(line: 27, column: 13, scope: !45)
!61 = !DILocation(line: 27, column: 11, scope: !45)
!62 = !DILocation(line: 27, column: 5, scope: !45)
!63 = !DILocation(line: 28, column: 13, scope: !45)
!64 = !DILocation(line: 28, column: 5, scope: !45)
!65 = !DILocation(line: 29, column: 1, scope: !45)
