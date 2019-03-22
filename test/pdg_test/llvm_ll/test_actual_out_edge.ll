; ModuleID = 'test_actual_out_edge.c'
source_filename = "test_actual_out_edge.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.Person = type { i32, i8* }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [5 x i8] c"Jack\00", align 1
@main.pp = private unnamed_addr constant %struct.Person { i32 15, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0) }, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f(i32) #0 !dbg !8 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !12, metadata !13), !dbg !14
  %3 = load i32, i32* %2, align 4, !dbg !15
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %3), !dbg !16
  ret void, !dbg !17
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !18 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.Person*, align 8
  %3 = alloca %struct.Person, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !21, metadata !13), !dbg !30
  call void @llvm.dbg.declare(metadata %struct.Person* %3, metadata !31, metadata !13), !dbg !32
  %7 = bitcast %struct.Person* %3 to i8*, !dbg !32
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* bitcast (%struct.Person* @main.pp to i8*), i64 16, i32 8, i1 false), !dbg !32
  store %struct.Person* %3, %struct.Person** %2, align 8, !dbg !33
  call void @llvm.dbg.declare(metadata i32* %4, metadata !34, metadata !13), !dbg !35
  %8 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !36
  %9 = getelementptr inbounds %struct.Person, %struct.Person* %8, i32 0, i32 0, !dbg !37
  %10 = load i32, i32* %9, align 8, !dbg !37
  store i32 %10, i32* %4, align 4, !dbg !35
  %11 = load i32, i32* %4, align 4, !dbg !38
  call void @f(i32 %11), !dbg !39
  call void @llvm.dbg.declare(metadata i32* %5, metadata !40, metadata !13), !dbg !41
  %12 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !42
  %13 = getelementptr inbounds %struct.Person, %struct.Person* %12, i32 0, i32 0, !dbg !43
  %14 = load i32, i32* %13, align 8, !dbg !43
  store i32 %14, i32* %5, align 4, !dbg !41
  %15 = load i32, i32* %5, align 4, !dbg !44
  call void @f(i32 %15), !dbg !45
  call void @llvm.dbg.declare(metadata i32* %6, metadata !46, metadata !13), !dbg !47
  %16 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !48
  %17 = getelementptr inbounds %struct.Person, %struct.Person* %16, i32 0, i32 0, !dbg !49
  %18 = load i32, i32* %17, align 8, !dbg !49
  store i32 %18, i32* %6, align 4, !dbg !47
  ret i32 1, !dbg !50
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
!1 = !DIFile(filename: "test_actual_out_edge.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 7, type: !9, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DILocalVariable(name: "a", arg: 1, scope: !8, file: !1, line: 7, type: !11)
!13 = !DIExpression()
!14 = !DILocation(line: 7, column: 12, scope: !8)
!15 = !DILocation(line: 8, column: 20, scope: !8)
!16 = !DILocation(line: 8, column: 5, scope: !8)
!17 = !DILocation(line: 9, column: 1, scope: !8)
!18 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, type: !19, isLocal: false, isDefinition: true, scopeLine: 11, isOptimized: false, unit: !0, variables: !2)
!19 = !DISubroutineType(types: !20)
!20 = !{!11}
!21 = !DILocalVariable(name: "p1", scope: !18, file: !1, line: 12, type: !22)
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !23, size: 64)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "person_t", file: !1, line: 5, baseType: !24)
!24 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !1, line: 2, size: 128, elements: !25)
!25 = !{!26, !27}
!26 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !24, file: !1, line: 3, baseType: !11, size: 32)
!27 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !24, file: !1, line: 4, baseType: !28, size: 64, offset: 64)
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !29, size: 64)
!29 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!30 = !DILocation(line: 12, column: 15, scope: !18)
!31 = !DILocalVariable(name: "pp", scope: !18, file: !1, line: 13, type: !24)
!32 = !DILocation(line: 13, column: 19, scope: !18)
!33 = !DILocation(line: 14, column: 8, scope: !18)
!34 = !DILocalVariable(name: "age", scope: !18, file: !1, line: 15, type: !11)
!35 = !DILocation(line: 15, column: 9, scope: !18)
!36 = !DILocation(line: 15, column: 15, scope: !18)
!37 = !DILocation(line: 15, column: 19, scope: !18)
!38 = !DILocation(line: 16, column: 7, scope: !18)
!39 = !DILocation(line: 16, column: 5, scope: !18)
!40 = !DILocalVariable(name: "age1", scope: !18, file: !1, line: 17, type: !11)
!41 = !DILocation(line: 17, column: 9, scope: !18)
!42 = !DILocation(line: 17, column: 16, scope: !18)
!43 = !DILocation(line: 17, column: 20, scope: !18)
!44 = !DILocation(line: 18, column: 7, scope: !18)
!45 = !DILocation(line: 18, column: 5, scope: !18)
!46 = !DILocalVariable(name: "age2", scope: !18, file: !1, line: 19, type: !11)
!47 = !DILocation(line: 19, column: 9, scope: !18)
!48 = !DILocation(line: 19, column: 16, scope: !18)
!49 = !DILocation(line: 19, column: 20, scope: !18)
!50 = !DILocation(line: 21, column: 5, scope: !18)
