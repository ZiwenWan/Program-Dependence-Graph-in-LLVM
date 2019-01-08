; ModuleID = 'test_complex_struct_passing.c'
source_filename = "test_complex_struct_passing.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.clothes = type { [10 x i8], i32 }
%struct.person_t = type { i32, [10 x i8], %struct.clothes* }

@.str = private unnamed_addr constant [24 x i8] c"%s is wearing %s today.\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { [10 x i8] c"red\00\00\00\00\00\00\00", i32 5 }, align 4
@.str.1 = private unnamed_addr constant [10 x i8] c"Jack\00\00\00\00\00\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f(%struct.person_t*) #0 !dbg !8 {
  %2 = alloca %struct.person_t*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca i8*, align 8
  store %struct.person_t* %0, %struct.person_t** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person_t** %2, metadata !29, metadata !30), !dbg !31
  call void @llvm.dbg.declare(metadata i8** %3, metadata !32, metadata !30), !dbg !34
  %5 = load %struct.person_t*, %struct.person_t** %2, align 8, !dbg !35
  %6 = getelementptr inbounds %struct.person_t, %struct.person_t* %5, i32 0, i32 1, !dbg !36
  %7 = getelementptr inbounds [10 x i8], [10 x i8]* %6, i32 0, i32 0, !dbg !35
  store i8* %7, i8** %3, align 8, !dbg !34
  call void @llvm.dbg.declare(metadata i8** %4, metadata !37, metadata !30), !dbg !38
  %8 = load %struct.person_t*, %struct.person_t** %2, align 8, !dbg !39
  %9 = getelementptr inbounds %struct.person_t, %struct.person_t* %8, i32 0, i32 2, !dbg !40
  %10 = load %struct.clothes*, %struct.clothes** %9, align 8, !dbg !40
  %11 = getelementptr inbounds %struct.clothes, %struct.clothes* %10, i32 0, i32 0, !dbg !41
  %12 = getelementptr inbounds [10 x i8], [10 x i8]* %11, i32 0, i32 0, !dbg !39
  store i8* %12, i8** %4, align 8, !dbg !38
  %13 = load i8*, i8** %3, align 8, !dbg !42
  %14 = load i8*, i8** %4, align 8, !dbg !43
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str, i32 0, i32 0), i8* %13, i8* %14), !dbg !44
  ret void, !dbg !45
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !46 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.clothes, align 4
  %3 = alloca %struct.person_t, align 8
  %4 = alloca %struct.person_t*, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.clothes* %2, metadata !49, metadata !30), !dbg !50
  %5 = bitcast %struct.clothes* %2 to i8*, !dbg !50
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* getelementptr inbounds (%struct.clothes, %struct.clothes* @main.c, i32 0, i32 0, i32 0), i64 16, i32 4, i1 false), !dbg !50
  call void @llvm.dbg.declare(metadata %struct.person_t* %3, metadata !51, metadata !30), !dbg !52
  %6 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 0, !dbg !53
  store i32 10, i32* %6, align 8, !dbg !53
  %7 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 1, !dbg !53
  %8 = bitcast [10 x i8]* %7 to i8*, !dbg !54
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %8, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0), i64 10, i32 1, i1 false), !dbg !54
  %9 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 2, !dbg !53
  store %struct.clothes* %2, %struct.clothes** %9, align 8, !dbg !53
  call void @llvm.dbg.declare(metadata %struct.person_t** %4, metadata !55, metadata !30), !dbg !56
  store %struct.person_t* %3, %struct.person_t** %4, align 8, !dbg !56
  %10 = load %struct.person_t*, %struct.person_t** %4, align 8, !dbg !57
  call void @f(%struct.person_t* %10), !dbg !58
  ret i32 0, !dbg !59
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
!1 = !DIFile(filename: "test_complex_struct_passing.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 14, type: !9, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 12, baseType: !13)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person_t", file: !1, line: 8, size: 192, elements: !14)
!14 = !{!15, !17, !22}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !13, file: !1, line: 9, baseType: !16, size: 32)
!16 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !13, file: !1, line: 10, baseType: !18, size: 80, offset: 32)
!18 = !DICompositeType(tag: DW_TAG_array_type, baseType: !19, size: 80, elements: !20)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !{!21}
!21 = !DISubrange(count: 10)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "s", scope: !13, file: !1, line: 11, baseType: !23, size: 64, offset: 128)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "Clothes", file: !1, line: 6, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "clothes", file: !1, line: 3, size: 128, elements: !26)
!26 = !{!27, !28}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "color", scope: !25, file: !1, line: 4, baseType: !18, size: 80)
!28 = !DIDerivedType(tag: DW_TAG_member, name: "length", scope: !25, file: !1, line: 5, baseType: !16, size: 32, offset: 96)
!29 = !DILocalVariable(name: "p1", arg: 1, scope: !8, file: !1, line: 14, type: !11)
!30 = !DIExpression()
!31 = !DILocation(line: 14, column: 16, scope: !8)
!32 = !DILocalVariable(name: "name", scope: !8, file: !1, line: 15, type: !33)
!33 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!34 = !DILocation(line: 15, column: 11, scope: !8)
!35 = !DILocation(line: 15, column: 18, scope: !8)
!36 = !DILocation(line: 15, column: 22, scope: !8)
!37 = !DILocalVariable(name: "color", scope: !8, file: !1, line: 16, type: !33)
!38 = !DILocation(line: 16, column: 11, scope: !8)
!39 = !DILocation(line: 16, column: 19, scope: !8)
!40 = !DILocation(line: 16, column: 23, scope: !8)
!41 = !DILocation(line: 16, column: 26, scope: !8)
!42 = !DILocation(line: 17, column: 39, scope: !8)
!43 = !DILocation(line: 17, column: 45, scope: !8)
!44 = !DILocation(line: 17, column: 5, scope: !8)
!45 = !DILocation(line: 18, column: 1, scope: !8)
!46 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 20, type: !47, isLocal: false, isDefinition: true, scopeLine: 20, isOptimized: false, unit: !0, variables: !2)
!47 = !DISubroutineType(types: !48)
!48 = !{!16}
!49 = !DILocalVariable(name: "c", scope: !46, file: !1, line: 21, type: !24)
!50 = !DILocation(line: 21, column: 13, scope: !46)
!51 = !DILocalVariable(name: "p", scope: !46, file: !1, line: 22, type: !12)
!52 = !DILocation(line: 22, column: 12, scope: !46)
!53 = !DILocation(line: 22, column: 16, scope: !46)
!54 = !DILocation(line: 22, column: 21, scope: !46)
!55 = !DILocalVariable(name: "pt", scope: !46, file: !1, line: 23, type: !11)
!56 = !DILocation(line: 23, column: 13, scope: !46)
!57 = !DILocation(line: 24, column: 7, scope: !46)
!58 = !DILocation(line: 24, column: 5, scope: !46)
!59 = !DILocation(line: 25, column: 5, scope: !46)
