; ModuleID = 'test_malloc_anno.c'
source_filename = "test_malloc_anno.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

@.str = private unnamed_addr constant [8 x i8] c"persist\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [19 x i8] c"test_malloc_anno.c\00", section "llvm.metadata"

; Function Attrs: noinline nounwind optnone ssp uwtable
define i8* @pz_malloc(i64) #0 !dbg !11 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void @llvm.dbg.declare(metadata i64* %2, metadata !20, metadata !DIExpression()), !dbg !21
  %3 = load i64, i64* %2, align 8, !dbg !22
  %4 = call i8* @malloc(i64 %3) #5, !dbg !23
  ret i8* %4, !dbg !24
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @pz_free(i8*) #0 !dbg !25 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  call void @llvm.dbg.declare(metadata i8** %2, metadata !28, metadata !DIExpression()), !dbg !29
  %3 = load i8*, i8** %2, align 8, !dbg !30
  call void @free(i8* %3), !dbg !31
  ret void, !dbg !32
}

declare void @free(i8*) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main(i32, i8**) #0 !dbg !33 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i32*, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !39, metadata !DIExpression()), !dbg !40
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !41, metadata !DIExpression()), !dbg !42
  call void @llvm.dbg.declare(metadata i32** %6, metadata !43, metadata !DIExpression()), !dbg !44
  %7 = bitcast i32** %6 to i8*, !dbg !45
  call void @llvm.var.annotation(i8* %7, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.1, i32 0, i32 0), i32 11), !dbg !45
  %8 = call i8* @pz_malloc(i64 4), !dbg !46
  %9 = bitcast i8* %8 to i32*, !dbg !47
  store i32* %9, i32** %6, align 8, !dbg !44
  %10 = load i32*, i32** %6, align 8, !dbg !48
  store i32 7, i32* %10, align 4, !dbg !49
  %11 = load i32*, i32** %6, align 8, !dbg !50
  %12 = bitcast i32* %11 to i8*, !dbg !50
  call void @pz_free(i8* %12), !dbg !51
  ret i32 0, !dbg !52
}

; Function Attrs: nounwind
declare void @llvm.var.annotation(i8*, i8*, i8*, i32) #4

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { allocsize(0) }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!6, !7, !8, !9}
!llvm.ident = !{!10}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 9.0.0 (tags/RELEASE_900/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, nameTableKind: GNU)
!1 = !DIFile(filename: "test_malloc_anno.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!2 = !{}
!3 = !{!4}
!4 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!5 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!6 = !{i32 2, !"Dwarf Version", i32 4}
!7 = !{i32 2, !"Debug Info Version", i32 3}
!8 = !{i32 1, !"wchar_size", i32 4}
!9 = !{i32 7, !"PIC Level", i32 2}
!10 = !{!"clang version 9.0.0 (tags/RELEASE_900/final)"}
!11 = distinct !DISubprogram(name: "pz_malloc", scope: !1, file: !1, line: 4, type: !12, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!12 = !DISubroutineType(types: !13)
!13 = !{!14, !15}
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!15 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !16, line: 31, baseType: !17)
!16 = !DIFile(filename: "/usr/include/sys/_types/_size_t.h", directory: "")
!17 = !DIDerivedType(tag: DW_TAG_typedef, name: "__darwin_size_t", file: !18, line: 92, baseType: !19)
!18 = !DIFile(filename: "/usr/include/i386/_types.h", directory: "")
!19 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!20 = !DILocalVariable(name: "size", arg: 1, scope: !11, file: !1, line: 4, type: !15)
!21 = !DILocation(line: 4, column: 24, scope: !11)
!22 = !DILocation(line: 4, column: 46, scope: !11)
!23 = !DILocation(line: 4, column: 39, scope: !11)
!24 = !DILocation(line: 4, column: 32, scope: !11)
!25 = distinct !DISubprogram(name: "pz_free", scope: !1, file: !1, line: 7, type: !26, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!26 = !DISubroutineType(types: !27)
!27 = !{null, !14}
!28 = !DILocalVariable(name: "p", arg: 1, scope: !25, file: !1, line: 7, type: !14)
!29 = !DILocation(line: 7, column: 20, scope: !25)
!30 = !DILocation(line: 7, column: 30, scope: !25)
!31 = !DILocation(line: 7, column: 25, scope: !25)
!32 = !DILocation(line: 7, column: 34, scope: !25)
!33 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 9, type: !34, scopeLine: 9, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!34 = !DISubroutineType(types: !35)
!35 = !{!5, !5, !36}
!36 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !37, size: 64)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !38, size: 64)
!38 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!39 = !DILocalVariable(name: "argc", arg: 1, scope: !33, file: !1, line: 9, type: !5)
!40 = !DILocation(line: 9, column: 14, scope: !33)
!41 = !DILocalVariable(name: "argv", arg: 2, scope: !33, file: !1, line: 9, type: !36)
!42 = !DILocation(line: 9, column: 27, scope: !33)
!43 = !DILocalVariable(name: "p_r2", scope: !33, file: !1, line: 11, type: !4)
!44 = !DILocation(line: 11, column: 9, scope: !33)
!45 = !DILocation(line: 10, column: 4, scope: !33)
!46 = !DILocation(line: 11, column: 22, scope: !33)
!47 = !DILocation(line: 11, column: 16, scope: !33)
!48 = !DILocation(line: 12, column: 5, scope: !33)
!49 = !DILocation(line: 12, column: 10, scope: !33)
!50 = !DILocation(line: 13, column: 13, scope: !33)
!51 = !DILocation(line: 13, column: 5, scope: !33)
!52 = !DILocation(line: 14, column: 5, scope: !33)
