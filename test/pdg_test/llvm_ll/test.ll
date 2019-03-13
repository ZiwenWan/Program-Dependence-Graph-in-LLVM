; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, i8* }

@.str = private unnamed_addr constant [2 x i8] c"J\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i32 0, i32 0) }, align 8
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @k(%struct.Person**) #0 !dbg !8 {
  %2 = alloca %struct.Person**, align 8
  store %struct.Person** %0, %struct.Person*** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person*** %2, metadata !20, metadata !21), !dbg !22
  ret i32 0, !dbg !23
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 !dbg !24 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !27, metadata !21), !dbg !28
  %3 = call i32 @k(%struct.Person** %2), !dbg !29
  ret i32 0, !dbg !30
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !31 {
  %1 = alloca %struct.Person, align 8
  call void @llvm.dbg.declare(metadata %struct.Person* %1, metadata !34, metadata !21), !dbg !35
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !35
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 8, i1 false), !dbg !35
  %3 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !36
  %4 = load i32, i32* %3, align 8, !dbg !36
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %4), !dbg !37
  %6 = call i32 @f(%struct.Person* %1), !dbg !38
  ret i32 0, !dbg !39
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

declare i32 @printf(i8*, ...) #3

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "k", scope: !1, file: !1, line: 7, type: !9, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !1, line: 2, size: 128, elements: !15)
!15 = !{!16, !17}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !14, file: !1, line: 3, baseType: !11, size: 32)
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !14, file: !1, line: 4, baseType: !18, size: 64, offset: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !DILocalVariable(name: "p", arg: 1, scope: !8, file: !1, line: 7, type: !12)
!21 = !DIExpression()
!22 = !DILocation(line: 7, column: 23, scope: !8)
!23 = !DILocation(line: 8, column: 5, scope: !8)
!24 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 11, type: !25, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!25 = !DISubroutineType(types: !26)
!26 = !{!11, !13}
!27 = !DILocalVariable(name: "p", arg: 1, scope: !24, file: !1, line: 11, type: !13)
!28 = !DILocation(line: 11, column: 22, scope: !24)
!29 = !DILocation(line: 14, column: 5, scope: !24)
!30 = !DILocation(line: 15, column: 5, scope: !24)
!31 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 18, type: !32, isLocal: false, isDefinition: true, scopeLine: 18, isOptimized: false, unit: !0, variables: !2)
!32 = !DISubroutineType(types: !33)
!33 = !{!11}
!34 = !DILocalVariable(name: "p", scope: !31, file: !1, line: 19, type: !14)
!35 = !DILocation(line: 19, column: 19, scope: !31)
!36 = !DILocation(line: 20, column: 22, scope: !31)
!37 = !DILocation(line: 20, column: 5, scope: !31)
!38 = !DILocation(line: 21, column: 5, scope: !31)
!39 = !DILocation(line: 22, column: 1, scope: !31)
