; ModuleID = 'test_struct_gen.c'
source_filename = "test_struct_gen.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.person = type { i32, i8* }
%struct.t = type { i32 (%struct.person*)*, void (%struct.person*)* }
%struct.tt = type { %struct.t* }

@person = common global %struct.person zeroinitializer, align 8, !dbg !0
@t = common global %struct.t zeroinitializer, align 8, !dbg !6
@tt = common global %struct.tt zeroinitializer, align 8, !dbg !26

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !38 {
  ret i32 0, !dbg !41
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!33, !34, !35, !36}
!llvm.ident = !{!37}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "person", scope: !2, file: !3, line: 4, type: !16, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "test_struct_gen.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
!4 = !{}
!5 = !{!0, !6, !26}
!6 = !DIGlobalVariableExpression(var: !7)
!7 = distinct !DIGlobalVariable(name: "t", scope: !2, file: !3, line: 9, type: !8, isLocal: false, isDefinition: true)
!8 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "t", file: !3, line: 6, size: 128, elements: !9)
!9 = !{!10, !22}
!10 = !DIDerivedType(tag: DW_TAG_member, name: "init", scope: !8, file: !3, line: 7, baseType: !11, size: 64)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DISubroutineType(types: !13)
!13 = !{!14, !15}
!14 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !16, size: 64)
!16 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !3, line: 1, size: 128, elements: !17)
!17 = !{!18, !19}
!18 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !16, file: !3, line: 2, baseType: !14, size: 32)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !16, file: !3, line: 3, baseType: !20, size: 64, offset: 64)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "uninit", scope: !8, file: !3, line: 8, baseType: !23, size: 64, offset: 64)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DISubroutineType(types: !25)
!25 = !{null, !15}
!26 = !DIGlobalVariableExpression(var: !27)
!27 = distinct !DIGlobalVariable(name: "tt", scope: !2, file: !3, line: 13, type: !28, isLocal: false, isDefinition: true)
!28 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "tt", file: !3, line: 11, size: 64, elements: !29)
!29 = !{!30}
!30 = !DIDerivedType(tag: DW_TAG_member, name: "T", scope: !28, file: !3, line: 12, baseType: !31, size: 64)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !32, size: 64)
!32 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !8)
!33 = !{i32 2, !"Dwarf Version", i32 4}
!34 = !{i32 2, !"Debug Info Version", i32 3}
!35 = !{i32 1, !"wchar_size", i32 4}
!36 = !{i32 7, !"PIC Level", i32 2}
!37 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!38 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 15, type: !39, isLocal: false, isDefinition: true, scopeLine: 15, isOptimized: false, unit: !2, variables: !4)
!39 = !DISubroutineType(types: !40)
!40 = !{!14}
!41 = !DILocation(line: 17, column: 1, scope: !38)
