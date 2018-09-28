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

; Function Attrs: nounwind ssp uwtable
define void @func1(i8*) local_unnamed_addr #0 !dbg !8 {
  tail call void @llvm.dbg.value(metadata i8* %0, i64 0, metadata !14, metadata !15), !dbg !16
  %2 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i8* %0), !dbg !17
  %3 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false, i1 true), !dbg !18
  %4 = tail call i8* @__strncpy_chk(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), i64 10, i64 %3) #4, !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #1

; Function Attrs: nounwind
declare i8* @__strncpy_chk(i8*, i8*, i64, i64) local_unnamed_addr #1

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #2

; Function Attrs: nounwind ssp uwtable
define void @func2(i32* nocapture) local_unnamed_addr #0 !dbg !20 {
  tail call void @llvm.dbg.value(metadata i32* %0, i64 0, metadata !26, metadata !15), !dbg !27
  %2 = load i32, i32* %0, align 4, !dbg !28, !tbaa !29
  %3 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 0), i32 %2), !dbg !33
  store i32 30, i32* %0, align 4, !dbg !34, !tbaa !29
  ret void, !dbg !35
}

; Function Attrs: nounwind ssp uwtable
define void @func3(i32) local_unnamed_addr #0 !dbg !36 {
  tail call void @llvm.dbg.value(metadata i32 %0, i64 0, metadata !40, metadata !15), !dbg !41
  %2 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i64 0, i64 0), i32 %0), !dbg !42
  ret void, !dbg !43
}

; Function Attrs: nounwind ssp uwtable
define i32 @main() local_unnamed_addr #0 !dbg !44 {
  %1 = alloca %struct.Person, align 4
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !57
  call void @llvm.lifetime.start.p0i8(i64 16, i8* nonnull %2) #4, !dbg !57
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 4, i1 false), !dbg !58
  %3 = getelementptr inbounds %struct.Person, %struct.Person* %1, i64 0, i32 0, !dbg !59
  call void @func2(i32* nonnull %3), !dbg !60
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %1, i64 0, i32 1, i64 0, !dbg !61
  call void @func1(i8* %4), !dbg !62
  %5 = load i32, i32* %3, align 4, !dbg !63, !tbaa !64
  call void @func3(i32 %5), !dbg !66
  call void @llvm.lifetime.end.p0i8(i64 16, i8* nonnull %2) #4, !dbg !67
  ret i32 0, !dbg !67
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #3

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #3

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #3

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #2

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind readnone speculatable }
attributes #3 = { argmemonly nounwind }
attributes #4 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "func1", scope: !1, file: !1, line: 9, type: !9, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !13)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!13 = !{!14}
!14 = !DILocalVariable(name: "name", arg: 1, scope: !8, file: !1, line: 9, type: !11)
!15 = !DIExpression()
!16 = !DILocation(line: 9, column: 18, scope: !8)
!17 = !DILocation(line: 10, column: 5, scope: !8)
!18 = !DILocation(line: 11, column: 5, scope: !8)
!19 = !DILocation(line: 12, column: 1, scope: !8)
!20 = distinct !DISubprogram(name: "func2", scope: !1, file: !1, line: 14, type: !21, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !25)
!21 = !DISubroutineType(types: !22)
!22 = !{null, !23}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!25 = !{!26}
!26 = !DILocalVariable(name: "age", arg: 1, scope: !20, file: !1, line: 14, type: !23)
!27 = !DILocation(line: 14, column: 17, scope: !20)
!28 = !DILocation(line: 16, column: 18, scope: !20)
!29 = !{!30, !30, i64 0}
!30 = !{!"int", !31, i64 0}
!31 = !{!"omnipotent char", !32, i64 0}
!32 = !{!"Simple C/C++ TBAA"}
!33 = !DILocation(line: 16, column: 5, scope: !20)
!34 = !DILocation(line: 18, column: 10, scope: !20)
!35 = !DILocation(line: 20, column: 1, scope: !20)
!36 = distinct !DISubprogram(name: "func3", scope: !1, file: !1, line: 22, type: !37, isLocal: false, isDefinition: true, scopeLine: 22, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !39)
!37 = !DISubroutineType(types: !38)
!38 = !{null, !24}
!39 = !{!40}
!40 = !DILocalVariable(name: "age", arg: 1, scope: !36, file: !1, line: 22, type: !24)
!41 = !DILocation(line: 22, column: 16, scope: !36)
!42 = !DILocation(line: 23, column: 5, scope: !36)
!43 = !DILocation(line: 24, column: 1, scope: !36)
!44 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 26, type: !45, isLocal: false, isDefinition: true, scopeLine: 26, isOptimized: true, unit: !0, variables: !47)
!45 = !DISubroutineType(types: !46)
!46 = !{!24}
!47 = !{!48}
!48 = !DILocalVariable(name: "p", scope: !44, file: !1, line: 27, type: !49)
!49 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 7, baseType: !50)
!50 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !1, line: 4, size: 128, elements: !51)
!51 = !{!52, !53}
!52 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !50, file: !1, line: 5, baseType: !24, size: 32)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !50, file: !1, line: 6, baseType: !54, size: 80, offset: 32)
!54 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 80, elements: !55)
!55 = !{!56}
!56 = !DISubrange(count: 10)
!57 = !DILocation(line: 27, column: 5, scope: !44)
!58 = !DILocation(line: 27, column: 12, scope: !44)
!59 = !DILocation(line: 28, column: 14, scope: !44)
!60 = !DILocation(line: 28, column: 5, scope: !44)
!61 = !DILocation(line: 29, column: 11, scope: !44)
!62 = !DILocation(line: 29, column: 5, scope: !44)
!63 = !DILocation(line: 30, column: 13, scope: !44)
!64 = !{!65, !30, i64 0}
!65 = !{!"", !30, i64 0, !31, i64 4}
!66 = !DILocation(line: 30, column: 5, scope: !44)
!67 = !DILocation(line: 31, column: 1, scope: !44)
