; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, i8*, %union.anon }
%union.anon = type { i32 }

@.str = private unnamed_addr constant [2 x i8] c"J\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i32 0, i32 0), %union.anon zeroinitializer }, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 !dbg !8 {
  %2 = alloca %struct.Person*, align 8
  %3 = alloca %struct.Person, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !24, metadata !25), !dbg !26
  call void @llvm.dbg.declare(metadata %struct.Person* %3, metadata !27, metadata !25), !dbg !28
  %4 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !29
  %5 = bitcast %struct.Person* %3 to i8*, !dbg !30
  %6 = bitcast %struct.Person* %4 to i8*, !dbg !30
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %6, i64 24, i32 8, i1 false), !dbg !30
  ret i32 0, !dbg !31
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !32 {
  %1 = alloca %struct.Person, align 8
  call void @llvm.dbg.declare(metadata %struct.Person* %1, metadata !35, metadata !25), !dbg !36
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !36
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 24, i32 8, i1 false), !dbg !36
  %3 = call i32 @f(%struct.Person* %1), !dbg !37
  ret i32 0, !dbg !38
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 12, type: !9, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !1, line: 2, size: 192, elements: !14)
!14 = !{!15, !16, !19}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !13, file: !1, line: 3, baseType: !11, size: 32)
!16 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !13, file: !1, line: 4, baseType: !17, size: 64, offset: 64)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!18 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!19 = !DIDerivedType(tag: DW_TAG_member, scope: !13, file: !1, line: 5, baseType: !20, size: 32, offset: 128)
!20 = distinct !DICompositeType(tag: DW_TAG_union_type, scope: !13, file: !1, line: 5, size: 32, elements: !21)
!21 = !{!22, !23}
!22 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !20, file: !1, line: 7, baseType: !11, size: 32)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !20, file: !1, line: 8, baseType: !11, size: 32)
!24 = !DILocalVariable(name: "p", arg: 1, scope: !8, file: !1, line: 12, type: !12)
!25 = !DIExpression()
!26 = !DILocation(line: 12, column: 22, scope: !8)
!27 = !DILocalVariable(name: "p1", scope: !8, file: !1, line: 13, type: !13)
!28 = !DILocation(line: 13, column: 19, scope: !8)
!29 = !DILocation(line: 13, column: 25, scope: !8)
!30 = !DILocation(line: 13, column: 24, scope: !8)
!31 = !DILocation(line: 14, column: 5, scope: !8)
!32 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 17, type: !33, isLocal: false, isDefinition: true, scopeLine: 17, isOptimized: false, unit: !0, variables: !2)
!33 = !DISubroutineType(types: !34)
!34 = !{!11}
!35 = !DILocalVariable(name: "p", scope: !32, file: !1, line: 18, type: !13)
!36 = !DILocation(line: 18, column: 19, scope: !32)
!37 = !DILocation(line: 19, column: 5, scope: !32)
!38 = !DILocation(line: 20, column: 1, scope: !32)
