; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f2(i32*) #0 !dbg !8 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !13, metadata !14), !dbg !15
  %3 = load i32*, i32** %2, align 8, !dbg !16
  %4 = load i32, i32* %3, align 4, !dbg !17
  ret i32 %4, !dbg !18
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f1(i32*) #0 !dbg !19 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !20, metadata !14), !dbg !21
  %3 = load i32*, i32** %2, align 8, !dbg !22
  %4 = call i32 @f2(i32* %3), !dbg !23
  ret i32 %4, !dbg !24
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !25 {
  %1 = alloca i32, align 4
  call void @llvm.dbg.declare(metadata i32* %1, metadata !28, metadata !14), !dbg !29
  store i32 5, i32* %1, align 4, !dbg !29
  %2 = call i32 @f1(i32* %1), !dbg !30
  ret i32 0, !dbg !31
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/unittest")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f2", scope: !1, file: !1, line: 3, type: !9, isLocal: false, isDefinition: true, scopeLine: 3, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!13 = !DILocalVariable(name: "a", arg: 1, scope: !8, file: !1, line: 3, type: !12)
!14 = !DIExpression()
!15 = !DILocation(line: 3, column: 13, scope: !8)
!16 = !DILocation(line: 4, column: 13, scope: !8)
!17 = !DILocation(line: 4, column: 12, scope: !8)
!18 = !DILocation(line: 4, column: 5, scope: !8)
!19 = distinct !DISubprogram(name: "f1", scope: !1, file: !1, line: 7, type: !9, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!20 = !DILocalVariable(name: "a", arg: 1, scope: !19, file: !1, line: 7, type: !12)
!21 = !DILocation(line: 7, column: 13, scope: !19)
!22 = !DILocation(line: 8, column: 15, scope: !19)
!23 = !DILocation(line: 8, column: 12, scope: !19)
!24 = !DILocation(line: 8, column: 5, scope: !19)
!25 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, type: !26, isLocal: false, isDefinition: true, scopeLine: 11, isOptimized: false, unit: !0, variables: !2)
!26 = !DISubroutineType(types: !27)
!27 = !{!11}
!28 = !DILocalVariable(name: "a", scope: !25, file: !1, line: 12, type: !11)
!29 = !DILocation(line: 12, column: 6, scope: !25)
!30 = !DILocation(line: 13, column: 2, scope: !25)
!31 = !DILocation(line: 14, column: 1, scope: !25)
