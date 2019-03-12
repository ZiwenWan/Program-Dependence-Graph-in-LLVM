; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, i8* }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"J\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i32 0, i32 0) }, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 !dbg !8 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !19, metadata !20), !dbg !21
  %3 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !22
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %3, i32 0, i32 0, !dbg !23
  %5 = load i32, i32* %4, align 8, !dbg !23
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %5), !dbg !24
  ret i32 0, !dbg !25
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !26 {
  %1 = alloca %struct.Person, align 8
  call void @llvm.dbg.declare(metadata %struct.Person* %1, metadata !29, metadata !20), !dbg !30
  %2 = bitcast %struct.Person* %1 to i8*, !dbg !30
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* bitcast (%struct.Person* @main.p to i8*), i64 16, i32 8, i1 false), !dbg !30
  %3 = getelementptr inbounds %struct.Person, %struct.Person* %1, i32 0, i32 0, !dbg !31
  %4 = load i32, i32* %3, align 8, !dbg !31
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4), !dbg !32
  ret i32 0, !dbg !33
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #3

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }

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
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 7, type: !9, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !1, line: 2, size: 128, elements: !14)
!14 = !{!15, !16}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !13, file: !1, line: 3, baseType: !11, size: 32)
!16 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !13, file: !1, line: 4, baseType: !17, size: 64, offset: 64)
!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!18 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!19 = !DILocalVariable(name: "p", arg: 1, scope: !8, file: !1, line: 7, type: !12)
!20 = !DIExpression()
!21 = !DILocation(line: 7, column: 22, scope: !8)
!22 = !DILocation(line: 8, column: 20, scope: !8)
!23 = !DILocation(line: 8, column: 23, scope: !8)
!24 = !DILocation(line: 8, column: 5, scope: !8)
!25 = !DILocation(line: 9, column: 5, scope: !8)
!26 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 12, type: !27, isLocal: false, isDefinition: true, scopeLine: 12, isOptimized: false, unit: !0, variables: !2)
!27 = !DISubroutineType(types: !28)
!28 = !{!11}
!29 = !DILocalVariable(name: "p", scope: !26, file: !1, line: 13, type: !13)
!30 = !DILocation(line: 13, column: 19, scope: !26)
!31 = !DILocation(line: 14, column: 22, scope: !26)
!32 = !DILocation(line: 14, column: 5, scope: !26)
!33 = !DILocation(line: 15, column: 1, scope: !26)
