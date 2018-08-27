; ModuleID = 'test_indirect_call.c'
source_filename = "test_indirect_call.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.list = type { i32 }

@Global = global i32 10, align 4, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define void @fun2(i32* %X) #0 !dbg !11 {
entry:
  %X.addr = alloca i32*, align 8
  store i32* %X, i32** %X.addr, align 8
  call void @llvm.dbg.declare(metadata i32** %X.addr, metadata !15, metadata !DIExpression()), !dbg !16
  %0 = load i32, i32* @Global, align 4, !dbg !17
  %sub = sub nsw i32 %0, 1, !dbg !18
  %1 = load i32*, i32** %X.addr, align 8, !dbg !19
  %2 = load i32, i32* %1, align 4, !dbg !20
  %add = add nsw i32 %2, %sub, !dbg !20
  store i32 %add, i32* %1, align 4, !dbg !20
  ret void, !dbg !21
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define void @fun(i32* %X) #0 !dbg !22 {
entry:
  %X.addr = alloca i32*, align 8
  store i32* %X, i32** %X.addr, align 8
  call void @llvm.dbg.declare(metadata i32** %X.addr, metadata !23, metadata !DIExpression()), !dbg !24
  %0 = load i32, i32* @Global, align 4, !dbg !25
  %1 = load i32*, i32** %X.addr, align 8, !dbg !26
  %2 = load i32, i32* %1, align 4, !dbg !27
  %add = add nsw i32 %2, %0, !dbg !27
  store i32 %add, i32* %1, align 4, !dbg !27
  ret void, !dbg !28
}

; Function Attrs: noinline nounwind optnone uwtable
define void @accessF(%struct.list* %L, void (i32*)* %FP) #0 !dbg !29 {
entry:
  %L.addr = alloca %struct.list*, align 8
  %FP.addr = alloca void (i32*)*, align 8
  store %struct.list* %L, %struct.list** %L.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %L.addr, metadata !37, metadata !DIExpression()), !dbg !38
  store void (i32*)* %FP, void (i32*)** %FP.addr, align 8
  call void @llvm.dbg.declare(metadata void (i32*)** %FP.addr, metadata !39, metadata !DIExpression()), !dbg !40
  %0 = load void (i32*)*, void (i32*)** %FP.addr, align 8, !dbg !41
  %1 = load %struct.list*, %struct.list** %L.addr, align 8, !dbg !42
  %Data = getelementptr inbounds %struct.list, %struct.list* %1, i32 0, i32 0, !dbg !43
  call void %0(i32* %Data), !dbg !41
  ret void, !dbg !44
}

; Function Attrs: noinline nounwind optnone uwtable
define void @passF(%struct.list* %L) #0 !dbg !45 {
entry:
  %L.addr = alloca %struct.list*, align 8
  store %struct.list* %L, %struct.list** %L.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %L.addr, metadata !48, metadata !DIExpression()), !dbg !49
  %0 = load %struct.list*, %struct.list** %L.addr, align 8, !dbg !50
  call void @accessF(%struct.list* %0, void (i32*)* @fun), !dbg !51
  %1 = load %struct.list*, %struct.list** %L.addr, align 8, !dbg !52
  call void @accessF(%struct.list* %1, void (i32*)* @fun2), !dbg !53
  ret void, !dbg !54
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !55 {
entry:
  %X = alloca %struct.list*, align 8
  %Y = alloca %struct.list*, align 8
  call void @llvm.dbg.declare(metadata %struct.list** %X, metadata !58, metadata !DIExpression()), !dbg !59
  %call = call noalias i8* @malloc(i64 4) #3, !dbg !60
  %0 = bitcast i8* %call to %struct.list*, !dbg !60
  store %struct.list* %0, %struct.list** %X, align 8, !dbg !59
  call void @llvm.dbg.declare(metadata %struct.list** %Y, metadata !61, metadata !DIExpression()), !dbg !62
  %1 = alloca i8, i64 4, align 16, !dbg !63
  %2 = bitcast i8* %1 to %struct.list*, !dbg !63
  store %struct.list* %2, %struct.list** %Y, align 8, !dbg !62
  %3 = load %struct.list*, %struct.list** %X, align 8, !dbg !64
  call void @passF(%struct.list* %3), !dbg !65
  ret i32 0, !dbg !66
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9}
!llvm.ident = !{!10}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "Global", scope: !2, file: !3, line: 4, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5)
!3 = !DIFile(filename: "test_indirect_call.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/CDG/test/test_idl_gen")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!11 = distinct !DISubprogram(name: "fun2", scope: !3, file: !3, line: 11, type: !12, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!12 = !DISubroutineType(types: !13)
!13 = !{null, !14}
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!15 = !DILocalVariable(name: "X", arg: 1, scope: !11, file: !3, line: 11, type: !14)
!16 = !DILocation(line: 11, column: 16, scope: !11)
!17 = !DILocation(line: 12, column: 13, scope: !11)
!18 = !DILocation(line: 12, column: 20, scope: !11)
!19 = !DILocation(line: 12, column: 7, scope: !11)
!20 = !DILocation(line: 12, column: 10, scope: !11)
!21 = !DILocation(line: 13, column: 1, scope: !11)
!22 = distinct !DISubprogram(name: "fun", scope: !3, file: !3, line: 15, type: !12, isLocal: false, isDefinition: true, scopeLine: 15, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!23 = !DILocalVariable(name: "X", arg: 1, scope: !22, file: !3, line: 15, type: !14)
!24 = !DILocation(line: 15, column: 15, scope: !22)
!25 = !DILocation(line: 17, column: 11, scope: !22)
!26 = !DILocation(line: 17, column: 7, scope: !22)
!27 = !DILocation(line: 17, column: 9, scope: !22)
!28 = !DILocation(line: 23, column: 1, scope: !22)
!29 = distinct !DISubprogram(name: "accessF", scope: !3, file: !3, line: 25, type: !30, isLocal: false, isDefinition: true, scopeLine: 25, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!30 = !DISubroutineType(types: !31)
!31 = !{null, !32, !36}
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64)
!33 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "list", file: !3, line: 6, size: 32, elements: !34)
!34 = !{!35}
!35 = !DIDerivedType(tag: DW_TAG_member, name: "Data", scope: !33, file: !3, line: 8, baseType: !6, size: 32)
!36 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!37 = !DILocalVariable(name: "L", arg: 1, scope: !29, file: !3, line: 25, type: !32)
!38 = !DILocation(line: 25, column: 28, scope: !29)
!39 = !DILocalVariable(name: "FP", arg: 2, scope: !29, file: !3, line: 25, type: !36)
!40 = !DILocation(line: 25, column: 38, scope: !29)
!41 = !DILocation(line: 28, column: 10, scope: !29)
!42 = !DILocation(line: 28, column: 14, scope: !29)
!43 = !DILocation(line: 28, column: 17, scope: !29)
!44 = !DILocation(line: 29, column: 2, scope: !29)
!45 = distinct !DISubprogram(name: "passF", scope: !3, file: !3, line: 31, type: !46, isLocal: false, isDefinition: true, scopeLine: 31, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!46 = !DISubroutineType(types: !47)
!47 = !{null, !32}
!48 = !DILocalVariable(name: "L", arg: 1, scope: !45, file: !3, line: 31, type: !32)
!49 = !DILocation(line: 31, column: 26, scope: !45)
!50 = !DILocation(line: 33, column: 14, scope: !45)
!51 = !DILocation(line: 33, column: 6, scope: !45)
!52 = !DILocation(line: 34, column: 14, scope: !45)
!53 = !DILocation(line: 34, column: 6, scope: !45)
!54 = !DILocation(line: 35, column: 2, scope: !45)
!55 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 37, type: !56, isLocal: false, isDefinition: true, scopeLine: 37, isOptimized: false, unit: !2, variables: !4)
!56 = !DISubroutineType(types: !57)
!57 = !{!6}
!58 = !DILocalVariable(name: "X", scope: !55, file: !3, line: 38, type: !32)
!59 = !DILocation(line: 38, column: 18, scope: !55)
!60 = !DILocation(line: 38, column: 22, scope: !55)
!61 = !DILocalVariable(name: "Y", scope: !55, file: !3, line: 39, type: !32)
!62 = !DILocation(line: 39, column: 18, scope: !55)
!63 = !DILocation(line: 39, column: 22, scope: !55)
!64 = !DILocation(line: 40, column: 11, scope: !55)
!65 = !DILocation(line: 40, column: 5, scope: !55)
!66 = !DILocation(line: 41, column: 1, scope: !55)
