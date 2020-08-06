; ModuleID = 'test_indirect_call.c'
source_filename = "test_indirect_call.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.A = type { void (i32*)*, i32 }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f1(i32*) #0 !dbg !8 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !13, metadata !14), !dbg !15
  %3 = load i32*, i32** %2, align 8, !dbg !16
  %4 = load i32, i32* %3, align 4, !dbg !17
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4), !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @f(%struct.A*) #0 !dbg !20 {
  %2 = alloca %struct.A*, align 8
  store %struct.A* %0, %struct.A** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.A** %2, metadata !29, metadata !14), !dbg !30
  %3 = load %struct.A*, %struct.A** %2, align 8, !dbg !31
  %4 = getelementptr inbounds %struct.A, %struct.A* %3, i32 0, i32 0, !dbg !32
  store void (i32*)* @f1, void (i32*)** %4, align 8, !dbg !33
  %5 = load %struct.A*, %struct.A** %2, align 8, !dbg !34
  %6 = getelementptr inbounds %struct.A, %struct.A* %5, i32 0, i32 0, !dbg !35
  %7 = load void (i32*)*, void (i32*)** %6, align 8, !dbg !35
  %8 = load %struct.A*, %struct.A** %2, align 8, !dbg !36
  %9 = getelementptr inbounds %struct.A, %struct.A* %8, i32 0, i32 1, !dbg !37
  call void %7(i32* %9), !dbg !38
  ret i32 0, !dbg !39
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !40 {
  ret i32 0, !dbg !43
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_indirect_call.c", directory: "/Users/yongzhehuang/Documents/llvm_versions/program-dependence-graph-in-llvm/test/pdg_test")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f1", scope: !1, file: !1, line: 7, type: !9, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocalVariable(name: "b", arg: 1, scope: !8, file: !1, line: 7, type: !11)
!14 = !DIExpression()
!15 = !DILocation(line: 7, column: 14, scope: !8)
!16 = !DILocation(line: 8, column: 21, scope: !8)
!17 = !DILocation(line: 8, column: 20, scope: !8)
!18 = !DILocation(line: 8, column: 5, scope: !8)
!19 = !DILocation(line: 9, column: 1, scope: !8)
!20 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 11, type: !21, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!21 = !DISubroutineType(types: !22)
!22 = !{!12, !23}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "A", file: !1, line: 2, size: 128, elements: !25)
!25 = !{!26, !28}
!26 = !DIDerivedType(tag: DW_TAG_member, name: "setup", scope: !24, file: !1, line: 3, baseType: !27, size: 64)
!27 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !9, size: 64)
!28 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !24, file: !1, line: 4, baseType: !12, size: 32, offset: 64)
!29 = !DILocalVariable(name: "a", arg: 1, scope: !20, file: !1, line: 11, type: !23)
!30 = !DILocation(line: 11, column: 17, scope: !20)
!31 = !DILocation(line: 12, column: 5, scope: !20)
!32 = !DILocation(line: 12, column: 8, scope: !20)
!33 = !DILocation(line: 12, column: 14, scope: !20)
!34 = !DILocation(line: 13, column: 7, scope: !20)
!35 = !DILocation(line: 13, column: 10, scope: !20)
!36 = !DILocation(line: 13, column: 18, scope: !20)
!37 = !DILocation(line: 13, column: 21, scope: !20)
!38 = !DILocation(line: 13, column: 5, scope: !20)
!39 = !DILocation(line: 14, column: 5, scope: !20)
!40 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 18, type: !41, isLocal: false, isDefinition: true, scopeLine: 18, isOptimized: false, unit: !0, variables: !2)
!41 = !DISubroutineType(types: !42)
!42 = !{!12}
!43 = !DILocation(line: 20, column: 1, scope: !40)
