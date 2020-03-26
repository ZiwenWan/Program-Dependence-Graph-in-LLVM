; ModuleID = 'example.k4.c'
source_filename = "example.k4.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.node = type { %struct.device*, %struct.node* }
%struct.device = type { void (%struct.device*)*, i8*, i32 }

@device_list_head = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [11 x i8] c"opened %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"Add device to the list:\0A\00", align 1
@dev1 = internal global %struct.device { void (%struct.device*)* @dev1_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.2, i32 0, i32 0), i32 0 }, align 8, !dbg !25
@dev2 = internal global %struct.device { void (%struct.device*)* @dev2_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.3, i32 0, i32 0), i32 0 }, align 8, !dbg !27
@.str.2 = private unnamed_addr constant [9 x i8] c"device_1\00", align 1
@.str.3 = private unnamed_addr constant [9 x i8] c"device_2\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @init(%struct.node*) #0 !dbg !34 {
  %2 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %2, metadata !37, metadata !DIExpression()), !dbg !38
  store %struct.node* null, %struct.node** %2, align 8, !dbg !39
  ret void, !dbg !40
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @register_device(%struct.device*) #0 !dbg !41 {
  %2 = alloca %struct.device*, align 8
  %3 = alloca %struct.node*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !42, metadata !DIExpression()), !dbg !43
  call void @llvm.dbg.declare(metadata %struct.node** %3, metadata !44, metadata !DIExpression()), !dbg !45
  %4 = call i8* @malloc(i64 16) #4, !dbg !46
  %5 = bitcast i8* %4 to %struct.node*, !dbg !47
  store %struct.node* %5, %struct.node** %3, align 8, !dbg !45
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !48
  %7 = load %struct.node*, %struct.node** %3, align 8, !dbg !49
  %8 = getelementptr inbounds %struct.node, %struct.node* %7, i32 0, i32 0, !dbg !50
  store %struct.device* %6, %struct.device** %8, align 8, !dbg !51
  %9 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !52
  %10 = load %struct.node*, %struct.node** %3, align 8, !dbg !53
  %11 = getelementptr inbounds %struct.node, %struct.node* %10, i32 0, i32 1, !dbg !54
  store %struct.node* %9, %struct.node** %11, align 8, !dbg !55
  %12 = load %struct.node*, %struct.node** %3, align 8, !dbg !56
  store %struct.node* %12, %struct.node** @device_list_head, align 8, !dbg !57
  ret void, !dbg !58
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @open(%struct.device*) #0 !dbg !59 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !62, metadata !DIExpression()), !dbg !63
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !64
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 0, !dbg !65
  %5 = load void (%struct.device*)*, void (%struct.device*)** %4, align 8, !dbg !65
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !66
  call void %5(%struct.device* %6), !dbg !64
  %7 = load %struct.device*, %struct.device** %2, align 8, !dbg !67
  %8 = getelementptr inbounds %struct.device, %struct.device* %7, i32 0, i32 1, !dbg !68
  %9 = load i8*, i8** %8, align 8, !dbg !68
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i64 0, i64 0), i8* %9), !dbg !69
  ret i32 1, !dbg !70
}

declare i32 @printf(i8*, ...) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @device_reset(i32*) #0 !dbg !71 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !75, metadata !DIExpression()), !dbg !76
  %3 = load i32*, i32** %2, align 8, !dbg !77
  store i32 0, i32* %3, align 4, !dbg !78
  ret void, !dbg !79
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.device* @lookup_device(i8*) #0 !dbg !80 {
  %2 = alloca %struct.device*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca %struct.node*, align 8
  store i8* %0, i8** %3, align 8
  call void @llvm.dbg.declare(metadata i8** %3, metadata !83, metadata !DIExpression()), !dbg !84
  call void @llvm.dbg.declare(metadata %struct.node** %4, metadata !85, metadata !DIExpression()), !dbg !86
  %5 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !87
  store %struct.node* %5, %struct.node** %4, align 8, !dbg !86
  br label %6, !dbg !88

6:                                                ; preds = %26, %1
  %7 = load %struct.node*, %struct.node** %4, align 8, !dbg !89
  %8 = icmp ne %struct.node* %7, null, !dbg !90
  br i1 %8, label %9, label %27, !dbg !88

9:                                                ; preds = %6
  %10 = load %struct.node*, %struct.node** %4, align 8, !dbg !91
  %11 = getelementptr inbounds %struct.node, %struct.node* %10, i32 0, i32 0, !dbg !94
  %12 = load %struct.device*, %struct.device** %11, align 8, !dbg !94
  %13 = getelementptr inbounds %struct.device, %struct.device* %12, i32 0, i32 1, !dbg !95
  %14 = load i8*, i8** %13, align 8, !dbg !95
  %15 = load i8*, i8** %3, align 8, !dbg !96
  %16 = call i32 @strcmp(i8* %14, i8* %15), !dbg !97
  %17 = icmp eq i32 %16, 0, !dbg !98
  br i1 %17, label %18, label %22, !dbg !99

18:                                               ; preds = %9
  %19 = load %struct.node*, %struct.node** %4, align 8, !dbg !100
  %20 = getelementptr inbounds %struct.node, %struct.node* %19, i32 0, i32 0, !dbg !101
  %21 = load %struct.device*, %struct.device** %20, align 8, !dbg !101
  store %struct.device* %21, %struct.device** %2, align 8, !dbg !102
  br label %28, !dbg !102

22:                                               ; preds = %9
  %23 = load %struct.node*, %struct.node** %4, align 8, !dbg !103
  %24 = getelementptr inbounds %struct.node, %struct.node* %23, i32 0, i32 1, !dbg !104
  %25 = load %struct.node*, %struct.node** %24, align 8, !dbg !104
  store %struct.node* %25, %struct.node** %4, align 8, !dbg !105
  br label %26

26:                                               ; preds = %22
  br label %6, !dbg !88, !llvm.loop !106

27:                                               ; preds = %6
  store %struct.device* null, %struct.device** %2, align 8, !dbg !108
  br label %28, !dbg !108

28:                                               ; preds = %27, %18
  %29 = load %struct.device*, %struct.device** %2, align 8, !dbg !109
  ret %struct.device* %29, !dbg !109
}

declare i32 @strcmp(i8*, i8*) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !110 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.device*, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !113, metadata !DIExpression()), !dbg !114
  %3 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !115
  call void @init(%struct.node* %3), !dbg !116
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str.1, i64 0, i64 0)), !dbg !117
  call void @register_device(%struct.device* @dev1), !dbg !118
  call void @register_device(%struct.device* @dev2), !dbg !119
  %5 = call %struct.device* @lookup_device(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.2, i64 0, i64 0)), !dbg !120
  store %struct.device* %5, %struct.device** %2, align 8, !dbg !121
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !122
  %7 = call i32 @open(%struct.device* %6), !dbg !123
  %8 = call %struct.device* @lookup_device(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.3, i64 0, i64 0)), !dbg !124
  store %struct.device* %8, %struct.device** %2, align 8, !dbg !125
  %9 = load %struct.device*, %struct.device** %2, align 8, !dbg !126
  %10 = call i32 @open(%struct.device* %9), !dbg !127
  ret i32 0, !dbg !128
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @dev1_open(%struct.device*) #0 !dbg !129 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !130, metadata !DIExpression()), !dbg !131
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !132
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 2, !dbg !133
  call void @device_reset(i32* %4), !dbg !134
  ret void, !dbg !135
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @dev2_open(%struct.device*) #0 !dbg !136 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !137, metadata !DIExpression()), !dbg !138
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !139
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 2, !dbg !140
  store i32 1, i32* %4, align 8, !dbg !141
  ret void, !dbg !142
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { allocsize(0) }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!29, !30, !31, !32}
!llvm.ident = !{!33}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "device_list_head", scope: !2, file: !3, line: 23, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 9.0.0 (tags/RELEASE_900/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !24, nameTableKind: GNU)
!3 = !DIFile(filename: "example.k4.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
!4 = !{}
!5 = !{!6, !23}
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "node", file: !3, line: 18, size: 128, elements: !8)
!8 = !{!9, !22}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "dev", scope: !7, file: !3, line: 19, baseType: !10, size: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "device", file: !3, line: 12, size: 192, elements: !12)
!12 = !{!13, !17, !20}
!13 = !DIDerivedType(tag: DW_TAG_member, name: "open", scope: !11, file: !3, line: 13, baseType: !14, size: 64)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DISubroutineType(types: !16)
!16 = !{null, !10}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !11, file: !3, line: 14, baseType: !18, size: 64, offset: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "devop_init_registered", scope: !11, file: !3, line: 15, baseType: !21, size: 32, offset: 128)
!21 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !7, file: !3, line: 20, baseType: !6, size: 64, offset: 64)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!24 = !{!0, !25, !27}
!25 = !DIGlobalVariableExpression(var: !26, expr: !DIExpression())
!26 = distinct !DIGlobalVariable(name: "dev1", scope: !2, file: !3, line: 77, type: !11, isLocal: true, isDefinition: true)
!27 = !DIGlobalVariableExpression(var: !28, expr: !DIExpression())
!28 = distinct !DIGlobalVariable(name: "dev2", scope: !2, file: !3, line: 88, type: !11, isLocal: true, isDefinition: true)
!29 = !{i32 2, !"Dwarf Version", i32 4}
!30 = !{i32 2, !"Debug Info Version", i32 3}
!31 = !{i32 1, !"wchar_size", i32 4}
!32 = !{i32 7, !"PIC Level", i32 2}
!33 = !{!"clang version 9.0.0 (tags/RELEASE_900/final)"}
!34 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 27, type: !35, scopeLine: 27, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!35 = !DISubroutineType(types: !36)
!36 = !{null, !6}
!37 = !DILocalVariable(name: "head", arg: 1, scope: !34, file: !3, line: 27, type: !6)
!38 = !DILocation(line: 27, column: 24, scope: !34)
!39 = !DILocation(line: 28, column: 10, scope: !34)
!40 = !DILocation(line: 29, column: 1, scope: !34)
!41 = distinct !DISubprogram(name: "register_device", scope: !3, file: !3, line: 31, type: !15, scopeLine: 32, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!42 = !DILocalVariable(name: "dev", arg: 1, scope: !41, file: !3, line: 31, type: !10)
!43 = !DILocation(line: 31, column: 37, scope: !41)
!44 = !DILocalVariable(name: "new_node", scope: !41, file: !3, line: 34, type: !6)
!45 = !DILocation(line: 34, column: 18, scope: !41)
!46 = !DILocation(line: 34, column: 44, scope: !41)
!47 = !DILocation(line: 34, column: 29, scope: !41)
!48 = !DILocation(line: 37, column: 22, scope: !41)
!49 = !DILocation(line: 37, column: 5, scope: !41)
!50 = !DILocation(line: 37, column: 15, scope: !41)
!51 = !DILocation(line: 37, column: 20, scope: !41)
!52 = !DILocation(line: 40, column: 22, scope: !41)
!53 = !DILocation(line: 40, column: 5, scope: !41)
!54 = !DILocation(line: 40, column: 15, scope: !41)
!55 = !DILocation(line: 40, column: 20, scope: !41)
!56 = !DILocation(line: 43, column: 24, scope: !41)
!57 = !DILocation(line: 43, column: 22, scope: !41)
!58 = !DILocation(line: 44, column: 1, scope: !41)
!59 = distinct !DISubprogram(name: "open", scope: !3, file: !3, line: 46, type: !60, scopeLine: 46, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!60 = !DISubroutineType(types: !61)
!61 = !{!21, !10}
!62 = !DILocalVariable(name: "dev", arg: 1, scope: !59, file: !3, line: 46, type: !10)
!63 = !DILocation(line: 46, column: 25, scope: !59)
!64 = !DILocation(line: 47, column: 2, scope: !59)
!65 = !DILocation(line: 47, column: 7, scope: !59)
!66 = !DILocation(line: 47, column: 12, scope: !59)
!67 = !DILocation(line: 48, column: 26, scope: !59)
!68 = !DILocation(line: 48, column: 31, scope: !59)
!69 = !DILocation(line: 48, column: 5, scope: !59)
!70 = !DILocation(line: 49, column: 5, scope: !59)
!71 = distinct !DISubprogram(name: "device_reset", scope: !3, file: !3, line: 52, type: !72, scopeLine: 52, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!72 = !DISubroutineType(types: !73)
!73 = !{null, !74}
!74 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!75 = !DILocalVariable(name: "dev_init", arg: 1, scope: !71, file: !3, line: 52, type: !74)
!76 = !DILocation(line: 52, column: 25, scope: !71)
!77 = !DILocation(line: 53, column: 6, scope: !71)
!78 = !DILocation(line: 53, column: 15, scope: !71)
!79 = !DILocation(line: 54, column: 1, scope: !71)
!80 = distinct !DISubprogram(name: "lookup_device", scope: !3, file: !3, line: 56, type: !81, scopeLine: 57, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!81 = !DISubroutineType(types: !82)
!82 = !{!10, !18}
!83 = !DILocalVariable(name: "search_name", arg: 1, scope: !80, file: !3, line: 56, type: !18)
!84 = !DILocation(line: 56, column: 38, scope: !80)
!85 = !DILocalVariable(name: "current", scope: !80, file: !3, line: 58, type: !6)
!86 = !DILocation(line: 58, column: 18, scope: !80)
!87 = !DILocation(line: 58, column: 28, scope: !80)
!88 = !DILocation(line: 59, column: 5, scope: !80)
!89 = !DILocation(line: 59, column: 12, scope: !80)
!90 = !DILocation(line: 59, column: 20, scope: !80)
!91 = !DILocation(line: 61, column: 20, scope: !92)
!92 = distinct !DILexicalBlock(scope: !93, file: !3, line: 61, column: 13)
!93 = distinct !DILexicalBlock(scope: !80, file: !3, line: 60, column: 5)
!94 = !DILocation(line: 61, column: 29, scope: !92)
!95 = !DILocation(line: 61, column: 34, scope: !92)
!96 = !DILocation(line: 61, column: 39, scope: !92)
!97 = !DILocation(line: 61, column: 13, scope: !92)
!98 = !DILocation(line: 61, column: 52, scope: !92)
!99 = !DILocation(line: 61, column: 13, scope: !93)
!100 = !DILocation(line: 62, column: 20, scope: !92)
!101 = !DILocation(line: 62, column: 29, scope: !92)
!102 = !DILocation(line: 62, column: 13, scope: !92)
!103 = !DILocation(line: 64, column: 19, scope: !92)
!104 = !DILocation(line: 64, column: 28, scope: !92)
!105 = !DILocation(line: 64, column: 17, scope: !92)
!106 = distinct !{!106, !88, !107}
!107 = !DILocation(line: 65, column: 5, scope: !80)
!108 = !DILocation(line: 66, column: 5, scope: !80)
!109 = !DILocation(line: 68, column: 1, scope: !80)
!110 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 95, type: !111, scopeLine: 95, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!111 = !DISubroutineType(types: !112)
!112 = !{!21}
!113 = !DILocalVariable(name: "dev", scope: !110, file: !3, line: 96, type: !10)
!114 = !DILocation(line: 96, column: 21, scope: !110)
!115 = !DILocation(line: 97, column: 10, scope: !110)
!116 = !DILocation(line: 97, column: 5, scope: !110)
!117 = !DILocation(line: 98, column: 5, scope: !110)
!118 = !DILocation(line: 99, column: 5, scope: !110)
!119 = !DILocation(line: 100, column: 5, scope: !110)
!120 = !DILocation(line: 102, column: 11, scope: !110)
!121 = !DILocation(line: 102, column: 9, scope: !110)
!122 = !DILocation(line: 103, column: 10, scope: !110)
!123 = !DILocation(line: 103, column: 5, scope: !110)
!124 = !DILocation(line: 104, column: 11, scope: !110)
!125 = !DILocation(line: 104, column: 9, scope: !110)
!126 = !DILocation(line: 105, column: 10, scope: !110)
!127 = !DILocation(line: 105, column: 5, scope: !110)
!128 = !DILocation(line: 107, column: 5, scope: !110)
!129 = distinct !DISubprogram(name: "dev1_open", scope: !3, file: !3, line: 73, type: !15, scopeLine: 73, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!130 = !DILocalVariable(name: "dev", arg: 1, scope: !129, file: !3, line: 73, type: !10)
!131 = !DILocation(line: 73, column: 38, scope: !129)
!132 = !DILocation(line: 74, column: 19, scope: !129)
!133 = !DILocation(line: 74, column: 24, scope: !129)
!134 = !DILocation(line: 74, column: 5, scope: !129)
!135 = !DILocation(line: 75, column: 1, scope: !129)
!136 = distinct !DISubprogram(name: "dev2_open", scope: !3, file: !3, line: 84, type: !15, scopeLine: 84, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!137 = !DILocalVariable(name: "dev", arg: 1, scope: !136, file: !3, line: 84, type: !10)
!138 = !DILocation(line: 84, column: 38, scope: !136)
!139 = !DILocation(line: 85, column: 2, scope: !136)
!140 = !DILocation(line: 85, column: 7, scope: !136)
!141 = !DILocation(line: 85, column: 29, scope: !136)
!142 = !DILocation(line: 86, column: 1, scope: !136)
