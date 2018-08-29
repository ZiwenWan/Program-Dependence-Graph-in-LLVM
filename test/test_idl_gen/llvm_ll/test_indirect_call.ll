; ModuleID = 'test_indirect_call.c'
source_filename = "test_indirect_call.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

%struct.list = type { i32 }

@Global = global i32 10, align 4, !dbg !0

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @fun2(i32*) #0 !dbg !12 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !16, metadata !DIExpression()), !dbg !17
  %3 = load i32, i32* @Global, align 4, !dbg !18
  %4 = sub nsw i32 %3, 1, !dbg !19
  %5 = load i32*, i32** %2, align 8, !dbg !20
  %6 = load i32, i32* %5, align 4, !dbg !21
  %7 = add nsw i32 %6, %4, !dbg !21
  store i32 %7, i32* %5, align 4, !dbg !21
  ret void, !dbg !22
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @fun(i32*) #0 !dbg !23 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !24, metadata !DIExpression()), !dbg !25
  %3 = load i32, i32* @Global, align 4, !dbg !26
  %4 = load i32*, i32** %2, align 8, !dbg !27
  %5 = load i32, i32* %4, align 4, !dbg !28
  %6 = add nsw i32 %5, %3, !dbg !28
  store i32 %6, i32* %4, align 4, !dbg !28
  ret void, !dbg !29
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @accessF(%struct.list*, void (i32*)*) #0 !dbg !30 {
  %3 = alloca %struct.list*, align 8
  %4 = alloca void (i32*)*, align 8
  store %struct.list* %0, %struct.list** %3, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %3, metadata !38, metadata !DIExpression()), !dbg !39
  store void (i32*)* %1, void (i32*)** %4, align 8
  call void @llvm.dbg.declare(metadata void (i32*)** %4, metadata !40, metadata !DIExpression()), !dbg !41
  %5 = load void (i32*)*, void (i32*)** %4, align 8, !dbg !42
  %6 = load %struct.list*, %struct.list** %3, align 8, !dbg !43
  %7 = getelementptr inbounds %struct.list, %struct.list* %6, i32 0, i32 0, !dbg !44
  call void %5(i32* %7), !dbg !42
  ret void, !dbg !45
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @passF(%struct.list*) #0 !dbg !46 {
  %2 = alloca %struct.list*, align 8
  store %struct.list* %0, %struct.list** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %2, metadata !49, metadata !DIExpression()), !dbg !50
  %3 = load %struct.list*, %struct.list** %2, align 8, !dbg !51
  call void @accessF(%struct.list* %3, void (i32*)* @fun), !dbg !52
  %4 = load %struct.list*, %struct.list** %2, align 8, !dbg !53
  call void @accessF(%struct.list* %4, void (i32*)* @fun2), !dbg !54
  ret void, !dbg !55
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !56 {
  %1 = alloca %struct.list*, align 8
  %2 = alloca %struct.list*, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %1, metadata !59, metadata !DIExpression()), !dbg !60
  %3 = call i8* @malloc(i64 4) #3, !dbg !61
  %4 = bitcast i8* %3 to %struct.list*, !dbg !61
  store %struct.list* %4, %struct.list** %1, align 8, !dbg !60
  call void @llvm.dbg.declare(metadata %struct.list** %2, metadata !62, metadata !DIExpression()), !dbg !63
  %5 = alloca i8, i64 4, align 16, !dbg !64
  %6 = bitcast i8* %5 to %struct.list*, !dbg !64
  store %struct.list* %6, %struct.list** %2, align 8, !dbg !63
  %7 = load %struct.list*, %struct.list** %1, align 8, !dbg !65
  call void @passF(%struct.list* %7), !dbg !66
  ret i32 0, !dbg !67
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { allocsize(0) }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9, !10}
!llvm.ident = !{!11}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "Global", scope: !2, file: !3, line: 4, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "Apple LLVM version 9.1.0 (clang-902.0.39.2)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "test_indirect_call.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{i32 7, !"PIC Level", i32 2}
!11 = !{!"Apple LLVM version 9.1.0 (clang-902.0.39.2)"}
!12 = distinct !DISubprogram(name: "fun2", scope: !3, file: !3, line: 11, type: !13, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!13 = !DISubroutineType(types: !14)
!14 = !{null, !15}
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!16 = !DILocalVariable(name: "X", arg: 1, scope: !12, file: !3, line: 11, type: !15)
!17 = !DILocation(line: 11, column: 16, scope: !12)
!18 = !DILocation(line: 12, column: 13, scope: !12)
!19 = !DILocation(line: 12, column: 20, scope: !12)
!20 = !DILocation(line: 12, column: 7, scope: !12)
!21 = !DILocation(line: 12, column: 10, scope: !12)
!22 = !DILocation(line: 13, column: 1, scope: !12)
!23 = distinct !DISubprogram(name: "fun", scope: !3, file: !3, line: 15, type: !13, isLocal: false, isDefinition: true, scopeLine: 15, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!24 = !DILocalVariable(name: "X", arg: 1, scope: !23, file: !3, line: 15, type: !15)
!25 = !DILocation(line: 15, column: 15, scope: !23)
!26 = !DILocation(line: 17, column: 11, scope: !23)
!27 = !DILocation(line: 17, column: 7, scope: !23)
!28 = !DILocation(line: 17, column: 9, scope: !23)
!29 = !DILocation(line: 23, column: 1, scope: !23)
!30 = distinct !DISubprogram(name: "accessF", scope: !3, file: !3, line: 25, type: !31, isLocal: false, isDefinition: true, scopeLine: 25, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!31 = !DISubroutineType(types: !32)
!32 = !{null, !33, !37}
!33 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !34, size: 64)
!34 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "list", file: !3, line: 6, size: 32, elements: !35)
!35 = !{!36}
!36 = !DIDerivedType(tag: DW_TAG_member, name: "Data", scope: !34, file: !3, line: 8, baseType: !6, size: 32)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!38 = !DILocalVariable(name: "L", arg: 1, scope: !30, file: !3, line: 25, type: !33)
!39 = !DILocation(line: 25, column: 28, scope: !30)
!40 = !DILocalVariable(name: "FP", arg: 2, scope: !30, file: !3, line: 25, type: !37)
!41 = !DILocation(line: 25, column: 38, scope: !30)
!42 = !DILocation(line: 28, column: 10, scope: !30)
!43 = !DILocation(line: 28, column: 14, scope: !30)
!44 = !DILocation(line: 28, column: 17, scope: !30)
!45 = !DILocation(line: 29, column: 2, scope: !30)
!46 = distinct !DISubprogram(name: "passF", scope: !3, file: !3, line: 31, type: !47, isLocal: false, isDefinition: true, scopeLine: 31, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!47 = !DISubroutineType(types: !48)
!48 = !{null, !33}
!49 = !DILocalVariable(name: "L", arg: 1, scope: !46, file: !3, line: 31, type: !33)
!50 = !DILocation(line: 31, column: 26, scope: !46)
!51 = !DILocation(line: 33, column: 14, scope: !46)
!52 = !DILocation(line: 33, column: 6, scope: !46)
!53 = !DILocation(line: 34, column: 14, scope: !46)
!54 = !DILocation(line: 34, column: 6, scope: !46)
!55 = !DILocation(line: 35, column: 2, scope: !46)
!56 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 37, type: !57, isLocal: false, isDefinition: true, scopeLine: 37, isOptimized: false, unit: !2, variables: !4)
!57 = !DISubroutineType(types: !58)
!58 = !{!6}
!59 = !DILocalVariable(name: "X", scope: !56, file: !3, line: 38, type: !33)
!60 = !DILocation(line: 38, column: 18, scope: !56)
!61 = !DILocation(line: 38, column: 22, scope: !56)
!62 = !DILocalVariable(name: "Y", scope: !56, file: !3, line: 39, type: !33)
!63 = !DILocation(line: 39, column: 18, scope: !56)
!64 = !DILocation(line: 39, column: 22, scope: !56)
!65 = !DILocation(line: 40, column: 11, scope: !56)
!66 = !DILocation(line: 40, column: 5, scope: !56)
!67 = !DILocation(line: 41, column: 1, scope: !56)
