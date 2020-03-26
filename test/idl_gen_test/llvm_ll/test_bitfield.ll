; ModuleID = 'test_bitfield.c'
source_filename = "test_bitfield.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.pack = type { i8, i8 }

@__const.main.p = private unnamed_addr constant %struct.pack { i8 83, i8 11 }, align 2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.pack, align 2
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.pack* %2, metadata !12, metadata !DIExpression()), !dbg !25
  %3 = bitcast %struct.pack* %2 to i8*, !dbg !25
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 2 %3, i8* align 2 getelementptr inbounds (%struct.pack, %struct.pack* @__const.main.p, i32 0, i32 0), i64 2, i1 false), !dbg !25
  %4 = bitcast %struct.pack* %2 to i8*, !dbg !26
  %5 = load i8, i8* %4, align 2, !dbg !27
  %6 = and i8 %5, -97, !dbg !27
  %7 = or i8 %6, 96, !dbg !27
  store i8 %7, i8* %4, align 2, !dbg !27
  ret i32 0, !dbg !28
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1 immarg) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 9.0.0 (tags/RELEASE_900/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, nameTableKind: GNU)
!1 = !DIFile(filename: "test_bitfield.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 9.0.0 (tags/RELEASE_900/final)"}
!8 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, type: !9, scopeLine: 11, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DILocalVariable(name: "p", scope: !8, file: !1, line: 12, type: !13)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "pack", file: !1, line: 3, size: 16, elements: !14)
!14 = !{!15, !19, !22, !23, !24}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !13, file: !1, line: 4, baseType: !16, size: 4, flags: DIFlagBitField, extraData: i64 0)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !17, line: 43, baseType: !18)
!17 = !DIFile(filename: "/usr/include/i386/_types.h", directory: "")
!18 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!19 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !13, file: !1, line: 5, baseType: !20, size: 1, offset: 4, flags: DIFlagBitField, extraData: i64 0)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !17, line: 41, baseType: !21)
!21 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !13, file: !1, line: 6, baseType: !20, size: 2, offset: 5, flags: DIFlagBitField, extraData: i64 0)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "d", scope: !13, file: !1, line: 7, baseType: !20, size: 3, offset: 8, flags: DIFlagBitField, extraData: i64 8)
!24 = !DIDerivedType(tag: DW_TAG_member, name: "e", scope: !13, file: !1, line: 8, baseType: !20, size: 1, offset: 11, flags: DIFlagBitField, extraData: i64 8)
!25 = !DILocation(line: 12, column: 17, scope: !8)
!26 = !DILocation(line: 15, column: 7, scope: !8)
!27 = !DILocation(line: 15, column: 9, scope: !8)
!28 = !DILocation(line: 16, column: 5, scope: !8)
