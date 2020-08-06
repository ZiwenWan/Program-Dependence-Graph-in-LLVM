; ModuleID = 'test_union.c'
source_filename = "test_union.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%union.UTEST = type { %struct.anon }
%struct.anon = type { i32, i32 }

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%union.UTEST*) #0 !dbg !8 {
  %2 = alloca i32, align 4
  %3 = alloca %union.UTEST*, align 8
  store %union.UTEST* %0, %union.UTEST** %3, align 8
  call void @llvm.dbg.declare(metadata %union.UTEST** %3, metadata !24, metadata !25), !dbg !26
  %4 = load %union.UTEST*, %union.UTEST** %3, align 8, !dbg !27
  %5 = bitcast %union.UTEST* %4 to %struct.anon*, !dbg !29
  %6 = getelementptr inbounds %struct.anon, %struct.anon* %5, i32 0, i32 0, !dbg !30
  %7 = load i32, i32* %6, align 4, !dbg !30
  %8 = icmp sgt i32 %7, 5, !dbg !31
  br i1 %8, label %9, label %14, !dbg !32

; <label>:9:                                      ; preds = %1
  %10 = load %union.UTEST*, %union.UTEST** %3, align 8, !dbg !33
  %11 = bitcast %union.UTEST* %10 to %struct.anon*, !dbg !34
  %12 = getelementptr inbounds %struct.anon, %struct.anon* %11, i32 0, i32 0, !dbg !35
  %13 = load i32, i32* %12, align 4, !dbg !35
  store i32 %13, i32* %2, align 4, !dbg !36
  br label %15, !dbg !36

; <label>:14:                                     ; preds = %1
  store i32 5, i32* %2, align 4, !dbg !37
  br label %15, !dbg !37

; <label>:15:                                     ; preds = %14, %9
  %16 = load i32, i32* %2, align 4, !dbg !38
  ret i32 %16, !dbg !38
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !39 {
  %1 = alloca i32, align 4
  %2 = alloca %union.UTEST, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %union.UTEST* %2, metadata !42, metadata !25), !dbg !43
  %3 = bitcast %union.UTEST* %2 to %struct.anon*, !dbg !44
  %4 = getelementptr inbounds %struct.anon, %struct.anon* %3, i32 0, i32 0, !dbg !45
  store i32 10, i32* %4, align 4, !dbg !46
  %5 = call i32 @f(%union.UTEST* %2), !dbg !47
  ret i32 0, !dbg !48
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_union.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 14, type: !9, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = distinct !DICompositeType(tag: DW_TAG_union_type, name: "UTEST", file: !1, line: 4, size: 64, elements: !14)
!14 = !{!15, !20}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "B", scope: !13, file: !1, line: 8, baseType: !16, size: 64)
!16 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !13, file: !1, line: 5, size: 64, elements: !17)
!17 = !{!18, !19}
!18 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !16, file: !1, line: 6, baseType: !11, size: 32)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !16, file: !1, line: 7, baseType: !11, size: 32, offset: 32)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "C", scope: !13, file: !1, line: 11, baseType: !21, size: 32)
!21 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !13, file: !1, line: 9, size: 32, elements: !22)
!22 = !{!23}
!23 = !DIDerivedType(tag: DW_TAG_member, name: "d", scope: !21, file: !1, line: 10, baseType: !11, size: 32)
!24 = !DILocalVariable(name: "utest", arg: 1, scope: !8, file: !1, line: 14, type: !12)
!25 = !DIExpression()
!26 = !DILocation(line: 14, column: 20, scope: !8)
!27 = !DILocation(line: 15, column: 9, scope: !28)
!28 = distinct !DILexicalBlock(scope: !8, file: !1, line: 15, column: 9)
!29 = !DILocation(line: 15, column: 16, scope: !28)
!30 = !DILocation(line: 15, column: 18, scope: !28)
!31 = !DILocation(line: 15, column: 20, scope: !28)
!32 = !DILocation(line: 15, column: 9, scope: !8)
!33 = !DILocation(line: 16, column: 16, scope: !28)
!34 = !DILocation(line: 16, column: 23, scope: !28)
!35 = !DILocation(line: 16, column: 25, scope: !28)
!36 = !DILocation(line: 16, column: 9, scope: !28)
!37 = !DILocation(line: 17, column: 5, scope: !8)
!38 = !DILocation(line: 18, column: 1, scope: !8)
!39 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 20, type: !40, isLocal: false, isDefinition: true, scopeLine: 20, isOptimized: false, unit: !0, variables: !2)
!40 = !DISubroutineType(types: !41)
!41 = !{!11}
!42 = !DILocalVariable(name: "utest", scope: !39, file: !1, line: 21, type: !13)
!43 = !DILocation(line: 21, column: 17, scope: !39)
!44 = !DILocation(line: 22, column: 11, scope: !39)
!45 = !DILocation(line: 22, column: 13, scope: !39)
!46 = !DILocation(line: 22, column: 15, scope: !39)
!47 = !DILocation(line: 23, column: 5, scope: !39)
!48 = !DILocation(line: 24, column: 5, scope: !39)
