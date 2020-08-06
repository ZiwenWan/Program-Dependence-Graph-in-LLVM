; ModuleID = 'test_alias.c'
source_filename = "test_alias.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(i32*) #0 !dbg !8 {
  %2 = alloca i32*, align 8
  %3 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !13, metadata !14), !dbg !15
  call void @llvm.dbg.declare(metadata i32** %3, metadata !16, metadata !14), !dbg !17
  %4 = load i32*, i32** %2, align 8, !dbg !18
  store i32* %4, i32** %3, align 8, !dbg !17
  %5 = load i32*, i32** %3, align 8, !dbg !19
  %6 = load i32, i32* %5, align 4, !dbg !20
  ret i32 %6, !dbg !21
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !22 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !25, metadata !14), !dbg !26
  store i32 5, i32* %2, align 4, !dbg !26
  call void @llvm.dbg.declare(metadata i32* %3, metadata !27, metadata !14), !dbg !28
  %4 = call i32 @f(i32* %2), !dbg !29
  store i32 %4, i32* %3, align 4, !dbg !28
  ret i32 0, !dbg !30
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
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 2, type: !9, isLocal: false, isDefinition: true, scopeLine: 2, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!13 = !DILocalVariable(name: "a", arg: 1, scope: !8, file: !1, line: 2, type: !12)
!14 = !DIExpression()
!15 = !DILocation(line: 2, column: 12, scope: !8)
!16 = !DILocalVariable(name: "b", scope: !8, file: !1, line: 3, type: !12)
!17 = !DILocation(line: 3, column: 10, scope: !8)
!18 = !DILocation(line: 3, column: 14, scope: !8)
!19 = !DILocation(line: 4, column: 13, scope: !8)
!20 = !DILocation(line: 4, column: 12, scope: !8)
!21 = !DILocation(line: 4, column: 5, scope: !8)
!22 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 7, type: !23, isLocal: false, isDefinition: true, scopeLine: 7, isOptimized: false, unit: !0, variables: !2)
!23 = !DISubroutineType(types: !24)
!24 = !{!11}
!25 = !DILocalVariable(name: "a", scope: !22, file: !1, line: 8, type: !11)
!26 = !DILocation(line: 8, column: 9, scope: !22)
!27 = !DILocalVariable(name: "b", scope: !22, file: !1, line: 9, type: !11)
!28 = !DILocation(line: 9, column: 9, scope: !22)
!29 = !DILocation(line: 9, column: 13, scope: !22)
!30 = !DILocation(line: 10, column: 5, scope: !22)
