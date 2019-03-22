; ModuleID = 'test_sample.c'
source_filename = "test_sample.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @test() #0 !dbg !8 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  call void @llvm.dbg.declare(metadata [10 x i32]* %1, metadata !11, metadata !16), !dbg !17
  call void @llvm.dbg.declare(metadata [10 x i32]* %2, metadata !18, metadata !16), !dbg !19
  call void @llvm.dbg.declare(metadata i32* %3, metadata !20, metadata !16), !dbg !21
  call void @llvm.dbg.declare(metadata i32* %4, metadata !22, metadata !16), !dbg !23
  %5 = load i32, i32* %4, align 4, !dbg !24
  %6 = icmp ne i32 %5, 0, !dbg !24
  br i1 %6, label %7, label %13, !dbg !26

; <label>:7:                                      ; preds = %0
  %8 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i64 0, i64 1, !dbg !27
  %9 = load i32, i32* %8, align 4, !dbg !27
  %10 = sext i32 %9 to i64, !dbg !29
  %11 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %10, !dbg !29
  %12 = load i32, i32* %11, align 4, !dbg !29
  store i32 %12, i32* %3, align 4, !dbg !30
  br label %19, !dbg !31

; <label>:13:                                     ; preds = %0
  %14 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i64 0, i64 2, !dbg !32
  %15 = load i32, i32* %14, align 8, !dbg !32
  %16 = sext i32 %15 to i64, !dbg !34
  %17 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %16, !dbg !34
  %18 = load i32, i32* %17, align 4, !dbg !34
  store i32 %18, i32* %3, align 4, !dbg !35
  br label %19

; <label>:19:                                     ; preds = %13, %7
  %20 = load i32, i32* %3, align 4, !dbg !36
  %21 = add nsw i32 %20, 1, !dbg !36
  store i32 %21, i32* %3, align 4, !dbg !36
  ret void, !dbg !37
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_sample.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "test", scope: !1, file: !1, line: 3, type: !9, isLocal: false, isDefinition: true, scopeLine: 4, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null}
!11 = !DILocalVariable(name: "a", scope: !8, file: !1, line: 5, type: !12)
!12 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, size: 320, elements: !14)
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{!15}
!15 = !DISubrange(count: 10)
!16 = !DIExpression()
!17 = !DILocation(line: 5, column: 6, scope: !8)
!18 = !DILocalVariable(name: "a2", scope: !8, file: !1, line: 6, type: !12)
!19 = !DILocation(line: 6, column: 6, scope: !8)
!20 = !DILocalVariable(name: "b", scope: !8, file: !1, line: 6, type: !13)
!21 = !DILocation(line: 6, column: 13, scope: !8)
!22 = !DILocalVariable(name: "key", scope: !8, file: !1, line: 8, type: !13)
!23 = !DILocation(line: 8, column: 6, scope: !8)
!24 = !DILocation(line: 11, column: 5, scope: !25)
!25 = distinct !DILexicalBlock(scope: !8, file: !1, line: 11, column: 5)
!26 = !DILocation(line: 11, column: 5, scope: !8)
!27 = !DILocation(line: 13, column: 9, scope: !28)
!28 = distinct !DILexicalBlock(scope: !25, file: !1, line: 12, column: 2)
!29 = !DILocation(line: 13, column: 7, scope: !28)
!30 = !DILocation(line: 13, column: 5, scope: !28)
!31 = !DILocation(line: 14, column: 2, scope: !28)
!32 = !DILocation(line: 17, column: 9, scope: !33)
!33 = distinct !DILexicalBlock(scope: !25, file: !1, line: 16, column: 2)
!34 = !DILocation(line: 17, column: 7, scope: !33)
!35 = !DILocation(line: 17, column: 5, scope: !33)
!36 = !DILocation(line: 19, column: 3, scope: !8)
!37 = !DILocation(line: 20, column: 2, scope: !8)
