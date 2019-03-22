; ModuleID = 'test_arr.c'
source_filename = "test_arr.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.P = type { i16, i8 }

@main.p = private unnamed_addr constant %struct.P { i16 1, i8 119 }, align 2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.P, align 2
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.P* %2, metadata !12, metadata !27), !dbg !28
  %3 = bitcast %struct.P* %2 to i8*, !dbg !28
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.P* @main.p to i8*), i64 4, i32 2, i1 false), !dbg !28
  %4 = getelementptr inbounds %struct.P, %struct.P* %2, i32 0, i32 1, !dbg !29
  %5 = load i8, i8* %4, align 2, !dbg !30
  %6 = and i8 %5, -13, !dbg !30
  %7 = or i8 %6, 12, !dbg !30
  store i8 %7, i8* %4, align 2, !dbg !30
  %8 = getelementptr inbounds %struct.P, %struct.P* %2, i32 0, i32 1, !dbg !31
  %9 = load i8, i8* %8, align 2, !dbg !32
  %10 = and i8 %9, -17, !dbg !32
  %11 = or i8 %10, 16, !dbg !32
  store i8 %11, i8* %8, align 2, !dbg !32
  ret i32 0, !dbg !33
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_arr.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 13, type: !9, isLocal: false, isDefinition: true, scopeLine: 13, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DILocalVariable(name: "p", scope: !8, file: !1, line: 14, type: !13)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "P", file: !1, line: 3, size: 32, elements: !14)
!14 = !{!15, !19, !22, !23, !24, !25, !26}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "queue_mapping", scope: !13, file: !1, line: 4, baseType: !16, size: 16)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !17, line: 43, baseType: !18)
!17 = !DIFile(filename: "/usr/include/i386/_types.h", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!18 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "cloned", scope: !13, file: !1, line: 5, baseType: !20, size: 1, offset: 16, flags: DIFlagBitField, extraData: i64 16)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !17, line: 41, baseType: !21)
!21 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "nohdr", scope: !13, file: !1, line: 6, baseType: !20, size: 1, offset: 17, flags: DIFlagBitField, extraData: i64 16)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "fclone", scope: !13, file: !1, line: 7, baseType: !20, size: 2, offset: 18, flags: DIFlagBitField, extraData: i64 16)
!24 = !DIDerivedType(tag: DW_TAG_member, name: "peeked", scope: !13, file: !1, line: 8, baseType: !20, size: 1, offset: 20, flags: DIFlagBitField, extraData: i64 16)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "head_frag", scope: !13, file: !1, line: 9, baseType: !20, size: 1, offset: 21, flags: DIFlagBitField, extraData: i64 16)
!26 = !DIDerivedType(tag: DW_TAG_member, name: "xmit_more", scope: !13, file: !1, line: 10, baseType: !20, size: 1, offset: 22, flags: DIFlagBitField, extraData: i64 16)
!27 = !DIExpression()
!28 = !DILocation(line: 14, column: 14, scope: !8)
!29 = !DILocation(line: 15, column: 7, scope: !8)
!30 = !DILocation(line: 15, column: 14, scope: !8)
!31 = !DILocation(line: 16, column: 7, scope: !8)
!32 = !DILocation(line: 16, column: 14, scope: !8)
!33 = !DILocation(line: 17, column: 5, scope: !8)
