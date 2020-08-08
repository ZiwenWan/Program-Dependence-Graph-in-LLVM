; ModuleID = 'test_alias.c'
source_filename = "test_alias.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.C = type { i32, i32 }

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32* @f(%struct.C*) #0 !dbg !8 {
  %2 = alloca %struct.C*, align 8
  store %struct.C* %0, %struct.C** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.C** %2, metadata !18, metadata !19), !dbg !20
  %3 = load %struct.C*, %struct.C** %2, align 8, !dbg !21
  %4 = getelementptr inbounds %struct.C, %struct.C* %3, i32 0, i32 0, !dbg !22
  ret i32* %4, !dbg !23
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !24 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca %struct.C, align 4
  %4 = alloca i32*, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !27, metadata !19), !dbg !28
  store i32 5, i32* %2, align 4, !dbg !28
  call void @llvm.dbg.declare(metadata %struct.C* %3, metadata !29, metadata !19), !dbg !30
  %5 = getelementptr inbounds %struct.C, %struct.C* %3, i32 0, i32 0, !dbg !31
  %6 = load i32, i32* %2, align 4, !dbg !32
  store i32 %6, i32* %5, align 4, !dbg !31
  %7 = getelementptr inbounds %struct.C, %struct.C* %3, i32 0, i32 1, !dbg !31
  store i32 3, i32* %7, align 4, !dbg !31
  call void @llvm.dbg.declare(metadata i32** %4, metadata !33, metadata !19), !dbg !34
  %8 = call i32* @f(%struct.C* %3), !dbg !35
  store i32* %8, i32** %4, align 8, !dbg !34
  ret i32 0, !dbg !36
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_alias.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 8, type: !9, isLocal: false, isDefinition: true, scopeLine: 8, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !13}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "C", file: !1, line: 3, size: 64, elements: !15)
!15 = !{!16, !17}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !14, file: !1, line: 4, baseType: !12, size: 32)
!17 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !14, file: !1, line: 5, baseType: !12, size: 32, offset: 32)
!18 = !DILocalVariable(name: "c", arg: 1, scope: !8, file: !1, line: 8, type: !13)
!19 = !DIExpression()
!20 = !DILocation(line: 8, column: 18, scope: !8)
!21 = !DILocation(line: 9, column: 14, scope: !8)
!22 = !DILocation(line: 9, column: 17, scope: !8)
!23 = !DILocation(line: 9, column: 5, scope: !8)
!24 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 12, type: !25, isLocal: false, isDefinition: true, scopeLine: 12, isOptimized: false, unit: !0, variables: !2)
!25 = !DISubroutineType(types: !26)
!26 = !{!12}
!27 = !DILocalVariable(name: "a", scope: !24, file: !1, line: 13, type: !12)
!28 = !DILocation(line: 13, column: 9, scope: !24)
!29 = !DILocalVariable(name: "c", scope: !24, file: !1, line: 14, type: !14)
!30 = !DILocation(line: 14, column: 14, scope: !24)
!31 = !DILocation(line: 14, column: 18, scope: !24)
!32 = !DILocation(line: 14, column: 19, scope: !24)
!33 = !DILocalVariable(name: "b", scope: !24, file: !1, line: 15, type: !11)
!34 = !DILocation(line: 15, column: 10, scope: !24)
!35 = !DILocation(line: 15, column: 14, scope: !24)
!36 = !DILocation(line: 16, column: 5, scope: !24)
