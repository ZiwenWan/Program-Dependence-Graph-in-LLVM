; ModuleID = 'test_two_source_data_dependency.c'
source_filename = "test_two_source_data_dependency.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.struct_t = type { i32, float }

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @getFlag() #0 !dbg !8 {
  ret i32 1, !dbg !12
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !13 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.struct_t, align 4
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.struct_t* %2, metadata !14, metadata !20), !dbg !21
  call void @llvm.dbg.declare(metadata i32* %3, metadata !22, metadata !20), !dbg !23
  store i32 5, i32* %3, align 4, !dbg !23
  call void @llvm.dbg.declare(metadata float* %4, metadata !24, metadata !20), !dbg !25
  store float 6.000000e+00, float* %4, align 4, !dbg !25
  %5 = call i32 @getFlag(), !dbg !26
  %6 = icmp ne i32 %5, 0, !dbg !26
  br i1 %6, label %7, label %10, !dbg !28

; <label>:7:                                      ; preds = %0
  %8 = load i32, i32* %3, align 4, !dbg !29
  %9 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %2, i32 0, i32 0, !dbg !31
  store i32 %8, i32* %9, align 4, !dbg !32
  br label %13, !dbg !33

; <label>:10:                                     ; preds = %0
  %11 = load float, float* %4, align 4, !dbg !34
  %12 = getelementptr inbounds %struct.struct_t, %struct.struct_t* %2, i32 0, i32 1, !dbg !36
  store float %11, float* %12, align 4, !dbg !37
  br label %13

; <label>:13:                                     ; preds = %10, %7
  ret i32 0, !dbg !38
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_two_source_data_dependency.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "getFlag", scope: !1, file: !1, line: 8, type: !9, isLocal: false, isDefinition: true, scopeLine: 8, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DILocation(line: 9, column: 5, scope: !8)
!13 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 12, type: !9, isLocal: false, isDefinition: true, scopeLine: 12, isOptimized: false, unit: !0, variables: !2)
!14 = !DILocalVariable(name: "s", scope: !13, file: !1, line: 13, type: !15)
!15 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "struct_t", file: !1, line: 3, size: 64, elements: !16)
!16 = !{!17, !18}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !15, file: !1, line: 4, baseType: !11, size: 32)
!18 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !15, file: !1, line: 5, baseType: !19, size: 32, offset: 32)
!19 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!20 = !DIExpression()
!21 = !DILocation(line: 13, column: 21, scope: !13)
!22 = !DILocalVariable(name: "a", scope: !13, file: !1, line: 14, type: !11)
!23 = !DILocation(line: 14, column: 9, scope: !13)
!24 = !DILocalVariable(name: "b", scope: !13, file: !1, line: 15, type: !19)
!25 = !DILocation(line: 15, column: 11, scope: !13)
!26 = !DILocation(line: 17, column: 9, scope: !27)
!27 = distinct !DILexicalBlock(scope: !13, file: !1, line: 17, column: 9)
!28 = !DILocation(line: 17, column: 9, scope: !13)
!29 = !DILocation(line: 18, column: 15, scope: !30)
!30 = distinct !DILexicalBlock(scope: !27, file: !1, line: 17, column: 20)
!31 = !DILocation(line: 18, column: 11, scope: !30)
!32 = !DILocation(line: 18, column: 13, scope: !30)
!33 = !DILocation(line: 19, column: 5, scope: !30)
!34 = !DILocation(line: 20, column: 15, scope: !35)
!35 = distinct !DILexicalBlock(scope: !27, file: !1, line: 19, column: 12)
!36 = !DILocation(line: 20, column: 11, scope: !35)
!37 = !DILocation(line: 20, column: 13, scope: !35)
!38 = !DILocation(line: 24, column: 5, scope: !13)
