; ModuleID = 'test_loading_inst.c'
source_filename = "test_loading_inst.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.clothes = type { i32, i8* }
%struct.Person = type { i32, %struct.clothes* }

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"red\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { i32 10, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0) }, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.Person*) #0 !dbg !8 {
  %2 = alloca %struct.Person*, align 8
  store %struct.Person* %0, %struct.Person** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.Person** %2, metadata !26, metadata !27), !dbg !28
  %3 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !29
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %3, i32 0, i32 1, !dbg !30
  %5 = load %struct.clothes*, %struct.clothes** %4, align 8, !dbg !30
  %6 = getelementptr inbounds %struct.clothes, %struct.clothes* %5, i32 0, i32 1, !dbg !31
  %7 = load i8*, i8** %6, align 8, !dbg !31
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i8* %7), !dbg !32
  %9 = load %struct.Person*, %struct.Person** %2, align 8, !dbg !33
  %10 = getelementptr inbounds %struct.Person, %struct.Person* %9, i32 0, i32 0, !dbg !34
  %11 = load i32, i32* %10, align 8, !dbg !34
  ret i32 %11, !dbg !35
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @ff(i32) #0 !dbg !36 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !39, metadata !27), !dbg !40
  %3 = load i32, i32* %2, align 4, !dbg !41
  ret i32 %3, !dbg !42
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !43 {
  %1 = alloca %struct.clothes, align 8
  %2 = alloca %struct.Person, align 8
  call void @llvm.dbg.declare(metadata %struct.clothes* %1, metadata !46, metadata !27), !dbg !47
  %3 = bitcast %struct.clothes* %1 to i8*, !dbg !47
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.clothes* @main.c to i8*), i64 16, i32 8, i1 false), !dbg !47
  call void @llvm.dbg.declare(metadata %struct.Person* %2, metadata !48, metadata !27), !dbg !49
  %4 = getelementptr inbounds %struct.Person, %struct.Person* %2, i32 0, i32 0, !dbg !50
  store i32 21, i32* %4, align 8, !dbg !50
  %5 = getelementptr inbounds %struct.Person, %struct.Person* %2, i32 0, i32 1, !dbg !50
  store %struct.clothes* %1, %struct.clothes** %5, align 8, !dbg !50
  %6 = call i32 @f(%struct.Person* %2), !dbg !51
  %7 = call i32 @ff(i32 5), !dbg !52
  ret i32 0, !dbg !53
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
!1 = !DIFile(filename: "test_loading_inst.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 13, type: !9, isLocal: false, isDefinition: true, scopeLine: 13, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!13 = !DIDerivedType(tag: DW_TAG_typedef, name: "person_t", file: !1, line: 11, baseType: !14)
!14 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Person", file: !1, line: 8, size: 128, elements: !15)
!15 = !{!16, !17}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !14, file: !1, line: 9, baseType: !11, size: 32)
!17 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !14, file: !1, line: 10, baseType: !18, size: 64, offset: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIDerivedType(tag: DW_TAG_typedef, name: "clothes_t", file: !1, line: 6, baseType: !20)
!20 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "clothes", file: !1, line: 3, size: 128, elements: !21)
!21 = !{!22, !23}
!22 = !DIDerivedType(tag: DW_TAG_member, name: "length", scope: !20, file: !1, line: 4, baseType: !11, size: 32)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "color", scope: !20, file: !1, line: 5, baseType: !24, size: 64, offset: 64)
!24 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !25, size: 64)
!25 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!26 = !DILocalVariable(name: "p", arg: 1, scope: !8, file: !1, line: 13, type: !12)
!27 = !DIExpression()
!28 = !DILocation(line: 13, column: 17, scope: !8)
!29 = !DILocation(line: 14, column: 20, scope: !8)
!30 = !DILocation(line: 14, column: 23, scope: !8)
!31 = !DILocation(line: 14, column: 26, scope: !8)
!32 = !DILocation(line: 14, column: 5, scope: !8)
!33 = !DILocation(line: 15, column: 12, scope: !8)
!34 = !DILocation(line: 15, column: 15, scope: !8)
!35 = !DILocation(line: 15, column: 5, scope: !8)
!36 = distinct !DISubprogram(name: "ff", scope: !1, file: !1, line: 18, type: !37, isLocal: false, isDefinition: true, scopeLine: 18, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!37 = !DISubroutineType(types: !38)
!38 = !{!11, !11}
!39 = !DILocalVariable(name: "a", arg: 1, scope: !36, file: !1, line: 18, type: !11)
!40 = !DILocation(line: 18, column: 13, scope: !36)
!41 = !DILocation(line: 19, column: 12, scope: !36)
!42 = !DILocation(line: 19, column: 5, scope: !36)
!43 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 22, type: !44, isLocal: false, isDefinition: true, scopeLine: 22, isOptimized: false, unit: !0, variables: !2)
!44 = !DISubroutineType(types: !45)
!45 = !{!11}
!46 = !DILocalVariable(name: "c", scope: !43, file: !1, line: 23, type: !19)
!47 = !DILocation(line: 23, column: 15, scope: !43)
!48 = !DILocalVariable(name: "p", scope: !43, file: !1, line: 24, type: !13)
!49 = !DILocation(line: 24, column: 14, scope: !43)
!50 = !DILocation(line: 24, column: 18, scope: !43)
!51 = !DILocation(line: 25, column: 5, scope: !43)
!52 = !DILocation(line: 26, column: 5, scope: !43)
!53 = !DILocation(line: 27, column: 1, scope: !43)
