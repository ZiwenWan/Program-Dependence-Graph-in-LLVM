; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"new_name\00", align 1
@.str.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
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
  store i32* %4, i32** %3, align 8, !dbg !28
  %5 = load i32*, i32** %3, align 8, !dbg !30
  %6 = load i32, i32* %5, align 4, !dbg !31
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i32 0, i32 0), i32 %6), !dbg !32
  ret void, !dbg !33
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @func3(i32) #0 !dbg !34 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !37, metadata !14), !dbg !38
  %3 = load i32, i32* %2, align 4, !dbg !39
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i32 0, i32 0), i32 %3), !dbg !40
  ret void, !dbg !41
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !42 {
  %1 = alloca %struct.Person, align 4
  call void @llvm.dbg.declare(metadata %struct.Person* %1, metadata !45, metadata !14), !dbg !54
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !54
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 4, i1 false), !dbg !54
  %3 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !55
  call void @func2(i32* %3), !dbg !56
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 1, !dbg !57
  %5 = getelementptr inbounds [10 x i8], [10 x i8]* %4, i32 0, i32 0, !dbg !58
  call void @func1(i8* %5), !dbg !59
  %6 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !60
  %7 = load i32, i32* %6, align 4, !dbg !60
  call void @func3(i32 %7), !dbg !61
  ret i32 0, !dbg !62
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
!27 = !DILocalVariable(name: "tmp_age", scope: !20, file: !1, line: 15, type: !23)
!28 = !DILocation(line: 15, column: 10, scope: !20)
!29 = !DILocation(line: 15, column: 20, scope: !20)
!30 = !DILocation(line: 16, column: 21, scope: !20)
!31 = !DILocation(line: 16, column: 20, scope: !20)
!32 = !DILocation(line: 16, column: 5, scope: !20)
!33 = !DILocation(line: 17, column: 1, scope: !20)
!34 = distinct !DISubprogram(name: "func3", scope: !1, file: !1, line: 19, type: !35, isLocal: false, isDefinition: true, scopeLine: 19, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!35 = !DISubroutineType(types: !36)
!36 = !{null, !24}
!37 = !DILocalVariable(name: "age", arg: 1, scope: !34, file: !1, line: 19, type: !24)
!38 = !DILocation(line: 19, column: 16, scope: !34)
!39 = !DILocation(line: 20, column: 20, scope: !34)
!40 = !DILocation(line: 20, column: 5, scope: !34)
!41 = !DILocation(line: 21, column: 1, scope: !34)
!42 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 23, type: !43, isLocal: false, isDefinition: true, scopeLine: 23, isOptimized: false, unit: !0, variables: !2)
!43 = !DISubroutineType(types: !44)
!44 = !{!24}
!45 = !DILocalVariable(name: "p", scope: !42, file: !1, line: 24, type: !46)
!46 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 7, baseType: !47)
!47 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !1, line: 4, size: 128, elements: !48)
!48 = !{!49, !50}
!49 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !47, file: !1, line: 5, baseType: !24, size: 32)
!50 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !47, file: !1, line: 6, baseType: !51, size: 80, offset: 32)
!51 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 80, elements: !52)
!52 = !{!53}
!53 = !DISubrange(count: 10)
!54 = !DILocation(line: 24, column: 12, scope: !42)
!55 = !DILocation(line: 25, column: 14, scope: !42)
!56 = !DILocation(line: 25, column: 5, scope: !42)
!57 = !DILocation(line: 26, column: 13, scope: !42)
!58 = !DILocation(line: 26, column: 11, scope: !42)
!59 = !DILocation(line: 26, column: 5, scope: !42)
!60 = !DILocation(line: 27, column: 13, scope: !42)
!61 = !DILocation(line: 27, column: 5, scope: !42)
!62 = !DILocation(line: 28, column: 1, scope: !42)
